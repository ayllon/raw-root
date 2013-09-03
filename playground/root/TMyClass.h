#include <TObject.h>
#include <TTree.h>
#include <TVector.h>

/**
 * Convenience class just to generate a easy struct
 * I know to see how to traverse it
 */
class TMyClass: public TObject
{
protected:
    void fillVector() {
        // Just fill several floats with incrementing values
        fFloats.ResizeTo(1000);
        for (Float_t f = 0; f < fFloats.GetNoElements(); ++f) {
            fFloats[static_cast<Int_t>(f)] = f;
        }
    }
    
    void fillTree() {
    }
    
public:
    TString fDescription;
    TTree   fTree;
    TVector fFloats;

    TMyClass() {
    }
    
    TMyClass(const TString& descr): fDescription(descr) {
        fillVector();
        fillTree();
    }
    
    ClassDef (TMyClass, 1);
};
