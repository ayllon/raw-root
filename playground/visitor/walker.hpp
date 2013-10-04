#ifndef __ROOT_WALKER_HPP
#define __ROOT_WALKER_HPP

#include <TFile.h>
#include "data.hpp"

namespace scidb {
namespace root {

class IVisitor
{
public:
    virtual void pre(const std::string& objType, bool isArray, const std::string& objName, void* ptr) = 0;
    virtual void post(const std::string& objType, bool isArray, const std::string& objName, void* ptr) = 0;
    virtual void leaf(const std::string& name, const Data& data, void* ptr) = 0;
    virtual void leaf(size_t index, const Data& data, void* ptr) = 0;
};

class Walker
{
public:
    Walker(const TFile& file);
    
    void walk(IVisitor& visitor, void* ptr);
    
protected:
    const TFile& file;
};

}}

#endif // __ROOT_WALKER_HPP
