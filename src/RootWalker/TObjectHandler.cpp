#include <TClass.h>
#include <TDataMember.h>
#include "TObjectHandler.hpp"
#include "TypeResolver.hpp"

using namespace raw::root;


TObjectHandler::TObjectHandler(TypeResolver* resolver):
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


void TObjectHandler::inspect(const std::shared_ptr<Node> node, std::shared_ptr<IVisitor> visitor)
{
    node->setType(Node::kDictionary);
    if (visitor->pre(node)) {
        this->visitor = visitor;
        TObject* obj = ((TObject*)node->getAddress());
        if (obj)
            obj->ShowMembers(*this);
    }
    visitor->post(node);
}


void TObjectHandler::Inspect(TClass* klass, const char* parent, const char* name, const void* addr)
{
    // We don't want to recurse into members... yet
    if (parent && parent[0])
        return;
    
    TDataMember* member     = klass->GetDataMember(name);
    std::string  memberType = member->GetTypeName();
    
    // Basic types can not be traversed, so they are leafs
    // Consider TString a basic type
    if (member->IsBasic() || memberType == "TString") {
        std::shared_ptr<Node> node(new Node(memberType, name));
        this->visitor->pre(node);
        this->visitor->post(node);
    }
    // Complex types
    else {
        std::shared_ptr<Node> thisNode(new Node(memberType, name, addr));
        if (!member->IsaPointer()) {
            ITypeHandler* handler = resolver->getHandlerForType(memberType);
            if (handler && !member->IsaPointer())
                handler->inspect(thisNode, this->visitor);
            else
                this->visitor->unknown(thisNode);
        } else {
            this->visitor->unknown(thisNode);
        }
    }
}
