#include <TKey.h>
#include "TypeResolver.hpp"
#include "Walker.hpp"

using namespace scidb::root;



Walker::Walker(const TFile& file, TypeResolver& typeResolver):
    file(file), typeResolver(typeResolver)
{
}


void Walker::walk(IVisitor& visitor)
{   
    Int_t  nkeys = this->file.GetNkeys();
    TList* keys  = this->file.GetListOfKeys();
    
    for (Int_t i = 0; i < nkeys; ++i) {
        TKey* key = static_cast<TKey*>(keys->At(i));
        TObject* obj = key->ReadObj();
        ITypeHandler* handler = typeResolver.getHandlerForType(key->GetClassName());
        if (handler)
            handler->inspect(key->GetClassName(), false, obj->GetName(), obj, visitor);
        else
            visitor.unknown(obj->GetName(), key->GetClassName());
    }
}
