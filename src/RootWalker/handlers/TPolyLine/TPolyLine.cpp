#include <TPolyLine.h>
#include "../../TypeHandler.hpp"
#include "../../TypeResolver.hpp"

using namespace scidb::root;


class TPolyLineHandler: public ITypeHandler
{
public:
    
    bool recognize(const std::string& typeName)
    {
        return typeName.compare("TPolyLine") == 0;
    }
    
    
    void inspect(const std::string& typeName, bool isPointer, 
                 const std::string& name, const void* addr,
                 IVisitor& visitor)
    {
        if (visitor.pre(typeName, false, name, addr) && !isPointer) {
            const TPolyLine* poly = static_cast<const TPolyLine*>(addr);
            Int_t nPoints = poly->GetN();
            // Number of elements
            visitor.leaf("N", Data("Int_t", &nPoints));
            // First X
            if (visitor.pre("Double_t", true, "x", addr))
                this->iterate(nPoints, poly->GetX(), visitor);
            visitor.post("Double_t", true, "x", addr);
            // Then Y
            if (visitor.pre("Double_t", true, "y", addr))
                this->iterate(nPoints, poly->GetY(), visitor);
            visitor.post("Double_t", true, "y", addr);
        }
        visitor.post(typeName, false, name, addr);
    }
    
    
    void iterate(Int_t nPoints, Double_t* array, IVisitor& visitor)
    {
        for (Int_t i = 0; i < nPoints; ++i)
            visitor.leaf(i, Data("Double_t", array + i));
    }
};


/// This method must register the type handlers
extern "C" void registerTypes(TypeResolver* resolver)
{
    resolver->registerHandler(new TPolyLineHandler());
}
