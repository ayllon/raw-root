#include <TNamed.h>
#include <TTree.h>
#include <TVector.h>

/**
 * Convenience class just to generate a easy struct
 * I know to see how to traverse it
 */
class TMyClass: public TNamed
{
protected:
    void fillVector() {
        // Just fill several floats with incrementing values
        fFloats.ResizeTo(1000);
        for (Int_t f = 0; f < fFloats.GetNoElements(); ++f) {
            fFloats[f] = f;
        }
        fInts.push_back(6);
		fInts.push_back(99);
		
		something = 42;
		value = 88;
    }
    
    void fillTree() {
    }
    
public:
    TString fDescription;
    TTree   fTree;
    TVector fFloats;
	std::vector<Int_t> fInts;
	
	Double_t something;
	Long64_t value;

    TMyClass() {
    }
    
    TMyClass(const TString& descr): fDescription(descr) {
        fillVector();
        fillTree();
    }
    
    ClassDef (TMyClass, 1);
};
