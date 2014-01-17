#include <TPolyLine.h>
#include "../../TypeHandler.hpp"
#include "../../TypeResolver.hpp"

using namespace raw::root;


class TPolyLineHandler: public ITypeHandler
{
public:
    
    std::string getHandlerId()
    {
        return "TPolyLineHandler";
    }
    
    
    bool recognize(const std::string& typeName)
    {
        return typeName.compare("TPolyLine") == 0;
    }
    
    
    void inspect(std::shared_ptr<Node> node, std::shared_ptr<IVisitor> visitor)
    {
        node->setType(Node::kDictionary);
        if (visitor->pre(node)) {
            this->visit("x", visitor);
            this->visit("y", visitor);
        }
        visitor->post(node);
    }


    void visit(const char* name, std::shared_ptr<IVisitor> visitor)
    {
        std::shared_ptr<Node> arrayContentNode(new Node(Node::kDouble));
        visitor->pre(arrayContentNode);
        visitor->post(arrayContentNode);
    }
};


/// This method must register the type handlers
extern "C" void registerTypes(TypeResolver* resolver)
{
    resolver->registerHandler(new TPolyLineHandler());
}
