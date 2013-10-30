#ifndef __DATA_HPP_
#define __DATA_HPP_

#include <ostream>
#include <stdint.h>
#include <string>

namespace scidb {
namespace root {

/// Enumerate for different _basic_ data types
enum DataType {
    kUnknown = 0,
    kInt8,  kInt32,  kInt64,
    kUInt8, kUInt32, kUInt64,
    kFloat, kDouble,
    kString
};


/// Holder and caster for types identified by a string
class Data
{
public:
    /// Return the corresponding DataType for typeName
    static DataType typeFromStr(const std::string& typeName);
    
    /// Default constructor
    Data();
    
    /// Constructor
    Data(const std::string& typeName, const void* dataPtr);
    
    /// Get the type as a string
    std::string getTypeName() const;
    
    /// Get the type as an enumerate
    DataType getType() const;
    
    /// Get raw pointer
    const void* getRawPointer(void) const;
    
    // Get the cast value of the raw data pointed by dataPtr
    
    int8_t      getInt8() const;
    int32_t     getInt32() const;
    uint32_t    getUInt32() const;
    int64_t     getInt64() const;
    float       getFloat() const;
    double      getDouble() const;
    std::string getString() const;
    
protected:
    std::string typeName;
    const void* dataPtr;
    DataType    type;
};

/// Convenience method to dump a Data object into a stream
std::ostream& operator << (std::ostream& o, const Data& d);

}}

#endif // __DATA_HPP_
