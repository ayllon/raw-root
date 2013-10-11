#include <iostream>
#include <TFile.h>
#include <TPolyLine.h>
#include <TVector.h>

int GenerateFile(const TString& library, const TString& file)
{
    std::cout << "Loading " << library << std::endl;
    gSystem->Load(library);
    
    std::cout << "Filling " << file << std::endl;
    TFile out(file, "RECREATE");
    
    // A class
    TMyClass obj("A description");
	obj.SetName("Hello");
	obj.fFloats[2] = 5;
    obj.Write();
    
    // A vector
    TVector firstLevelVector;
    firstLevelVector.ResizeTo(20);
    for (size_t i = 0; i < firstLevelVector.GetNoElements(); ++i)
        firstLevelVector[i] = 42;
    firstLevelVector.Write();
    
    // A list (inherits from TCollection)
    TList list;
    for (size_t i = 0; i < 10; ++i) {
        Double_t x[3] = {1, 2, 3};
        Double_t y[3] = {4, 5, 6};
        TPolyLine* line = new TPolyLine(3, x, y);
        list.Add(line);
    }
    list.Write("AList", TObject::kSingleKey);

    return 0;
}
