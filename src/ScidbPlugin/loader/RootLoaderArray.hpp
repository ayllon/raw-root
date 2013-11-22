#ifndef __ROOTLOADERARRAY_HPP
#define __ROOTLOADERARRAY_HPP

#include <array/Array.h>
#include <array/MemArray.h>
#include <array/TupleArray.h>

namespace scidb {

class RootConstArrayIterator: public ConstArrayIterator
{
private:
    boost::shared_ptr<std::vector<root::Node> > attributes;
    scidb::AttributeID attId;
    Coordinates coordinates;
    MemChunk chunk;
    
public:
    RootConstArrayIterator(boost::shared_ptr<std::vector<root::Node> > attributes,
        scidb::AttributeID attId):
        attributes(attributes), attId(attId)
    {
        coordinates.push_back(0);
    }
    
    
    bool end()
    {
        return coordinates[0] > 0;
    }
    
    
    void operator++()
    {
        ++coordinates[0];
    }
    
    
    const Coordinates& getPosition()
    {
        return coordinates;
    }
    
    
    bool setPosition(Coordinates const& pos)
    {
        if (pos[0] < 2) {
            coordinates = pos;
            return true;
        }
        else {
            return false;
        }
    }
    
    
    const ConstChunk& getChunk()
    {
        return chunk;
    }
};

  
class RootLoaderArray: public Array
{
private:
    ArrayDesc schema;
    boost::shared_ptr<Query> query;
    boost::shared_ptr<std::vector<root::Node> > attributes;
    
    friend class RootConstArrayIterator;
    
public:
    RootLoaderArray(ArrayDesc schema, boost::shared_ptr<Query> query, boost::shared_ptr<std::vector<root::Node> > attributes):
        schema(schema), query(query), attributes(attributes)
    {
    }
    
    
    virtual ~RootLoaderArray()
    {
        // Pass
    }
    
    
    ArrayDesc const& getArrayDesc() const
    {
        return schema;
    }
    
    
    boost::shared_ptr<scidb::ConstArrayIterator> getConstIterator(scidb::AttributeID attId) const
    {
        return boost::shared_ptr<ConstArrayIterator>(new RootConstArrayIterator(this->attributes, attId));
    }
};
    
}

#endif // __ROOTLOADERARRAY_HPP
