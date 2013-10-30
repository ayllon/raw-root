#include <query/Operator.h>

using namespace scidb;

/**
 * Logical Root Loader operator
 */
class LogicalRootLoader: public scidb::LogicalOperator
{
public:
    LogicalRootLoader(const string& logicalName, const std::string& alias):
        LogicalOperator(logicalName, alias)
    {
        ADD_PARAM_CONSTANT("string");
        ADD_PARAM_CONSTANT("string");
    }
    
    ArrayDesc inferSchema(std::vector<ArrayDesc> schemas, boost::shared_ptr<Query> query)
    {
        const string& filePath = ((boost::shared_ptr<OperatorParamPhysicalExpression>&)_parameters[0])->getExpression()->evaluate().getString();
        const string& objName = ((boost::shared_ptr<OperatorParamPhysicalExpression>&)_parameters[1])->getExpression()->evaluate().getString();
        
        // Cells are one single float. At least, for the moment being.
        Attributes attrs(1);
        attrs[0] = AttributeDesc(0, "file",  TID_STRING, 0, 0);

        Dimensions outputDims;
        outputDims.push_back(DimensionDesc("v",
            0, 0, // start, end
            1, 0  // chunk interval, chunk overlap
        ));

        return ArrayDesc("rootloader_output", attrs, outputDims);
    }
};

REGISTER_LOGICAL_OPERATOR_FACTORY(LogicalRootLoader, "rootloader");
