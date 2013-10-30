#include <sstream>
#include "TypeResolver.hpp"
#include "Walker.hpp"

using namespace scidb::root;



Walker::Walker(TypeResolver& typeResolver):
    typeResolver(typeResolver)
{
}


void Walker::walk(const void* obj, const std::string& objTypeName, IVisitor& visitor)
{   
    ITypeHandler* handler = typeResolver.getHandlerForType(objTypeName);
    if (handler)
        handler->inspect(objTypeName, false, "/", obj, visitor);
    else
        visitor.unknown(objTypeName, "/", obj);
}


class PickerVisitor: public IVisitor
{
private:
    const void ** nodePtr;
    std::string *nodeTypeName;
    
    std::vector<std::string> pathComponents;
    size_t depth, maxDepth;
    
public:
    PickerVisitor(const std::string& path, const void** ptr, std::string* typeName):
        nodePtr(ptr), nodeTypeName(typeName), depth(0)
    {
        *ptr = nullptr;
        nodeTypeName->clear();
        
        std::istringstream helper(path);
        std::string c;
        pathComponents.push_back("/");
        while (std::getline(helper, c, '.'))
            pathComponents.push_back(c);

        maxDepth = pathComponents.size() - 1;
    }
    
    bool foundNode(void)
    {
        return *nodePtr != nullptr;
    }
    
    bool pre(const std::string& typeName, bool isArray, const std::string& objName, const void* obj)
    {
        bool recurse = false;
        if (*nodePtr == nullptr && depth <= maxDepth) {
            std::string lookingFor = pathComponents[depth];
            
            if (depth == maxDepth && lookingFor == objName) {
                *nodePtr = obj;
                *nodeTypeName = typeName;
                recurse = false;
            }
            else {
                recurse = (objName == lookingFor);
            }
        }
        ++depth;
    }
    
    void post(const std::string& typeName, bool isArray, const std::string& objName, const void* obj)
    {
        --depth;
    }
    
    void unknown(const std::string& typeName, const std::string& objName, const void* obj)
    {
        if (*nodePtr == nullptr && depth <= maxDepth) {
            std::string lookingFor = pathComponents[depth];
            
            if (depth == maxDepth && lookingFor == objName) {
                *nodePtr = obj;
                *nodeTypeName = typeName;
            }
        }
    }
    
    void leaf(const std::string& name, const Data& data)
    {
        if (*nodePtr == nullptr && depth <= maxDepth) {
            std::string lookingFor = pathComponents[depth];
            
            if (depth == maxDepth && lookingFor == name) {
                *nodePtr = data.getRawPointer();
                *nodeTypeName = data.getTypeName();
            }
        }
    }
    
    void leaf(size_t index, const Data& data)
    {
        if (*nodePtr == nullptr && depth <= maxDepth) {
            std::string lookingFor = pathComponents[depth];
            
            std::ostringstream indexStr;
            indexStr << index;
            if (depth == maxDepth && lookingFor == indexStr.str()) {
                *nodePtr = data.getRawPointer();
                *nodeTypeName = data.getTypeName();
            }
        }
    }
};


bool Walker::getNode(const void* root, const std::string& rootType,
    const std::string& path, const void** ptr, std::string* typeName)
{
    ITypeHandler* handler = typeResolver.getHandlerForType(rootType);
    if (!handler)
        return false;
    
    PickerVisitor picker(path, ptr, typeName);
    handler->inspect(rootType, false, "/", root, picker);
    return picker.foundNode();
}
