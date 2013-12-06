#include <TBasket.h>
#include <TClass.h>
#include <TLeaf.h>
#include <TTree.h>
#include <boost/concept_check.hpp>
#include "../../TypeHandler.hpp"
#include "../../TypeResolver.hpp"

using namespace raw::root;


static void getContainerType(const std::string& fullType,
                             std::string* container,
                             std::string* contained)
{
    size_t templateIndex = fullType.find('<');
    size_t fullTypeLen   = fullType.length();

    if (templateIndex == std::string::npos) {
        container->assign(fullType);
        contained->clear();
    }

    // The -2 is because of the "< >"
    size_t containedTypeLen = fullTypeLen - templateIndex - 2;

    container->assign(fullType.substr(0, templateIndex));
    contained->assign(fullType.substr(templateIndex + 1, containedTypeLen));
}


class TTreeHandler: public ITypeHandler
{
protected:
	TypeResolver* resolver;

public:

	TTreeHandler(TypeResolver* resolver): resolver(resolver)
	{
	}
    
    std::string getHandlerId()
    {
        return "TTreeHandler";
    }
    
    
    bool isTree(const std::string& typeName)
    {
        TClass* klass = TClass::GetClass(typeName.c_str());
        return klass && klass->InheritsFrom("TTree");
    }

    bool isLeaf(const std::string& typeName)
    {
        return typeName.compare(0, 6, "TLeaf<") == 0;
    }
    
    bool recognize(const std::string& typeName)
    {
        return isTree(typeName) || isLeaf(typeName);
    }
    
    void inspect(std::shared_ptr<Node> node, std::shared_ptr<IVisitor> visitor)
    {
        if (isTree(node->getTypeName())) {
            node->setType(Node::kDictionary);
            if (visitor->pre(node)) {
                TTree* tree = (TTree*)(node->getAddress());
                this->inspectTree(tree, visitor);
            }
            visitor->post(node);
        }
        else if (isLeaf(node->getTypeName())) {
            node->setType(Node::kCollection);
            if (visitor->pre(node)) {
                std::string container, contained;
                getContainerType(node->getTypeName(), &container, &contained);
                std::shared_ptr<Node> containedNode(new Node(contained));
                visitor->pre(containedNode);
                visitor->post(containedNode);
            }
            visitor->post(node);
        }
    }
    
    void inspectTree(TTree* tree, std::shared_ptr<IVisitor> visitor)
    {
        TObjArray* leaves = tree->GetListOfLeaves();
        TIterator* it = leaves->MakeIterator();
        TObject *obj;
        while ((obj = it->Next()) != NULL) {
            TLeaf* leaf = (TLeaf*)obj;
            std::shared_ptr<Node> leafNode(new Node(std::string("TLeaf<") + leaf->GetTypeName() + ">", Node::kCollection, leaf->GetName()));
            if (visitor->pre(leafNode)) {
                std::shared_ptr<Node> containedNode(new Node(leaf->GetTypeName()));

                if (containedNode->isBasic()) {
                	visitor->pre(containedNode);
					visitor->post(containedNode);
                }
                else {
                	ITypeHandler* handler = resolver->getHandlerForType(leaf->GetTypeName());
                	if (!handler) {
                		visitor->unknown(containedNode);
                	}
                	else {
                		handler->inspect(containedNode, visitor);
                	}
                }
            }
            visitor->post(leafNode);
        }
    }
};


/// This method must register the type handlers
extern "C" void registerTypes(TypeResolver* resolver)
{
    resolver->registerHandler(new TTreeHandler(resolver));
}
