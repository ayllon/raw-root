#include <TPolyLine.h>
#include "../../TypeHandler.hpp"
#include "../../TypeResolver.hpp"

using namespace raw::root;


class TPolyLineHandler: public ITypeHandler
{
public:
    
    std::string getHandlerId()
    {
        return "TPolyLineHandler";
    }
    
    
    bool recognize(const std::string& typeName)
    {
        return typeName.compare("TPolyLine") == 0;
    }
    
    
    void inspect(const Node& node, IVisitor& visitor)
    {
        if (visitor.pre(node) && !node.isPointer()) {
            const TPolyLine* poly = static_cast<const TPolyLine*>(node.getAddress());
            Int_t nPoints = poly->GetN();
            // Number of elements
            visitor.leaf("N", Data("Int_t", &nPoints));
            // First X
            ArrayNode xArray("TPolyLine<Double_t>", "Double_t", "x", poly->GetX());
            if (visitor.pre(xArray))
                this->iterate(nPoints, poly->GetX(), visitor);
            visitor.post(xArray);
            // Then Y
            ArrayNode yArray("TPolyLine<Double_t>", "Double_t", "y", poly->GetY());
            if (visitor.pre(yArray))
                this->iterate(nPoints, poly->GetY(), visitor);
            visitor.post(yArray);
        }
        visitor.post(node);
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
