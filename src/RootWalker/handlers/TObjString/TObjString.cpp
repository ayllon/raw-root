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


    void inspect(const Node& node, IVisitor& visitor)
    {
        if (!node.isPointer() && node.getAddress()) {
            const TObjString* objStr = static_cast<const TObjString*>(node.getAddress());
            TString str = objStr->GetString();
            visitor.leaf(node.getName(), Data("TString", &str));
        }
    }
};


/// This method must register the type handlers
extern "C" void registerTypes(TypeResolver* resolver)
{
    resolver->registerHandler(new TObjStringHandler());
}
