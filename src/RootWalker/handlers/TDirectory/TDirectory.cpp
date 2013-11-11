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
    
    
    std::string getHandlerId()
    {
        return "TDirectoryHandler";
    }
    
    
    bool recognize(const std::string& typeName)
    {
        TClass* klass = TClass::GetClass(typeName.c_str());
        return klass && klass->InheritsFrom("TDirectory");
    }
    
    
    void inspect(const Node& node, IVisitor& visitor)
    {
        if (visitor.pre(ArrayNode(node)) && !node.isPointer()) {
            const TDirectory* dir = static_cast<const TDirectory*>(node.getAddress());
            this->iterate(dir, visitor);
        }
        visitor.post(ArrayNode(node));
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
            Node node(key->GetClassName(), obj->GetName(), obj);
            if (handler)
                handler->inspect(node, visitor);
            else
                visitor.unknown(node);
        }
    }
};

/// This method must register the type handlers
extern "C" void registerTypes(TypeResolver* resolver)
{
    resolver->registerHandler(new TDirectoryHandler(resolver));
}
