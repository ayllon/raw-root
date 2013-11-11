#include <sstream>
#include "TypeResolver.hpp"
#include "Walker.hpp"

using namespace scidb::root;



Walker::Walker(TypeResolver& typeResolver, log4cxx::LoggerPtr logger):
    typeResolver(typeResolver), logger(logger)
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
    log4cxx::LoggerPtr logger;
    
public:
    PickerVisitor(const std::string& path, log4cxx::LoggerPtr logger):
        depth(0), logger(logger)
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
        LOG4CXX_DEBUG(logger, "Searching under " << visitedNode.getName());
        
        bool recurse = false;
        if (!node && depth <= maxDepth) {
            std::string lookingFor = pathComponents[depth];
            
            if (depth == maxDepth && lookingFor == visitedNode.getName()) {
                LOG4CXX_DEBUG(logger, "Found node " << lookingFor);
                node = visitedNode;
                recurse = false;
            }
            else {
                recurse = (visitedNode.getName() == lookingFor || lookingFor == "/") && !visitedNode.isPointer();
            }
        }
        ++depth;
        return recurse;
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
    
    LOG4CXX_DEBUG(logger, "Using " << handler->getHandlerId() << " for the root node, which is a " << node.getTypeName());
    
    PickerVisitor picker(path, logger);
    handler->inspect(node, picker);
    return picker.getNode();
}
