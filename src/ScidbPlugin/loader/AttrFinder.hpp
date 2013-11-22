#ifndef __ATTRFINDER_HPP
#define __ATTRFINDER_HPP

#include <Walker.hpp>

class AttrFinder: public scidb::root::IVisitor
{
private:
    boost::shared_ptr<std::vector<scidb::root::Node> > attributes;
    log4cxx::LoggerPtr logger;
    size_t depth;
    
public:
    
    AttrFinder(): attributes(new std::vector<scidb::root::Node>),
        depth(0), logger(log4cxx::Logger::getLogger("scidb.common.thread"))
    {
    }
    
    
    bool pre(const scidb::root::Node& node)
    {
        ++depth;
        
        if (depth == 1) {
            LOG4CXX_DEBUG(logger, "Searching attributes inside " << node.getName());
            return true;
        }
        
        LOG4CXX_DEBUG(logger, "Found attribute " << node.getTypeName() << " " << node.getName());
        
        if (node.isArray()) {
            scidb::root::DataType containedType = scidb::root::Data::typeFromStr(node.getContainedType());
            
            switch (containedType)
            {
                case scidb::root::kDouble:
                case scidb::root::kFloat:
                case scidb::root::kInt32:
                case scidb::root::kInt64:
                case scidb::root::kInt8:
                case scidb::root::kUInt32:
                case scidb::root::kUInt64:
                case scidb::root::kUInt8:
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
    
    
    void post(const scidb::root::Node& node)
    {
        --depth;
    }
    
    
    void unknown(const scidb::root::Node& /*node*/)
    {
        // Pass
    }
    

    void leaf(const std::string& /*name*/, const scidb::root::Data& /*data*/)
    {
        // Pass
    }
    

    void leaf(size_t /*index*/, const scidb::root::Data& /*data*/)
    {
        // Pass
    }
    
    boost::shared_ptr<std::vector<scidb::root::Node> > getAttributes()
    {
        return attributes;
    }
};

#endif // __ATTRFINDER_HPP
