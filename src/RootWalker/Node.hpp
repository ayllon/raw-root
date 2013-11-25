#ifndef __NODE_HPP
#define __NODE_HPP

#include <memory>
#include <string>
#include <TObject.h>
#include "Data.hpp"


namespace raw {
namespace root {
    
/// Holds a pointer to an object and associated metadata
class Node {
public:
    
    Node():
        type("nullptr"), name(""), pointer(false), address(nullptr), array(false)
    {
    }
    
    Node(const std::string& type, const std::string& name, const void* p):
         type(type), name(name), pointer(false), address(p), array(false)
    {
    }
    
    Node(const std::string& type, bool rp, const std::string& name, const void* p):
         type(type), name(name), pointer(rp), address(p), array(false)
    {
    }

    Node(const std::string& container, const std::string& contained, const std::string& name, const void* p):
        type(container), contained(contained), name(name), pointer(false), address(p), array(true)
    {
    }

    Node(const TObject& obj):
        type(obj.ClassName()), name(obj.GetName()), address(&obj), array(false)
    {
    }
    
    std::string getTypeName() const
    {
        return type;
    }
    
    std::string getName() const
    {
        return name;
    }
    
    const void* getAddress() const
    {
        return address;
    }
    
    bool isPointer() const
    {
        return pointer;
    }
    
    bool isBasic() const
    {
        return Data::typeFromStr(type) != kUnknown;
    }

    bool isArray() const
    {
        return array;
    }
    
    void setArray()
    {
        array = true;
    }

    std::string getContainedType() const
    {
        return contained;
    }
    
protected:
    std::string type;
    std::string name;
    bool pointer;
    
    const void *address;
    
    bool array;
    std::string contained;

    std::shared_ptr<Data> dPtrHolder;

private:
    // Forbid copies
    Node(const Node&);
    const Node& operator = (const Node&);
};

}}

#endif // __OBJREF_HPP
