#include <TVector.h>
#include <vector>
#include "../../TypeHandler.hpp"
#include "../../TypeResolver.hpp"

using namespace raw::root;


/// Put into container the container type, and into contained, the contained type
/// For instance
///      fullType = vector<int>
///      container = vector
///      contained = int
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


/// Implementation of IHandler for vector types
class VectorHandler: public ITypeHandler
{
public:
    ~VectorHandler()
    {
    }
    
    
    std::string getHandlerId()
    {
        return "VectorHandler";
    }
    
    
    bool recognize(const std::string& typeName)
    {
        return typeName.compare(0, 7, "TVector") == 0 ||
               typeName.compare(0, 9, "TVectorT<") == 0 ||
               typeName.compare(0, 7, "vector<") == 0;
    }
    
    
    void inspect(std::shared_ptr<Node> node, std::shared_ptr<IVisitor> visitor)
    {
        std::string containerTypeName;
        std::string containedTypeName;
        getContainerType(node->getTypeName(), &containerTypeName, &containedTypeName);
        
        node->setType(Node::kCollection);
        if (visitor->pre(node)) {
            std::shared_ptr<Node> contained(new Node(containedTypeName));
            visitor->pre(contained);
            visitor->post(contained);
        }
        visitor->post(node);
    }
};

/// This method must register the type handlers
extern "C" void registerTypes(TypeResolver* resolver)
{
    resolver->registerHandler(new VectorHandler());
}
