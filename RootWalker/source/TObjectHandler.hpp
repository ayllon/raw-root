#ifndef __TOBJECTHANDLER_HPP
#define __TOBJECTHANDLER_HPP

#include <TMemberInspector.h>
#include "TypeHandler.hpp"

namespace raw {
namespace root {
  
/// Built-in generic TObject handler. This is intended to be a fallback for types non
/// convertible directly to arrays or basic types.
/// It is registered by the "core" part directly last, so any more detailed
/// implementation is checked.
class TObjectHandler: public TMemberInspector, public ITypeHandler {
public:
    
    TObjectHandler(TypeResolver* resolver);
    
    std::string getHandlerId();
    
    bool recognize(const std::string& typeName);    
    
    void inspect(const std::shared_ptr<Node> node, std::shared_ptr<IVisitor> visitor);
    
    /// Overloaded method
    /// @param klass  The class of the object being inspected
    /// @param parent The parent of the current attribute
    /// @param name   The name of the current attribute
    /// @param addr   A pointer to the value
    void Inspect(TClass* klass, const char* parent, const char* name, const void* addr);

protected:
    TypeResolver* resolver;
    std::shared_ptr<IVisitor> visitor;
};

}}

#endif //__TOBJECTHANDLER_HPP
