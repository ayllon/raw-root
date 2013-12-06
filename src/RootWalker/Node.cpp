#include "Node.hpp"

using namespace raw::root;


struct TypeNamePair {
    Node::Type   type;
    const char** aliases;
    size_t       nAliases;

    TypeNamePair(Node::Type type, ...): type(type), nAliases(0)
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
    TypeNamePair(Node::kInt8,   "char", "Char_t", "Bool_t", NULL),
    TypeNamePair(Node::kInt32,  "int", "Int_t", NULL),
    TypeNamePair(Node::kUInt32, "UInt_t", NULL),
    TypeNamePair(Node::kInt64,  "Long64_t", NULL),
    TypeNamePair(Node::kFloat,  "float", "Float_t", NULL),
    TypeNamePair(Node::kDouble, "double", "Double_t", NULL),
    TypeNamePair(Node::kString, "TString", NULL)
};
const static size_t nPairs = sizeof(typeMaps) / sizeof(TypeNamePair);


Node::Type Node::TypeFromStr(const std::string& typeName)
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


Node::Node(Type type, const std::string& name, const void* addr):
    type (type), name(name), address(addr)
{
}


Node::Node(const std::string& type, const std::string& name, const void* addr):
    typeName(type), type (TypeFromStr(type)), name(name), address(addr)
{
}


Node::Node(const std::string& typeName, Type type, const std::string& name, const void* addr):
    typeName(typeName), type (type), name(name), address(addr)
{
}


Node::Node(const TObject& obj):
    typeName(obj.ClassName()), type(kDictionary), name(obj.GetName()), address(&obj)
{
}


std::string Node::getTypeName() const
{
    return typeName;
}


Node::Type Node::getType() const
{
    return type;
}


std::string Node::getName() const
{
    return name;
}


const void* Node::getAddress() const
{
    return address;
}


void Node::setType(Type t)
{
    type = t;
}


bool Node::isBasic() const
{
	return type > Node::kCollection && type <= Node::kString;
}


std::ostream& raw::root::operator << (std::ostream& os, Node::Type type)
{
   switch (type) {
       case Node::kDictionary:
           os << "dictionary";
           break;
       case Node::kCollection:
           os << "collection";
           break;
       case Node::kInt8: case Node::kInt32: case Node::kInt64:
           os << "integer";
           break;
       case Node::kUInt8: case Node::kUInt32: case Node::kUInt64:
           os << "unsigned";
           break;
       case Node::kFloat:
           os << "float";
           break;
       case Node::kDouble:
           os << "double";
           break;
       case Node::kString:
           os << "string";
           break;
       case Node::kUnknown:
           os << "unknown";
           break;
       default:
           os << (int)(type);
           break;
   }
   return os;
}
