#include <TVector.h>
#include <vector>
#include "../../Data.hpp"
#include "../../TypeHandler.hpp"
#include "../../TypeResolver.hpp"

using namespace raw::root;


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
    static void iterate(const void* addr, const std::string& elementType, IVisitor& visitor)
    {
        const CONTAINER<T> *v = static_cast<const CONTAINER<T>*>(addr);
        size_t nElements = v->size();
        for (size_t i = 0; i < nElements; ++i)
            visitor.leaf(i, Data(elementType, &((*v)[i])));
    }   
};


/// Partially specialized iterator for TVectorT type from Root
template <typename T>
struct ArrayIterator<TVectorT, T>
{
    static void iterate(const void* addr, const std::string& elementType, IVisitor& visitor)
    {
        const TVectorT<T> *v = static_cast<const TVectorT<T>*>(addr);
        size_t nElements = v->GetNoElements();
        for (size_t i = 0; i < nElements; ++i)
            visitor.leaf(i, Data(elementType, &((*v)[i])));
    }   
};


/// Implementation of IHandler for vector types
class VectorHandler: public ITypeHandler
{
public:
    ~VectorHandler()
    {
    }
    
    
    std::string getHandlerId()
    {
        return "VectorHandler";
    }
    
    
    bool recognize(const std::string& typeName)
    {
        return typeName.compare(0, 7, "TVector") == 0 ||
               typeName.compare(0, 9, "TVectorT<") == 0 ||
               typeName.compare(0, 7, "vector<") == 0;
    }
    
    
    void inspect(const Node& node, IVisitor& visitor)
    {
        std::string containerTypeName;
        std::string containedTypeName;
        getContainerType(node.getTypeName(), &containerTypeName, &containedTypeName);
        
        ArrayNode arrayNode(node, containedTypeName);
        
        if (visitor.pre(arrayNode) && !node.isPointer()) {
            DataType containedType = Data::typeFromStr(containedTypeName);
            
            // TVector is a typedef ot TVector<Float_t>
            if (containerTypeName == "TVector") {
                IterateGenericArray<TVectorT>(node.getAddress(), "Float_t", visitor);
            }
            // TVectorT types
            else if (containerTypeName == "TVectorT") {
                IterateGenericArray<TVectorT>(node.getAddress(), containedTypeName, visitor);
            }
            // std::vector types
            else if (containerTypeName == "vector") {
                IterateGenericArray<std::vector>(node.getAddress(), containedTypeName, visitor);
            }
        }
        visitor.post(arrayNode);
    }
    
    
    /// To iterate an array, we need to know the actual type of the contained type too
    /// (otherwise we can not iterate it!)
    /// Thanks to the ArrayIterator template magic, we can reuse this logic for both
    /// root container and standard containers.
    template <template <typename ...> class CONTAINER>
    void IterateGenericArray(const void* addr, const std::string& elementType,
                             IVisitor& visitor)
    {
        DataType containedType = Data::typeFromStr(elementType);
        switch (containedType) {
            case kInt32:
                ArrayIterator<CONTAINER, int32_t>::iterate(addr, elementType, visitor);
                break;
            case kInt64:
                ArrayIterator<CONTAINER, int64_t>::iterate(addr, elementType, visitor);
                break;
            case kFloat:
                ArrayIterator<CONTAINER, float>::iterate(addr, elementType, visitor);
                break;
            default:
                // Add additional cases to support other basic types.
                // May need to iterate array of objects in the future.
                // That's trickier...
                break;
        }
    }
};

/// This method must register the type handlers
extern "C" void registerTypes(TypeResolver* resolver)
{
    resolver->registerHandler(new VectorHandler());
}
