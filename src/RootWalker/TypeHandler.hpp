#ifndef __TYPEHANDLER_HPP
#define __TYPEHANDLER_HPP

#include "Walker.hpp"

namespace raw {
namespace root {

/// Interface for type handlers
class ITypeHandler
{
public:
    ITypeHandler() {};
    
    /// Destructor
    virtual ~ITypeHandler() {};
    
    /// Handler ID
    virtual std::string getHandlerId() = 0;
    
    /// Return true if this handler recognize the type
    virtual bool recognize(const std::string& typeName) = 0;
    
    /// Delegate processing of the type
    virtual void inspect(std::shared_ptr<Node> node,
                         std::shared_ptr<IVisitor> visitor) = 0;
                         
private:
    // Forbid copy
    ITypeHandler(const ITypeHandler&);
    const ITypeHandler& operator = (const ITypeHandler&) const;
};

}}

#endif //__TYPEHANDLER_HPP
