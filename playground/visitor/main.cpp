#include <iostream>
#include <TFile.h>
#include "Walker.hpp"

using namespace scidb::root;


class MyVisitor: public IVisitor
{
protected:
    unsigned tabLevel;
    
public:
    
    MyVisitor(): tabLevel(0)
    {
    }
    
    // Before children
    bool pre(const std::string& objType, bool isArray, const std::string& objName, void* ptr)
    {
        std::string indent(tabLevel, '\t');
        std::cout << indent << '+' << objType << ' ' << objName << std::endl;
        ++tabLevel;
        if (isArray)
            std::cout << indent << "\t[";
        
        return true;
    }
    
    // After children
    void post(const std::string& objType, bool isArray, const std::string& objName, void* ptr)
    {
        --tabLevel;
        if (isArray)
            std::cout << "]" << std::endl;
    }
    
    // On a leaf
    void leaf(const std::string& name, const Data& data, void* ptr)
    {
        std::string indent(tabLevel, '\t');
        std::cout << indent << ' '
                  << data.getTypeName() << ' '
                  << name << " = " << data
                  << std::endl;
    }
    
    // Array elements
    void leaf(size_t index, const Data& data, void* ptr)
    {
        std::cout << data << ',';
    }
};

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Expecting a file name as a parameter" << std::endl;
        return -1;
    }
    
    TFile file(argv[1]);
    
    MyVisitor visitor;
    Walker walker(file);
    walker.walk(visitor, nullptr);
    
    return 0;
}
