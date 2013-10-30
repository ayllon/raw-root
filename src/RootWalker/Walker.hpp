#ifndef __WALKER_HPP
#define __WALKER_HPP

#include <TFile.h>
#include "Data.hpp"

namespace scidb {
namespace root {
    
class TypeResolver;

/// Visitor interface.
/// This must be implemented by the client, so the Walker can call the corresponding
/// methods.
class IVisitor
{
public:
    /// Called before desdencing into a data member
    /// @param typeName The object type name
    /// @param isArray  If the object is an array or equivalent
    /// @param objName  The object name
    /// @param obj      A pointer to the object
    /// @return         If it returns false, the walker won't descend into the children
    virtual bool pre(const std::string& typeName, bool isArray, const std::string& objName, const void* obj) = 0;
    
    /// Called after processing a data member
    /// @param typeName The object type name
    /// @param isArray  If the object is an array or equivalent
    /// @param objName  The object name
    /// @param obj      A pointer to the object
    virtual void post(const std::string& typeName, bool isArray, const std::string& objName, const void* obj) = 0;
    
    /// Called when an element of unknown type is found.
    /// @param typeName The unknown type
    /// @param objName  The attribute name
    /// @param obj      A pointer to the object
    virtual void unknown(const std::string& typeName, const std::string& objName, const void* obj) = 0;
    
    /// Called for a basic type with a name. For instance, object attributes.
    /// @param name The attribute name
    /// @param data The attribute raw data
    virtual void leaf(const std::string& name, const Data& data) = 0;
    
    /// Called for a basic type within an array.
    /// @param index The attribute index within the array
    /// @param data  The attribute raw data
    virtual void leaf(size_t index, const Data& data) = 0;
};

/// Public interface
class Walker
{
public:
    /// Constructor
    /// @param typeResolver A TypeResolver
    Walker(TypeResolver& typeResolver);
    
    /// Perform the walk
    /// @param obj The root object to be traversed
    /// @param objTypeName The root object type name
    /// @param visitor The implementation of IVisitor
    void walk(const void *obj, const std::string& objTypeName, IVisitor& visitor);
    
    /// Convenience method to get a node
    /// Leave in ptr a pointer to the node, and fills typename with the node type name.
    /// Returns true if found
    /// i.e. Walker::getNode(file, "rootObj.attr.nestedAttr", &ptr, &typeName)
    bool getNode(const void* root, const std::string& rootType, const std::string& path,
                 const void** ptr, std::string* typeName);
    
protected:
    TypeResolver& typeResolver;
};

}}

#endif // __WALKER_HPP
