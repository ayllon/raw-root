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
        std::cout << indent << '+' << objType << ' ' << objName;
        ++tabLevel;
        if (isArray)
            std::cout << " [";
        else
            std::cout << " {";
        std::cout << std::endl;        
        return true;
    }
    
    // After children
    void post(const std::string& objType, bool isArray, const std::string& objName, void* ptr)
    {
        --tabLevel;
        std::string indent(tabLevel, '\t');
        if (isArray)
            std::cout << std::endl << indent << " ]" << std::endl;
        else
            std::cout << indent << " }" << std::endl;
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
        if (index == 0) {
            std::string indent(tabLevel, '\t');
            std::cout << indent;
        }
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
