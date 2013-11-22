#include <TBasket.h>
#include <TClass.h>
#include <TLeaf.h>
#include <TTree.h>
#include <boost/concept_check.hpp>
#include "../../TypeHandler.hpp"
#include "../../TypeResolver.hpp"

using namespace raw::root;


class TTreeHandler: public ITypeHandler
{
public:
    
    std::string getHandlerId()
    {
        return "TTreeHandler";
    }
    
    
    bool isTree(const std::string& typeName)
    {
        TClass* klass = TClass::GetClass(typeName.c_str());
        return klass && klass->InheritsFrom("TTree");
    }
    
    
    bool isBranch(const std::string& typeName)
    {
        TClass* klass = TClass::GetClass(typeName.c_str());
        return klass && klass->InheritsFrom("TBranch");
    }
    
    
    bool isDoubleBranch(const std::string& typeName)
    {
        return typeName == "TBranch<Double_t>";
    }
    
    
    bool recognize(const std::string& typeName)
    {
        return isTree(typeName) || isBranch(typeName) || isDoubleBranch(typeName);
    }
    
    
    void inspect(const Node& node, IVisitor& visitor)
    {
        if (isTree(node.getTypeName())) {
            if (visitor.pre(node) && !node.isPointer()) {
                TTree* tree = (TTree*)(node.getAddress());
                TObjArray* branches = tree->GetListOfBranches();
                this->iterateBranchArray(branches, visitor);
            }
            visitor.post(node);
        }
        else if (isBranch(node.getTypeName())) {
            TBranch* branch = (TBranch*)(node.getAddress());
            this->iterateBranch(node.getName(), branch, visitor);
        }
        else if (isDoubleBranch(node.getTypeName())) {
            TBranch* branch = (TBranch*)(node.getAddress());
            this->iterateEntries(node.getName(), branch, visitor);
        }
    }
    
    
    void iterateBranch(const std::string& name, TBranch* branch, IVisitor& visitor)
    {
        TObjArray* subBranches = branch->GetListOfBranches();
        
        if (subBranches->GetEntries() == 0) {
            this->iterateEntries(branch->GetName(), branch, visitor);
        }
        else {
            Node branchNode("TBranch", name, branch);
            if (visitor.pre(branchNode))
                this->iterateBranchArray(subBranches, visitor);
            visitor.post(branchNode);
        }
    }
    
    
    void iterateBranchArray(TObjArray* branches, IVisitor& visitor)
    {
        if (!branches)
            return;

        TIterator* iterator = branches->MakeIterator();
        TObject* obj;
        while ((obj = iterator->Next())) {
            TBranch* branch = static_cast<TBranch*>(obj);
            this->iterateBranch(branch->GetName(), branch, visitor);
        }
    }
    
    
    void iterateEntries(const std::string& name, TBranch* branch, IVisitor& visitor)
    {
        if (!branch)
            return;
        
        ArrayNode branchNode("TBranch<Double_t>", "Double_t", name, branch);
        if (visitor.pre(branchNode)) {
            Long64_t count = branch->GetEntries();
            for (Long64_t i = 0; i < count; ++i) {
                branch->GetEntry(i, 1);
                TObjArray* leaves = branch->GetListOfLeaves();
                this->iterateLeaves(leaves, i, visitor);
            }
        }
        visitor.post(branchNode);
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
                ArrayNode leafNode(leafType, "Double_t", leaf->GetName(), leaf);
                if (visitor.pre(leafNode)) {
                    for (Int_t j = 0; j < len; ++j) {
                        value = leaf->GetValue(j);
                        visitor.leaf(j, Data("Double_t", &value));
                    }
                }
                visitor.post(leafNode);
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
