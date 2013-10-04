#ifndef __ROOT_WALKER_HPP
#define __ROOT_WALKER_HPP

#include <TFile.h>
#include "Data.hpp"

namespace scidb {
namespace root {

/// Visitor interface.
class IVisitor
{
public:
    /// Called before desdencing into a data member
    /// @param objType The object type name
    /// @param isArray If the object is an array or equivalent
    /// @param objName The object name
    /// @param ptr     The pointer passed to Walker::walk method
    /// @return        If it returns false, the walker won't descend into the children
    virtual bool pre(const std::string& objType, bool isArray, const std::string& objName, void* ptr) = 0;
    
    /// Called after processing a data member
    /// @param objType The object type name
    /// @param isArray If the object is an array or equivalent
    /// @param objName The object name
    /// @param ptr     The pointer passed to Walker::walk method
    virtual void post(const std::string& objType, bool isArray, const std::string& objName, void* ptr) = 0;
    
    /// Called for a basic type with a name. For instance, object attributes.
    /// @param name The attribute name
    /// @param data The attribute raw data
    /// @param ptr  The pointer passed to Walker::walk method
    virtual void leaf(const std::string& name, const Data& data, void* ptr) = 0;
    
    /// Called for a basic type within an array.
    /// @param index The attribute index within the array
    /// @param data  The attribute raw data
    /// @param ptr   The pointer passed to Walker::walk method
    virtual void leaf(size_t index, const Data& data, void* ptr) = 0;
};

/// Public interface
class Walker
{
public:
    /// Constructor
    /// @param file The file to be traversed
    Walker(const TFile& file);
    
    /// Perform the walk
    /// @param visitor The implementation of IVisitor
    /// @param ptr     This pointer will be passed to the methods in visitor
    void walk(IVisitor& visitor, void* ptr);
    
protected:
    const TFile& file;
};

}}

#endif // __ROOT_WALKER_HPP
