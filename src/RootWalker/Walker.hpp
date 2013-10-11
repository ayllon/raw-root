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
    /// @param objType The object type name
    /// @param isArray If the object is an array or equivalent
    /// @param objName The object name
    /// @return        If it returns false, the walker won't descend into the children
    virtual bool pre(const std::string& objType, bool isArray, const std::string& objName) = 0;
    
    /// Called after processing a data member
    /// @param objType The object type name
    /// @param isArray If the object is an array or equivalent
    /// @param objName The object name
    virtual void post(const std::string& objType, bool isArray, const std::string& objName) = 0;
    
    /// Called for a basic type with a name. For instance, object attributes.
    /// @param name The attribute name
    /// @param data The attribute raw data
    virtual void leaf(const std::string& name, const Data& data) = 0;
    
    /// Called for a basic type within an array.
    /// @param index The attribute index within the array
    /// @param data  The attribute raw data
    virtual void leaf(size_t index, const Data& data) = 0;
    
    /// Called when an element of unknown type is found.
    /// @param name The attribute name
    /// @param type The unknown type
    virtual void unknown(const std::string& name, const std::string& type) = 0;
};

/// Public interface
class Walker
{
public:
    /// Constructor
    /// @param file The file to be traversed
    Walker(const TFile& file, TypeResolver& typeResolver);
    
    /// Perform the walk
    /// @param visitor The implementation of IVisitor
    void walk(IVisitor& visitor);
    
protected:
    TypeResolver& typeResolver;
    const TFile& file;
};

}}

#endif // __WALKER_HPP
