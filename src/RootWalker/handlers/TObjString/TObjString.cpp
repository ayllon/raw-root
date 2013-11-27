#include <TClass.h>
#include <TObjString.h>
#include "../../TypeHandler.hpp"
#include "../../TypeResolver.hpp"

using namespace raw::root;


class TObjStringHandler: public ITypeHandler
{
public:
    
    std::string getHandlerId()
    {
        return "TObjStringHandler";
    }


    bool recognize(const std::string& typeName)
    {
        TClass* klass = TClass::GetClass(typeName.c_str());
        return klass && klass->InheritsFrom("TObjString");
    }


    void inspect(std::shared_ptr<Node> node, std::shared_ptr<IVisitor> visitor)
    {
        node->setType(Node::kString);
        visitor->pre(node);
        visitor->post(node);
    }
};


/// This method must register the type handlers
extern "C" void registerTypes(TypeResolver* resolver)
{
    resolver->registerHandler(new TObjStringHandler());
}
