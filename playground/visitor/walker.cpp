#include <TClass.h>
#include <TDataMember.h>
#include <TKey.h>
#include <TMemberInspector.h>
#include "walker.hpp"

using namespace scidb::root;



class RootInspector: public TMemberInspector {
public:
	RootInspector(IVisitor& visitor, void* ptr): visitor(visitor), ptr(ptr) {
	}
	
	void Inspect(TClass* klass, const char* parent, const char* name, const void* addr)
	{
		// We don't want to recurse into members... yet
		if (parent && parent[0])
			return;
		
		TDataMember* member = klass->GetDataMember(name);
		
		// Basic types can not be traversed, so they are leafs
		if (member->IsBasic()) {
			this->visitor.leaf(name, Data(member->GetTypeName(), addr), this->ptr);
		}
		else {
			this->visitor.pre(member->GetTypeName(), name, this->ptr);
			this->visitor.post(member->GetTypeName(), name, this->ptr);
		}
	}
	
protected:
	IVisitor& visitor;
	void* ptr;
};



Walker::Walker(const TFile& file): file(file)
{
}



void Walker::walk(IVisitor& visitor, void* ptr)
{
	RootInspector inspector(visitor, ptr);
	
	Int_t  nkeys = this->file.GetNkeys();
	TList* keys  = this->file.GetListOfKeys();
	
	for (Int_t i = 0; i < nkeys; ++i) {
		TKey* key = static_cast<TKey*>(keys->At(i));
		TObject* obj = key->ReadObj();
		if (obj) {
			visitor.pre(key->GetClassName(), obj->GetName(), ptr);
			obj->ShowMembers(inspector);
			visitor.post(key->GetClassName(), obj->GetName(), ptr);
		}
	}
}
