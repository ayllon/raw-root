#include <iostream>
#include <TFile.h>
#include "TypeResolver.hpp"
#include "Walker.hpp"

#ifndef BUILD_PATH
#  define BUILD_PATH ""
#endif

using namespace raw::root;


class MyVisitor: public IVisitor
{
protected:
    unsigned tabLevel;
    
public:
    
    MyVisitor(): tabLevel(0)
    {
    }
    
    // Before children
    bool pre(const std::shared_ptr<Node> node)
    {
        std::string indent(tabLevel, '\t');
        std::cout << indent << node->getName().substr(0, 30) << ": " << node->getType();
        ++tabLevel;
        if (node->getType() == Node::kDictionary)
            std::cout << " {";
        else if (node->getType() == Node::kUnknown)
        	std::cout << " (" << node->getTypeName() << ")";
        std::cout << std::endl;        
        return true;
    }
    
    // After children
    void post(const std::shared_ptr<Node> node)
    {
        --tabLevel;
        std::string indent(tabLevel, '\t');
        if (node->getType() == Node::kDictionary)
            std::cout << indent << " }" << std::endl;
    }
    
    // Unknown elements
    void unknown(const std::shared_ptr<Node> node)
    {
        std::string indent(tabLevel, '\t');
        std::cout << indent << '?' << node->getTypeName() << ' ' << node->getName() << std::endl;
    }
};



static std::string getHandlerLibraryPath(void)
{
    const char* envPath = getenv("HANDLER_LIBRARY_PATH");
    if (envPath)
        return envPath;
    return std::string(BUILD_PATH);
}



int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Expecting a file name as a parameter" << std::endl;
        return -1;
    }
    
    TFile file(argv[1]);
    if (file.IsZombie()) {
        std::cerr << "Could not load!" << std::endl;
        return -1;
    }
    std::shared_ptr<TypeResolver> typeResolver(new TypeResolver(getHandlerLibraryPath()));
    Walker walker(typeResolver);
    
    std::shared_ptr<Node> rootNode(new Node(file));
    if (argc > 2)
        rootNode = walker.getChildNode(rootNode, argv[2]);

    if (rootNode) {
        std::shared_ptr<MyVisitor> visitor(new MyVisitor());
        walker.walk(rootNode, visitor);
    }
    else {
        std::cerr << "Could not find the given node" << std::endl;
    }
    
    return 0;
}
