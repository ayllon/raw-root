#include <TClass.h>
#include <TKey.h>
#include <TDirectory.h>
#include "../../TypeHandler.hpp"
#include "../../TypeResolver.hpp"

using namespace scidb::root;

class TDirectoryHandler: public ITypeHandler
{
protected:
    TypeResolver* resolver;
    
public:
    TDirectoryHandler(TypeResolver* resolver): resolver(resolver)
    {
    }
    
    
    ~TDirectoryHandler()
    {
    }
    
    
    bool recognize(const std::string& typeName)
    {
        TClass* klass = TClass::GetClass(typeName.c_str());
        return klass && klass->InheritsFrom("TDirectory");
    }
    
    
    void inspect(const std::string& typeName, bool isPointer,
                 const std::string& name, const void* addr,
                 IVisitor& visitor)
    {
        if (visitor.pre(typeName, true, name) && !isPointer) {
            const TDirectory* dir = static_cast<const TDirectory*>(addr);
            this->iterate(dir, visitor);
        }
        visitor.post(typeName, true, name);
    }
    
    
    void iterate(const TDirectory* dir, IVisitor& visitor)
    {
        if (!dir)
            return;
        
        Int_t nKeys = dir->GetNkeys();
        TList* keys = dir->GetListOfKeys();
        for (Int_t i = 0; i < nKeys; ++i) {
            TKey*    key = static_cast<TKey*>(keys->At(i));
            TObject* obj = key->ReadObj();
            ITypeHandler* handler = resolver->getHandlerForType(key->GetClassName());
            if (handler)
                handler->inspect(key->GetClassName(), false, obj->GetName(), obj, visitor);
            else
                visitor.unknown(obj->GetName(), key->GetClassName());
        }
    }
};

/// This method must register the type handlers
extern "C" void registerTypes(TypeResolver* resolver)
{
    resolver->registerHandler(new TDirectoryHandler(resolver));
}
