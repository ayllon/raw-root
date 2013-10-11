#ifndef __TYPEHANDLER_HPP
#define __TYPEHANDLER_HPP

#include "Walker.hpp"

namespace scidb {
namespace root {

/// Interface for type handlers
class ITypeHandler
{
public:
    ITypeHandler() {};
    
    /// Destructor
    virtual ~ITypeHandler() {};
    
    /// Return true if this handler recognize the type
    virtual bool recognize(const std::string& typeName) = 0;
    
    /// Delegate processing of the type
    virtual void inspect(const std::string& typeName, bool isPointer, 
                         const std::string& name, const void* addr,
                         IVisitor& visitor) = 0;
                         
private:
    // Forbid copy
    ITypeHandler(const ITypeHandler&);
    const ITypeHandler& operator = (const ITypeHandler&) const;
};

}}

#endif //__TYPEHANDLER_HPP
