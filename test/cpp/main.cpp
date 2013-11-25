#include <iostream>
#include <TFile.h>
#include "RootWalker/TypeResolver.hpp"
#include "RootWalker/Walker.hpp"

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
        std::cout << indent << '+' << node->getTypeName() << ' ' << node->getName().substr(0, 30);
        ++tabLevel;
        if (node->isArray())
            std::cout << " [";
        else
            std::cout << " {";
        std::cout << std::endl;        
        return true;
    }
    
    // After children
    void post(const std::shared_ptr<Node> node)
    {
        --tabLevel;
        std::string indent(tabLevel, '\t');
        if (node->isArray())
            std::cout << std::endl << indent << " ]" << std::endl;
        else
            std::cout << indent << " }" << std::endl;
    }
    
    // On a leaf
    void leaf(const std::string& name, const std::shared_ptr<Data> data)
    {
        std::string indent(tabLevel, '\t');
        std::cout << indent << ' '
                  << data->getTypeName() << ' '
                  << name.substr(0, 30) << " = " << data
                  << std::endl;
    }
    
    // Array elements
    void leaf(size_t index, const std::shared_ptr<Data> data)
    {
        if (index == 0) {
            std::string indent(tabLevel, '\t');
            std::cout << indent;
        }
        std::cout << *data << ',';
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
        MyVisitor visitor;
        walker.walk(rootNode, &visitor);
    }
    
    return 0;
}
