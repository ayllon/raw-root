#include <TNamed.h>

class TData: public TNamed {
public:
    Float_t x, y;
    
    ClassDef (TData, 1);
};
