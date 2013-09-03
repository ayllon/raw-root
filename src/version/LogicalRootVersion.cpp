#include <query/Operator.h>

using namespace scidb;

/**
 * Logical Root Version operator
 */
class LogicalRootVersion: public scidb::LogicalOperator
{
public:
    LogicalRootVersion(const string& logicalName, const std::string& alias):
        LogicalOperator(logicalName, alias)
    {
        // No parameters
    }
    
    ArrayDesc inferSchema(std::vector< ArrayDesc> schemas, boost::shared_ptr< Query> query)
    {
        Attributes outputAttrs;
        outputAttrs.push_back(AttributeDesc(0, "version", TID_STRING, 0, 0));

        Dimensions outputDims;
        outputDims.push_back(DimensionDesc("v",
            0, 0, // start, end
            1, 0  // chunk interval, chunk overlap
        ));

        return ArrayDesc("rootversion_output", outputAttrs, outputDims);
    }
};

REGISTER_LOGICAL_OPERATOR_FACTORY(LogicalRootVersion, "rootversion");
