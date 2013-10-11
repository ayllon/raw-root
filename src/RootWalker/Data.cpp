#include <cassert>
#include <cstdarg>
#include <TString.h>
#include "Data.hpp"

using namespace scidb::root;


struct TypeNamePair {
    DataType     type;
    const char** aliases;
    size_t       nAliases;
    
    TypeNamePair(DataType type, ...): type(type), nAliases(0)
    {
        va_list args;
        const char* a;
        
        va_start(args, type);
        while ((a = va_arg(args, const char*)))
            ++nAliases;
        va_end(args);
        
        aliases = new const char*[nAliases];
        
        size_t i = 0;
        va_start(args, type);
        while ((a = va_arg(args, const char*)))
            aliases[i++] = a;
        va_end(args);
    }
    
    ~TypeNamePair()
    {
        delete [] aliases;
    }
};


const static TypeNamePair typeMaps[] = {
    TypeNamePair(kInt8,   "char", "Char_t", "Bool_t", NULL),
    TypeNamePair(kInt32,  "int", "Int_t", NULL),
    TypeNamePair(kUInt32, "UInt_t", NULL),
    TypeNamePair(kInt64,  "Long64_t", NULL),
    TypeNamePair(kFloat,  "float", "Float_t", NULL),
    TypeNamePair(kDouble, "double", "Double_t", NULL),
    TypeNamePair(kString, "TString", NULL)
};
const static size_t nPairs = sizeof(typeMaps) / sizeof(TypeNamePair);


DataType Data::typeFromStr(const std::string& typeName)
{
    for (size_t i = 0; i < nPairs; ++i) {
        const TypeNamePair& pair = typeMaps[i];     
        for (size_t j = 0; j < pair.nAliases; ++j) {
            if (typeName.compare(pair.aliases[j]) == 0)
                return pair.type;
        }
    }
    return kUnknown;
}


Data::Data(const std::string& typeName, const void* dataPtr):
    typeName(typeName), dataPtr(dataPtr)
{
    type = typeFromStr(typeName);
}


std::string Data::getTypeName() const
{
    return typeName;
}


DataType Data::getType() const
{
    return type;
}


int8_t Data::getInt8() const
{
    assert(type == kInt8);
    return *(static_cast<const int8_t*>(dataPtr));
}


int32_t Data::getInt32() const
{
    assert(type == kInt32);
    return *(static_cast<const int32_t*>(dataPtr));
}


uint32_t Data::getUInt32() const
{
    assert(type == kUInt32);
    return *(static_cast<const uint32_t*>(dataPtr));
}


int64_t Data::getInt64() const
{
    assert(type == kInt64);
    return *(static_cast<const int64_t*>(dataPtr));
}


float Data::getFloat() const
{
    assert(type == kFloat);
    return *(static_cast<const float*>(dataPtr));
}


double Data::getDouble() const
{
    assert(type == kDouble);
    return *(static_cast<const double*>(dataPtr));
}


std::string Data::getString() const
{
    const TString *tStr = static_cast<const TString*>(dataPtr);
    return tStr->Data();
}


std::ostream& scidb::root::operator << (std::ostream& o, const Data& d)
{
    switch (d.getType()) {
        case kInt8:
            o << static_cast<int>(d.getInt8());
            break;
        case kInt32:
            o << d.getInt32();
            break;
        case kUInt32:
            o << d.getUInt32();
            break;
        case kInt64:
            o << d.getInt64();
            break;
        case kFloat:
            o << d.getFloat();
            break;
        case kDouble:
            o << d.getDouble();
            break;
        case kString:
            o << '"' << d.getString() << '"';
            break;
        default:
            o << '?';
    }
    return o;
}
