#include <boost/shared_ptr.hpp>
#include <boost/concept_check.hpp>
#include <iostream>
#include <iomanip>

#include <TClass.h>
#include <TDataMember.h>
#include <TDataType.h>
#include <TFile.h>
#include <TKey.h>
#include <TMemberInspector.h>
#include <TStreamerElement.h>
#include <TVirtualStreamerInfo.h>
#include <TROOT.h>

using namespace ROOT;


class MyInspector: public TMemberInspector
{
private:
    unsigned tabs;
public:
    MyInspector(): tabs(0)
    {
    }
    
    void Inspect(TClass* klass, const char* parent, const char* name, const void* addr)
    {
        std::string indent(tabs, '\t');
        
        TDataType* memberType = NULL;
        TString memberTypeName;
        TString memberFullTypeName;
        TString memberName;
        TString memberValueAsStr;
        Bool_t  isPointer;
        
        if (TDataMember* member = klass->GetDataMember(name)) {
            memberTypeName     = member->GetTypeName();
            memberFullTypeName = member->GetFullTypeName();
            memberType         = member->GetDataType(); // Only for basic types
            memberName         = member->GetName();
            isPointer          = member->IsaPointer();
        }
        else if (!klass->IsLoaded()) {
            // The class hasn't been loaded
            TVirtualStreamerInfo* info = klass->GetStreamerInfo();
            if (!info) return;
            const char* cursor = name;
            while ((*cursor) == '*') ++cursor;
            TString elname(cursor);
            Ssiz_t pos = elname.Index("[");
            if (pos != kNPOS)
                elname.Remove(pos);
            TStreamerElement* element = static_cast<TStreamerElement*>(info->GetElements()->FindObject(elname.Data()));
            if (!element) return;
            memberFullTypeName = element->GetTypeName();
            memberType     = gROOT->GetType(memberTypeName);           
            memberName = element->GetName();
            isPointer = element->IsaPointer() || element->GetType() == TVirtualStreamerInfo::kCharStar;
            
            memberTypeName = memberFullTypeName;
        }
        else {
            return;
        }
        
        TClass* dataClass = NULL;
        if (isPointer) {
            char buffer[32];
            snprintf(buffer, sizeof(buffer), "0x%lx", (off64_t)addr);
            memberValueAsStr = buffer;
        }
        else if(memberType) {
            memberValueAsStr = memberType->AsString((void*)addr);
        }
        else {
            char buffer[32];
            dataClass = TClass::GetClass(memberFullTypeName);
            
            
            if (dataClass == TString::Class()) {
                TString* str = (TString*)addr;
                memberValueAsStr = *str;
                dataClass = NULL;
            }
            else {
                snprintf(buffer, sizeof(buffer), "-> 0x%lx", (off64_t)addr);
                memberValueAsStr = buffer;
            }
        }
        
        std::cout << indent << std::setw(20) << memberFullTypeName << " "
                  << klass->GetName() << "::" << memberName << " = " << memberValueAsStr
                  << std::endl;
    }
    
    void Inspect(const TDirectory* dir)
    {
        std::string indent(tabs, '\t');
        
        Int_t nelements = dir->GetNkeys();
        std::cout << indent << nelements << " elements" << std::endl;
        
        TList* keys = dir->GetListOfKeys();
        for (Int_t i = 0; i < nelements; ++i) {
            TKey* key = static_cast<TKey*>(keys->At(i));
            std::cout << indent << key->GetName() << std::endl;
            ++tabs;
            key->ReadObj()->ShowMembers(*this);
            --tabs;
        }
    }
};


int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Expecting a file name as a parameter" << std::endl;
        return -1;
    }
    
    boost::shared_ptr<TFile> f(new TFile(argv[1]));
    MyInspector inspector;
    
    // Call inspect on the file
    inspector.Inspect(f.get());
    
    return 0;
}
