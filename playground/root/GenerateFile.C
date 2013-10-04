#include <iostream>
#include <TFile.h>
#include <TVector.h>

int GenerateFile(const TString& library, const TString& file)
{
    std::cout << "Loading " << library << std::endl;
    gSystem->Load(library);
    
    std::cout << "Filling " << file << std::endl;
    TMyClass obj("A description");
	obj.SetName("Hello");
	obj.fFloats[2] = 5;
    
    TVector firstLevelVector;
    firstLevelVector.ResizeTo(20);
    for (size_t i = 0; i < firstLevelVector.GetNoElements(); ++i)
        firstLevelVector[i] = 42;
    
    TFile out(file, "RECREATE");
    obj.Write();
    firstLevelVector.Write();

    return 0;
}
