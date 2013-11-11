#include <TClass.h>
#include <TDataMember.h>
#include "TObjectHandler.hpp"
#include "TypeResolver.hpp"

using namespace scidb::root;


TObjectHandler::TObjectHandler(TypeResolver& resolver):
    resolver(resolver), visitor(nullptr)
{
}


std::string TObjectHandler::getHandlerId()
{
    return "TObjectHandler";
}


bool TObjectHandler::recognize(const std::string& typeName)
{
    TClass* klass = TClass::GetClass(typeName.c_str());
    return klass && klass->InheritsFrom("TObject");
}


void TObjectHandler::inspect(const Node& node, IVisitor& visitor)
{
    if (visitor.pre(node)) {
        this->visitor = &visitor;
        ((TObject*)node.getAddress())->ShowMembers(*this);
    }
    visitor.post(node);
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
        Node thisNode(memberType, member->IsaPointer(), name, addr);
        ITypeHandler* handler = resolver.getHandlerForType(memberType);
        if (handler && !member->IsaPointer())
            handler->inspect(thisNode, *this->visitor);
        else
            this->visitor->unknown(thisNode);
    }
}
