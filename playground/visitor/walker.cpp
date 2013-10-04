#include <TClass.h>
#include <TDataMember.h>
#include <TKey.h>
#include <TMemberInspector.h>
#include <TString.h>
#include <TVector.h>
#include <boost/iterator/iterator_concepts.hpp>
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
        // Array types
        else if (isArray(memberType)) {
            this->visitor.pre(memberType, true, name, this->ptr);
            this->InspectArray(memberType, addr);
            this->visitor.post(memberType, true, name, this->ptr);
        }
        // Complex types
        else {
            this->visitor.pre(memberType, false, name, this->ptr);
            this->visitor.post(memberType, false, name, this->ptr);
        }
    }
    
    template <class A>
    void InspectStdArray(const void* addr, const std::string& elementType)
    {
        const A& array = *(static_cast<const A*>(addr));
        size_t nElements = array.size();
        for (size_t i = 0; i < nElements; ++i) {
            this->visitor.leaf(i, Data(elementType, &(array[i])), this->ptr);
        }
    }
    
    template <class A>
    void InspectRootArray(const void* addr, const std::string& elementType)
    {
        const A& array = *(static_cast<const A*>(addr));
        size_t nElements = array.GetNoElements();
        for (size_t i = 0; i < nElements; ++i) {
            this->visitor.leaf(i, Data(elementType, &(array[i])), this->ptr);
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
            InspectRootArray<TVector>(addr, "Float_t");
        }
        // std::vector types
        else if (containerTypeName == "vector") {
            switch (containedType) {
                case kInt32:
                    InspectStdArray<std::vector<int32_t> >(addr, containedTypeName);
                    break;
                case kInt64:
                    InspectStdArray<std::vector<int64_t> >(addr, containedTypeName);
                    break;
                default:
                    // Nothing
                    break;
            }
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
            visitor.pre(key->GetClassName(), false, obj->GetName(), ptr);
            obj->ShowMembers(inspector);
            visitor.post(key->GetClassName(), false, obj->GetName(), ptr);
        }
    }
}
