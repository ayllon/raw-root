#include <TClass.h>
#include "../../TypeHandler.hpp"
#include "../../TypeResolver.hpp"

using namespace raw::root;

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
    
    
    std::string getHandlerId()
    {
        return "TCollectionHandler";
    }
    
    
    bool recognize(const std::string& typeName)
    {
        TClass* klass = TClass::GetClass(typeName.c_str());
        return klass && klass->InheritsFrom("TCollection");
    }
    
    
    void inspect(std::shared_ptr<Node> node, IVisitor* visitor)
    {
        node->setArray();
        if (visitor->pre(node) && !node->isPointer()) {
            const TCollection* collection = static_cast<const TCollection*>(node->getAddress());
            this->iterate(collection, visitor);
        }
        visitor->post(node);
    }
    
    
    void iterate(const TCollection* collection, IVisitor* visitor)
    {
        if (!collection)
            return;
        
        TIterator* iterator = collection->MakeIterator();
        const TObject* obj;
        
        while ((obj = iterator->Next())) {
            std::string objType = obj->ClassName();
            
            std::shared_ptr<Node> node(new Node(objType, obj->GetName(), obj));
            
            ITypeHandler* handler = resolver->getHandlerForType(objType);
            if (handler)
                handler->inspect(node, visitor);
            else
                visitor->unknown(node);
        }
    }
};

/// This method must register the type handlers
extern "C" void registerTypes(TypeResolver* resolver)
{
    resolver->registerHandler(new TCollectionHandler(resolver));
}
