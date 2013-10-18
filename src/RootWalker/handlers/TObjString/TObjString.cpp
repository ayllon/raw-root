#include <TClass.h>
#include <TObjString.h>
#include "../../TypeHandler.hpp"
#include "../../TypeResolver.hpp"

using namespace scidb::root;


class TObjStringHandler: public ITypeHandler
{
public:
    
    bool recognize(const std::string& typeName)
    {
        TClass* klass = TClass::GetClass(typeName.c_str());
        return klass && klass->InheritsFrom("TObjString");
    }
    
    
    void inspect(const std::string& typeName, bool isPointer, 
                 const std::string& name, const void* addr,
                 IVisitor& visitor)
    {
        if (!isPointer && addr) {
            const TObjString* objStr = static_cast<const TObjString*>(addr);
            TString str = objStr->GetString();
            visitor.leaf(name, Data("TString", &str));
        }
    }
};


/// This method must register the type handlers
extern "C" void registerTypes(TypeResolver* resolver)
{
    resolver->registerHandler(new TObjStringHandler());
}
