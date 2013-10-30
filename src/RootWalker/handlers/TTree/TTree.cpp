#include <TBasket.h>
#include <TClass.h>
#include <TLeaf.h>
#include <TTree.h>
#include <boost/concept_check.hpp>
#include "../../TypeHandler.hpp"
#include "../../TypeResolver.hpp"

using namespace scidb::root;


class TTreeHandler: public ITypeHandler
{
public:
    
    bool recognize(const std::string& typeName)
    {
        TClass* klass = TClass::GetClass(typeName.c_str());
        return klass && klass->InheritsFrom("TTree");
    }
    
    
    void inspect(const std::string& typeName, bool isPointer, 
                 const std::string& name, const void* addr,
                 IVisitor& visitor)
    {
        if (visitor.pre(typeName, false, name, addr) && !isPointer) {
            TTree* tree = (TTree*)(addr);
            TObjArray* branches = tree->GetListOfBranches();
            this->iterateBranches(branches, visitor);
        }
        visitor.post(typeName, false, name, addr);
    }
    
    
    void iterateBranches(TObjArray* branches, IVisitor& visitor)
    {
        if (!branches)
            return;

        TIterator* iterator = branches->MakeIterator();
        TObject* obj;
        while ((obj = iterator->Next())) {
            TBranch* branch = static_cast<TBranch*>(obj);
            TObjArray* subBranches = branch->GetListOfBranches();
            
            if (subBranches->GetEntries() == 0) {
                this->iterateEntries(branch, visitor);
            }
            else {
                if (visitor.pre("TBranch", false, branch->GetName(), branches))
                    this->iterateBranches(subBranches, visitor);
                visitor.post("TBranch", false, branch->GetName(), branches);
            }
        }
    }
    
    
    void iterateEntries(TBranch* branch, IVisitor& visitor)
    {
        if (!branch)
            return;
        
        if (visitor.pre("Double_t", true, branch->GetName(), branch)) {
            Long64_t count = branch->GetEntries();
            for (Long64_t i = 0; i < count; ++i) {
                branch->GetEntry(i, 1);
                TObjArray* leaves = branch->GetListOfLeaves();
                this->iterateLeaves(leaves, i, visitor);
            }
        }
        visitor.post("Double_t", true, branch->GetName(), branch);
    }
    
    
    void iterateLeaves(TObjArray* leaves, Long64_t i, IVisitor& visitor)
    {
        if (!leaves)
            return;
        
        TIterator* iterator = leaves->MakeIterator();
        TObject* obj;
        while ((obj = iterator->Next())) {
            TLeaf* leaf = static_cast<TLeaf*>(obj);
            std::string leafType = leaf->GetTypeName();
            
            Double_t value;
            Int_t len = leaf->GetLen();
            if (len > 1) {
                if (visitor.pre(leafType, true, leaf->GetName(), leaf)) {
                    for (Int_t j = 0; j < len; ++j) {
                        value = leaf->GetValue(j);
                        visitor.leaf(j, Data("Double_t", &value));
                    }
                }
                visitor.post(leafType, true, leaf->GetName(), leaf);
            }
            else {
                value = leaf->GetValue();
                visitor.leaf(i, Data("Double_t", &value));
            }
        }
    }
};


/// This method must register the type handlers
extern "C" void registerTypes(TypeResolver* resolver)
{
    resolver->registerHandler(new TTreeHandler());
}
