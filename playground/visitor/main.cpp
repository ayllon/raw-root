#include <iostream>
#include <TFile.h>
#include "walker.hpp"

using namespace scidb::root;


class MyVisitor: public IVisitor
{
protected:
	unsigned tabLevel;
	
public:
	
	MyVisitor(): tabLevel(0) {
	}
	
	// Before children
	void pre(const std::string& objType, const std::string& objName, void* ptr) {
		std::string indent(tabLevel, '\t');
		std::cout << indent << '+' << objType << ' ' << objName << std::endl;
		++tabLevel;
	}
	
	// After children
	void post(const std::string& objType, const std::string& objName, void* ptr) {
		--tabLevel;
	}
	
	// On a leaf
	void leaf(const std::string& name, const Data& data, void* ptr) {
		std::string indent(tabLevel, '\t');
		std::cout << indent << ' '
				  << data.getTypeName() << ' '
				  << name << " = " << data
				  << std::endl;
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
