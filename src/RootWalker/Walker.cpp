#include "TypeResolver.hpp"
#include "Walker.hpp"

using namespace scidb::root;



Walker::Walker(const TObject& obj, TypeResolver& typeResolver):
    obj(obj), typeResolver(typeResolver)
{
}


void Walker::walk(IVisitor& visitor)
{   
    ITypeHandler* handler = typeResolver.getHandlerForType(obj.ClassName());
    if (handler)
        handler->inspect(obj.ClassName(), false, obj.GetName(), &obj, visitor);
    else
        visitor.unknown(obj.GetName(), obj.ClassName());
}
