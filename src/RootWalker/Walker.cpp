#include <sstream>
#include "TypeResolver.hpp"
#include "Walker.hpp"

using namespace raw::root;



Walker::Walker(std::shared_ptr<TypeResolver> typeResolver):
    typeResolver(typeResolver)
{
}


void Walker::walk(const std::shared_ptr<Node> node, IVisitor* visitor)
{   
    ITypeHandler* handler = typeResolver->getHandlerForType(node->getTypeName());
    if (handler)
        handler->inspect(node, visitor);
    else
        visitor->unknown(node);
}


class PickerVisitor: public IVisitor
{
private:
    std::shared_ptr<Node> node;
    
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
    
    std::shared_ptr<Node> getNode(void)
    {
        return node;
    }
    
    bool pre(const std::shared_ptr<Node> visitedNode)
    {
        bool recurse = false;
        if (!node && depth <= maxDepth) {
            std::string lookingFor = pathComponents[depth];
            
            if (depth == maxDepth && lookingFor == visitedNode->getName()) {
                node = visitedNode;
                recurse = false;
            }
            else {
                recurse = (visitedNode->getName() == lookingFor || lookingFor == "/") &&
                          !visitedNode->isPointer();
            }
        }
        ++depth;
        return recurse;
    }
    
    void post(const std::shared_ptr<Node> visitedNode)
    {
        --depth;
    }
    
    void unknown(const std::shared_ptr<Node> visitedNode)
    {
        if (!node && depth <= maxDepth) {
            std::string lookingFor = pathComponents[depth];
            
            if (depth == maxDepth && lookingFor == visitedNode->getName())
                node = visitedNode;
        }
    }
    
    void leaf(const std::string& name, const std::shared_ptr<Data> data)
    {
        // Leaf not supported
    }
    
    void leaf(size_t index, const std::shared_ptr<Data> data)
    {
        // Leaf not supported
    }
};


std::shared_ptr<Node> Walker::getChildNode(const std::shared_ptr<Node> node, const std::string& path)
{
    ITypeHandler* handler = typeResolver->getHandlerForType(node->getTypeName());
    if (!handler)
        return std::shared_ptr<Node>();

    PickerVisitor picker(path);
    handler->inspect(node, &picker);
    return picker.getNode();
}
