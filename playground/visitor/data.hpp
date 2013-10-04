#ifndef __DATA_HPP_
#define __DATA_HPP_

#include <ostream>
#include <stdint.h>
#include <string>

namespace scidb {
namespace root {

enum DataType {
	kUnknown = 0,
	kInt8,  kInt32,  kInt64,
	kUInt8, kUInt32, kUInt64,
	kFloat, kDouble,
	kString
};


class Data
{
public:
	static DataType typeFromStr(const std::string& typeName);
	
	Data(const std::string& typeName, const void* dataPtr);
	std::string getTypeName() const;
	DataType getType() const;
	
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

std::ostream& operator << (std::ostream& o, const Data& d);

}}

#endif // __DATA_HPP_
