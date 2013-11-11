#ifndef __TYPERESOLVER_HPP
#define __TYPERESOLVER_HPP

#include <log4cxx/logger.h>
#include "TypeHandler.hpp"

namespace scidb {
namespace root {

// Private implementation
class TypeResolverImpl;

class TypeResolver
{
public:
    TypeResolver(const std::string& handlerLocation,
                 log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger("default"));
    ~TypeResolver();
    
    /// Register a new handler.
    /// TypeResolver will take care of freeing this pointer!
    void registerHandler(ITypeHandler* handler);
    
    /// Return the type handler for the given type name
    ITypeHandler* getHandlerForType(const std::string& typeName);
    
private:
    TypeResolverImpl* implPtr;
    
    // Forbid copies
    TypeResolver(const TypeResolver&);
    const TypeResolver operator = (const TypeResolver&) const;
};

}}

#endif // __TYPERESOLVER_HPP
