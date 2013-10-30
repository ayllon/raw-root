#include <array/Metadata.h>
#include <array/TupleArray.h>
#include <query/Operator.h>

using namespace scidb;

/**
 * Physical Root version operator
 */
class PhysicalRootLoader: public PhysicalOperator
{
public:
    PhysicalRootLoader(const string& logicalName, const string& physicalName,
                        const Parameters& parameters, const ArrayDesc& schema):
        PhysicalOperator(logicalName, physicalName, parameters, schema)
    {
    }
    
    boost::shared_ptr<Array> execute(vector<boost::shared_ptr<Array> >& inputArrays, boost::shared_ptr<Query> query)
    {
        if (query->getCoordinatorID() != COORDINATOR_INSTANCE ) {
            return shared_ptr<Array> (new MemArray(_schema));
        }
        
        const string& filePath = ((boost::shared_ptr<OperatorParamPhysicalExpression>&)_parameters[0])->getExpression()->evaluate().getString();
        
        vector<boost::shared_ptr<Tuple> > tuples;
        Tuple &tuple = *new Tuple(1);
        tuple[0].setString(filePath.c_str());
        tuples.push_back(boost::shared_ptr<Tuple>(&tuple));
        
        return boost::shared_ptr<Array>(new TupleArray(_schema, tuples));
    }
};

REGISTER_PHYSICAL_OPERATOR_FACTORY(PhysicalRootLoader, "rootloader", "rootloader_impl");
