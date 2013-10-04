#include <TClass.h>
#include <TCollection.h>
#include <TDataMember.h>
#include <TKey.h>
#include <TMemberInspector.h>
#include <TString.h>
#include <TVector.h>
#include <vector>
#include "Walker.hpp"

using namespace scidb::root;

/// Return true if the type is an array (or equivalent)
static bool isArray(const std::string& type)
{
    return type.compare(0, 7, "TVector") == 0 ||
           type.compare(0, 9, "TVectorT<") == 0 ||
           type.compare(0, 7, "vector<") == 0 ||
           TClass::GetClass(type.c_str())->InheritsFrom("TCollection");
}

/// Put into container the container type, and into contained, the contained type
/// For instance
///      fullType = vector<int>
///      container = vector
///      contained = int
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

/// Generic template for an interator from the standard library
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

/// Partially specialized iterator for TVectorT type from Root
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

/// Implementation of TMemberInspector, plus additional methods, to inspect a Root file/object
class RootInspector: public TMemberInspector {
public:
    
    /// Constructor
    RootInspector(IVisitor& visitor, void* ptr): visitor(visitor), ptr(ptr)
    {
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
    
    /// Inspect a complext object (not basic)
    /// @param type      The typename of the object
    /// @param isPointer If the value actually is a pointer, rather than an object
    /// @param name      The name of the object (i.e. attribute name)
    /// @param addr      A pointer to the value
    void InspectComplex(const std::string& type, Bool_t isPointer,
                        const std::string& name, const void* addr)
    {
        bool array = isArray(type);
        
        bool descend = this->visitor.pre(type, array, name, this->ptr);
        if (descend && !isPointer) {
            if (array) {
                this->InspectArray(type, addr);
            }
            else if (TClass::GetClass(type.c_str())->InheritsFrom("TObject")) {
                TObject* obj = (TObject*)(addr);
                obj->ShowMembers(*this);
            }
        }
        
        this->visitor.post(type, array, name, this->ptr);
    }
    
    void IterateCollection(const void* addr)
    {
        const TCollection* collection = static_cast<const TCollection*>(addr);
        if (!collection)
            return;
        TIterator* iterator = collection->MakeIterator();
        const TObject* obj;
        size_t i = 0;
        while ((obj = iterator->Next())) {
            this->InspectComplex(obj->ClassName(), false, obj->GetName(), obj);
        }
        delete iterator;
    }
    
    /// Inspect an array
    /// @param type The full array type
    /// @param addr A pointer to the value
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
        // TCollection types
        else if (TClass::GetClass(containerTypeName.c_str())->InheritsFrom("TCollection")) {
            IterateCollection(addr);
        }
    }

    /// To iterate an array, we need to know the actual type of the contained type too
    /// (otherwise we can not iterate it!)
    /// Thanks to the ArrayIterator template magic, we can reuse this logic for both
    /// root container and standard containers.
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
                // Add additional cases to support other basic types.
                // May need to iterate array of objects in the future.
                // That's trickier...
                break;
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
