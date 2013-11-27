#ifndef __NODE_HPP
#define __NODE_HPP

#include <iostream>
#include <memory>
#include <string>
#include <TObject.h>


namespace raw {
namespace root {
    
/// Holds a pointer to an object and associated metadata
class Node {
public:
    enum Type {
        kUnknown = 0,
        kDictionary,
        kCollection,
        kInt8,  kInt32,  kInt64,
        kUInt8, kUInt32, kUInt64,
        kFloat, kDouble,
        kString
    };

    static Type TypeFromStr(const std::string& str);

protected:
    std::string typeName;
    Type        type;
    std::string name;
    const void* address;

public:
    Node(Type type, const std::string& name = "", const void* addr = NULL);

    Node(const std::string& type, const std::string& name = "", const void* addr = NULL);

    Node(const std::string& typeName, Type type, const std::string& name = "", const void* addr = NULL);

    Node(const TObject& obj);

    std::string getTypeName() const;

    Type getType() const;

    std::string getName() const;

    const void* getAddress() const;

    void setType(Type type);
};


std::ostream& operator << (std::ostream& os, Node::Type type);

}}

#endif // __OBJREF_HPP
