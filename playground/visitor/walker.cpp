#include <TClass.h>
#include <TDataMember.h>
#include <TKey.h>
#include <TMemberInspector.h>
#include <TString.h>
#include <TVector.h>
#include <vector>
#include "walker.hpp"

using namespace scidb::root;

static bool isArray(const std::string& type)
{
    return type.compare(0, 7, "TVector") == 0 ||
           type.compare(0, 9, "TVectorT<") == 0 ||
           type.compare(0, 7, "vector<") == 0;
}


static void getContainerType(const std::string& fullType,
                             std::string* container,
                             std::string* contained)
{
    size_t templateIndex = fullType.find('<');
    size_t fullTypeLen   = fullType.length();
    
    if (templateIndex == std::string::npos) {
        container->assign(fullType);
        contained->clear();
    }
    
    // The -2 is because of the "< >"
    size_t containedTypeLen = fullTypeLen - templateIndex - 2;
    
    container->assign(fullType.substr(0, templateIndex));
    contained->assign(fullType.substr(templateIndex + 1, containedTypeLen));
}

template <template <typename ...> class CONTAINER, typename T>
struct ArrayIterator {
    static void iterate(const void* addr, const std::string& elementType, IVisitor& visitor, void* ptr)
    {
        const CONTAINER<T> *v = static_cast<const CONTAINER<T>*>(addr);
        size_t nElements = v->size();
        for (size_t i = 0; i < nElements; ++i)
            visitor.leaf(i, Data(elementType, &((*v)[i])), ptr);
    }   
};

template <typename T>
struct ArrayIterator<TVectorT, T>
{
    static void iterate(const void* addr, const std::string& elementType, IVisitor& visitor, void* ptr)
    {
        const TVectorT<T> *v = static_cast<const TVectorT<T>*>(addr);
        size_t nElements = v->GetNoElements();
        for (size_t i = 0; i < nElements; ++i)
            visitor.leaf(i, Data(elementType, &((*v)[i])), ptr);
    }   
};


class RootInspector: public TMemberInspector {
public:
    RootInspector(IVisitor& visitor, void* ptr): visitor(visitor), ptr(ptr)
    {
    }
    
    void Inspect(TClass* klass, const char* parent, const char* name, const void* addr)
    {
        // We don't want to recurse into members... yet
        if (parent && parent[0])
            return;
        
        TDataMember* member     = klass->GetDataMember(name);
        std::string  memberType = member->GetTypeName();
        
        // Basic types can not be traversed, so they are leafs
        if (member->IsBasic()) {
            this->visitor.leaf(name, Data(memberType, addr), this->ptr);
        }
        // Consider TString a basic type
        else if (memberType == "TString") {
            this->visitor.leaf(name, Data(memberType, addr), this->ptr);
        }
        // Complext types
        else {
            this->InspectComplex(memberType, member->IsaPointer(), name, addr);
        }
    }
    
    void InspectComplex(const std::string& type, Bool_t isPointer,
                        const std::string& name, const void* addr)
    {
        if (isArray(type)) {
            this->visitor.pre(type, true, name, this->ptr);
            this->InspectArray(type, addr);
            this->visitor.post(type, true, name, this->ptr);
        }
        else if (!isPointer && TClass::GetClass(type.c_str())->InheritsFrom("TObject")) {
            this->visitor.pre(type, false, name, this->ptr);
            TObject* obj = (TObject*)(addr);
            obj->ShowMembers(*this);
            this->visitor.post(type, false, name, this->ptr);
        }
        else {
            this->visitor.pre(type, false, name, this->ptr);
            this->visitor.post(type, false, name, this->ptr);
        }
    }

    template <template <typename ...> class CONTAINER>
    void IterateGenericArray(const void* addr, const std::string& elementType)
    {
        DataType containedType = Data::typeFromStr(elementType);
        switch (containedType) {
            case kInt32:
                ArrayIterator<CONTAINER, int32_t>::iterate(addr, elementType, this->visitor, this->ptr);
                break;
            case kInt64:
                ArrayIterator<CONTAINER, int64_t>::iterate(addr, elementType, this->visitor, this->ptr);
                break;
            case kFloat:
                ArrayIterator<CONTAINER, float>::iterate(addr, elementType, this->visitor, this->ptr);
                break;
            default:
                // Nothing
                break;
        }
    }
    
    void InspectArray(const std::string& type, const void* addr)
    {
        std::string containerTypeName;
        std::string containedTypeName;
        getContainerType(type, &containerTypeName, &containedTypeName);
        
        DataType containedType = Data::typeFromStr(containedTypeName);
        
        // TVector is a typedef ot TVector<Float_t>
        if (containerTypeName == "TVector") {
            IterateGenericArray<TVectorT>(addr, "Float_t");
        }
        // TVectorT types
        else if (containerTypeName == "TVectorT") {
           IterateGenericArray<TVectorT>(addr, containedTypeName);
        }
        // std::vector types
        else if (containerTypeName == "vector") {
            IterateGenericArray<std::vector>(addr, containedTypeName);
        }
    }
    
protected:
    IVisitor& visitor;
    void* ptr;
};



Walker::Walker(const TFile& file): file(file)
{
}



void Walker::walk(IVisitor& visitor, void* ptr)
{
    RootInspector inspector(visitor, ptr);
    
    Int_t  nkeys = this->file.GetNkeys();
    TList* keys  = this->file.GetListOfKeys();
    
    for (Int_t i = 0; i < nkeys; ++i) {
        TKey* key = static_cast<TKey*>(keys->At(i));
        TObject* obj = key->ReadObj();
        if (obj) {
            inspector.InspectComplex(key->GetClassName(), false, obj->GetName(), obj);
        }
    }
}
