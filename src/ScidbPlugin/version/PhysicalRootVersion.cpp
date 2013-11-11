#include <array/Metadata.h>
#include <array/TupleArray.h>
#include <query/Operator.h>

#include <TROOT.h>

using namespace scidb;

/**
 * Physical Root version operator
 */
class PhysicalRootVersion: public PhysicalOperator
{
public:
    PhysicalRootVersion(const string& logicalName, const string& physicalName,
                        const Parameters& parameters, const ArrayDesc& schema):
        PhysicalOperator(logicalName, physicalName, parameters, schema)
    {
    }
    
    boost::shared_ptr<Array> execute(vector<boost::shared_ptr<Array> >& inputArrays, boost::shared_ptr<Query> query)
    {
        if (query->getCoordinatorID() != COORDINATOR_INSTANCE )
        {
            return shared_ptr<Array> (new MemArray(_schema, query));
        }
        
        vector<boost::shared_ptr<Tuple> > tuples;
        Tuple &tuple = *new Tuple(1);
        tuple[0].setString(ROOT::GetROOT()->GetVersion());
        tuples.push_back(boost::shared_ptr<Tuple>(&tuple));
        
        return boost::shared_ptr<Array>(new TupleArray(_schema, tuples));
    }
};

REGISTER_PHYSICAL_OPERATOR_FACTORY(PhysicalRootVersion, "rootversion", "rootversion_impl");
