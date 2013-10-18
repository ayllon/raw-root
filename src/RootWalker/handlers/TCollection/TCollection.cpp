#include <TClass.h>
#include "../../TypeHandler.hpp"
#include "../../TypeResolver.hpp"

using namespace scidb::root;

class TCollectionHandler: public ITypeHandler
{
protected:
    TypeResolver* resolver;
    
public:
    TCollectionHandler(TypeResolver* resolver): resolver(resolver)
    {
    }
    
    
    ~TCollectionHandler()
    {
    }
    
    
    bool recognize(const std::string& typeName)
    {
        TClass* klass = TClass::GetClass(typeName.c_str());
        return klass && klass->InheritsFrom("TCollection");
    }
    
    
    void inspect(const std::string& typeName, bool isPointer,
                 const std::string& name, const void* addr,
                 IVisitor& visitor)
    {
        if (visitor.pre(typeName, true, name) && !isPointer) {
            const TCollection* collection = static_cast<const TCollection*>(addr);
            this->iterate(collection, visitor);
        }
        visitor.post(typeName, true, name);
    }
    
    
    void iterate(const TCollection* collection, IVisitor& visitor)
    {
        if (!collection)
            return;
        
        TIterator* iterator = collection->MakeIterator();
        const TObject* obj;
        size_t i = 0;
        
        while ((obj = iterator->Next())) {
            std::string objType = obj->ClassName();
            ITypeHandler* handler = resolver->getHandlerForType(objType);
            if (handler)
                handler->inspect(objType, false, obj->GetName(), obj, visitor);
            else
                visitor.unknown(objType, obj->GetName());
        }
    }
};

/// This method must register the type handlers
extern "C" void registerTypes(TypeResolver* resolver)
{
    resolver->registerHandler(new TCollectionHandler(resolver));
}
