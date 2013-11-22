#ifndef __ATTRFINDER_HPP
#define __ATTRFINDER_HPP

#include <Walker.hpp>

class AttrFinder: public raw::root::IVisitor
{
private:
    boost::shared_ptr<std::vector<raw::root::Node> > attributes;
    log4cxx::LoggerPtr logger;
    size_t depth;
    
public:
    
    AttrFinder(): attributes(new std::vector<raw::root::Node>),
        depth(0), logger(log4cxx::Logger::getLogger("scidb.common.thread"))
    {
    }
    
    
    bool pre(const raw::root::Node& node)
    {
        ++depth;
        
        if (depth == 1) {
            LOG4CXX_DEBUG(logger, "Searching attributes inside " << node.getName());
            return true;
        }
        
        LOG4CXX_DEBUG(logger, "Found attribute " << node.getTypeName() << " " << node.getName());
        
        if (node.isArray()) {
            raw::root::DataType containedType = raw::root::Data::typeFromStr(node.getContainedType());
            
            switch (containedType)
            {
                case raw::root::kDouble:
                case raw::root::kFloat:
                case raw::root::kInt32:
                case raw::root::kInt64:
                case raw::root::kInt8:
                case raw::root::kUInt32:
                case raw::root::kUInt64:
                case raw::root::kUInt8:
                    LOG4CXX_DEBUG(logger, "Array of a known type found: " << node.getName());
                    attributes->push_back(node);
                    break;
                default:
                    // Pass
                    break;
            }
        }
        
        return false;
    }
    
    
    void post(const raw::root::Node& node)
    {
        --depth;
    }
    
    
    void unknown(const raw::root::Node& /*node*/)
    {
        // Pass
    }
    

    void leaf(const std::string& /*name*/, const raw::root::Data& /*data*/)
    {
        // Pass
    }
    

    void leaf(size_t /*index*/, const raw::root::Data& /*data*/)
    {
        // Pass
    }
    
    boost::shared_ptr<std::vector<raw::root::Node> > getAttributes()
    {
        return attributes;
    }
};

#endif // __ATTRFINDER_HPP
