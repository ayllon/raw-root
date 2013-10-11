#include <TClass.h>
#include <TCollection.h>
#include <TDataMember.h>
#include <TKey.h>
#include <TMemberInspector.h>
#include <TString.h>
#include "TypeResolver.hpp"
#include "Walker.hpp"

using namespace scidb::root;


/// Generic TObject handler. This is intended to be a fallback for types non
/// convertible directly to arrays or basic types.
/// It is registered by the "core" part directly last, so any more detailed
/// implementation is checked.
class TObjectHandler: public TMemberInspector, public ITypeHandler {
public:
    
    TObjectHandler(TypeResolver& resolver): resolver(resolver), visitor(nullptr)
    {
    }
    
    
    bool recognize(const std::string& typeName)
    {
        return TClass::GetClass(typeName.c_str())->InheritsFrom("TObject");
    }
    
    
    void inspect(const std::string& typeName, bool isPointer, 
                 const std::string& name, const void* addr,
                 IVisitor& visitor)
    {
        TObject* obj = (TObject*)(addr);
        
        this->visitor = &visitor;
        if (visitor.pre(typeName, false, name) && !isPointer)
            obj->ShowMembers(*this);
        visitor.post(typeName, false, name);
    }
    
    
    /// Overloaded method
    /// @param klass  The class of the object being inspected
    /// @param parent The parent of the current attribute
    /// @param name   The name of the current attribute
    /// @param addr   A pointer to the value
    void Inspect(TClass* klass, const char* parent, const char* name, const void* addr)
    {
        // We don't want to recurse into members... yet
        if (parent && parent[0])
            return;
        
        TDataMember* member     = klass->GetDataMember(name);
        std::string  memberType = member->GetTypeName();
        
        // Basic types can not be traversed, so they are leafs
        if (member->IsBasic()) {
            this->visitor->leaf(name, Data(memberType, addr));
        }
        // Consider TString a basic type
        else if (memberType == "TString") {
            this->visitor->leaf(name, Data(memberType, addr));
        }
        // Complex types
        else {
            ITypeHandler* handler = resolver.getHandlerForType(memberType);
            if (handler)
                handler->inspect(memberType, member->IsaPointer(),
                                 name, addr,
                                 *(this->visitor));
            else
                this->visitor->unknown(name, memberType);
        }
    }


protected:
    TypeResolver& resolver;
    IVisitor* visitor;
};


Walker::Walker(const TFile& file): file(file)
{
}


void Walker::walk(IVisitor& visitor)
{
    TypeResolver typeResolver("/home/aalvarez/Sources/scidb-root/build/playground/visitor/handlers/");
    typeResolver.registerHandler(new TObjectHandler(typeResolver));
    
    Int_t  nkeys = this->file.GetNkeys();
    TList* keys  = this->file.GetListOfKeys();
    
    for (Int_t i = 0; i < nkeys; ++i) {
        TKey* key = static_cast<TKey*>(keys->At(i));
        TObject* obj = key->ReadObj();
        ITypeHandler* handler = typeResolver.getHandlerForType(key->GetClassName());
        if (handler)
            handler->inspect(key->GetClassName(), false, obj->GetName(), obj, visitor);
        else
            visitor.unknown(obj->GetName(), key->GetClassName());
    }
}
