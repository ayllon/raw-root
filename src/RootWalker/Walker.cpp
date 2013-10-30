#include <sstream>
#include "TypeResolver.hpp"
#include "Walker.hpp"

using namespace scidb::root;



Walker::Walker(TypeResolver& typeResolver):
    typeResolver(typeResolver)
{
}


void Walker::walk(const Node& node, IVisitor& visitor)
{   
    ITypeHandler* handler = typeResolver.getHandlerForType(node.getTypeName());
    if (handler)
        handler->inspect(node, visitor);
    else
        visitor.unknown(node);
}


class PickerVisitor: public IVisitor
{
private:
    Node node;
    
    std::vector<std::string> pathComponents;
    size_t depth, maxDepth;
    
public:
    PickerVisitor(const std::string& path): depth(0)
    {
        std::istringstream helper(path);
        std::string c;
        pathComponents.push_back("/");
        while (std::getline(helper, c, '.'))
            pathComponents.push_back(c);

        maxDepth = pathComponents.size() - 1;
    }
    
    Node getNode(void)
    {
        return node;
    }
    
    bool pre(const Node& visitedNode)
    {
        bool recurse = false;
        if (!node && depth <= maxDepth) {
            std::string lookingFor = pathComponents[depth];
            
            if (depth == maxDepth && lookingFor == visitedNode.getName()) {
                node = visitedNode;
                recurse = false;
            }
            else {
                recurse = (visitedNode.getName() == lookingFor);
            }
        }
        ++depth;
    }
    
    void post(const Node& visitedNode)
    {
        --depth;
    }
    
    void unknown(const Node& visitedNode)
    {
        if (!node && depth <= maxDepth) {
            std::string lookingFor = pathComponents[depth];
            
            if (depth == maxDepth && lookingFor == visitedNode.getName())
                node = visitedNode;
        }
    }
    
    void leaf(const std::string& name, const Data& data)
    {
        if (!node && depth <= maxDepth) {
            std::string lookingFor = pathComponents[depth];
            
            if (depth == maxDepth && lookingFor == name)
                node = Node(name, data);
        }
    }
    
    void leaf(size_t index, const Data& data)
    {
        if (!node && depth <= maxDepth) {
            std::string lookingFor = pathComponents[depth];
            
            std::ostringstream indexStr;
            indexStr << index;
            if (depth == maxDepth && lookingFor == indexStr.str())
                node = Node(indexStr.str(), data);
        }
    }
};


Node Walker::getChildNode(const Node& node, const std::string& path)
{
    ITypeHandler* handler = typeResolver.getHandlerForType(node.getTypeName());
    if (!handler)
        return Node();
    
    PickerVisitor picker(path);
    handler->inspect(node, picker);
    return picker.getNode();
}
