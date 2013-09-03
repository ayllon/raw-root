#include <iostream>
#include <TFile.h>

int GenerateFile(const TString& library, const TString& file)
{
    std::cout << "Loading " << library << std::endl;
    gSystem->Load(library);
    
    std::cout << "Filling " << file << std::endl;
    TMyClass obj("A description");
    
    TFile out(file, "RECREATE");
    obj.Write();
    
    return 0;
}
