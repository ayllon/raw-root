#include <TPolyLine.h>
#include "TypeHandler.hpp"
#include "TypeResolver.hpp"

using namespace scidb::root;


class TPolyLineHandler: public ITypeHandler
{
public:
    
    bool recognize(const std::string& type)
    {
        return type.compare("TPolyLine") == 0;
    }
    
    
    void inspect(const std::string& typeName, bool isPointer, 
                 const std::string& name, const void* addr,
                 IVisitor& visitor)
    {
        if (visitor.pre(typeName, false, name) && !isPointer) {
            const TPolyLine* poly = static_cast<const TPolyLine*>(addr);
            Int_t nPoints = poly->GetN();
            // Number of elements
            visitor.leaf("N", Data("Int_t", &nPoints));
            // First X
            if (visitor.pre("Double_t", true, "x"))
                this->iterate(nPoints, poly->GetX(), visitor);
            visitor.post("Double_t", true, "x");
            // Then Y
            if (visitor.pre("Double_t", true, "y"))
                this->iterate(nPoints, poly->GetY(), visitor);
            visitor.post("Double_t", true, "y");
        }
        visitor.post(typeName, false, name);
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
