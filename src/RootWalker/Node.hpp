#ifndef __NODE_HPP
#define __NODE_HPP

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
    
    Node(const std::string& t, const std::string& n, const void* p):
         type(t), name(n), pointer(false), address(p), array(false)
    {
    }
    
    Node(const std::string& t, bool rp, const std::string& n, const void* p):
         type(t), name(n), pointer(rp), address(p), array(false)
    {
    }
    
    Node(const std::string& n, const Data& d):
        type(d.getTypeName()), name(n), address(d.getRawPointer()), array(false)
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
    
    Data getData() const
    {
        return Data(type, address);
    }
    
    bool isArray() const
    {
        return array;
    }
    
    std::string getContainedType() const
    {
        return contained;
    }
    
    operator bool () const
    {
        return address != nullptr;
    }
    
protected:
    std::string type;
    std::string name;
    bool pointer;
    
    const void *address;
    
    bool array;
    std::string contained;
};


class ArrayNode: public Node
{
public:
    ArrayNode(const Node& node, const std::string& ct = std::string("*")):
        Node(node)
    {
        array = true;
        contained = ct;
    }
    
    ArrayNode(const std::string& t, const std::string& ct, const std::string& n, const void* p):
        Node(t, n, p)
    {
        array = true;
        contained = ct;
    }
};
    
}}

#endif // __OBJREF_HPP
