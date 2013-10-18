#include <TClass.h>
#include <TDataMember.h>
#include "TObjectHandler.hpp"
#include "TypeResolver.hpp"

using namespace scidb::root;


TObjectHandler::TObjectHandler(TypeResolver& resolver):
    resolver(resolver), visitor(nullptr)
{
}


bool TObjectHandler::recognize(const std::string& typeName)
{
    TClass* klass = TClass::GetClass(typeName.c_str());
    return klass && klass->InheritsFrom("TObject");
}


void TObjectHandler::inspect(const std::string& typeName, bool isPointer, 
                             const std::string& name, const void* addr,
                             IVisitor& visitor)
{
    TObject* obj = (TObject*)(addr);
    
    this->visitor = &visitor;
    if (visitor.pre(typeName, false, name) && !isPointer)
        obj->ShowMembers(*this);
    visitor.post(typeName, false, name);
}


void TObjectHandler::Inspect(TClass* klass, const char* parent, const char* name, const void* addr)
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
            this->visitor->unknown(memberType, name);
    }
}
