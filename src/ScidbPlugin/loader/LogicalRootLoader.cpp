#include <log4cxx/logger.h>
#include <query/Operator.h>
#include <system/Exceptions.h>
#include <TypeResolver.hpp>
#include <Walker.hpp>

#include "AttrFinder.hpp"
#include "Utils.hpp"

using namespace scidb;
using namespace scidb::root;

static log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("scidb.common.thread"));

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
    
    
    void getParameters(boost::shared_ptr<Query> query, string& filePath, string& objPath)
    {
        if (_parameters.size() < 2) {
            throw SYSTEM_EXCEPTION(SCIDB_SE_INTERNAL, SCIDB_LE_ILLEGAL_OPERATION)
                  << "illegal number of parameters passed to LogicalRootLoader";
        }
        
        shared_ptr<OperatorParam>const& fParam = _parameters[0];
        shared_ptr<OperatorParam>const& oParam = _parameters[1];
        
        filePath = evaluate(((shared_ptr<OperatorParamLogicalExpression>&) fParam)->
                        getExpression(), query, TID_STRING).getString();
        objPath = evaluate(((shared_ptr<OperatorParamLogicalExpression>&) oParam)->
                        getExpression(), query, TID_STRING).getString();
    }
    
    
    ArrayDesc inferSchema(std::vector<ArrayDesc> schemas, boost::shared_ptr<Query> query)
    {
        
        string filePath;
        string objPath;
        getParameters(query, filePath, objPath);
        
        TFile rFd(filePath.c_str());
        if (rFd.IsZombie()) {
            char errDescr[64];
            strerror_r(rFd.GetErrno(), errDescr, sizeof(errDescr));
            LOG4CXX_FATAL(logger, "Could not load " << filePath << ": " << errDescr);
            throw SYSTEM_EXCEPTION(SCIDB_SE_OPERATOR, SCIDB_LE_CANT_OPEN_FILE) << filePath << rFd.GetErrno();
        }
        
        TypeResolver typeResolver(getHandlerLibraryPath(), logger);        
        
        Walker walker(typeResolver);
        Node rootNode(rFd);
        rootNode = walker.getChildNode(rootNode, objPath);
        
        if (!rootNode) {
            LOG4CXX_FATAL(logger, "Could not find the object " << filePath << "::" << objPath);
            throw SYSTEM_EXCEPTION(SCIDB_SE_OPERATOR, SCIDB_LE_ARRAY_DOESNT_EXIST) << objPath;
        }
        
        LOG4CXX_DEBUG(logger, "Loaded " << filePath << "::" << objPath);
        
        // Figure out the attributes
        AttrFinder arrayFinder;
        walker.walk(rootNode, arrayFinder);
        
        boost::shared_ptr<std::vector<Node> > arrayAttrs = arrayFinder.getAttributes();
        size_t nAttrs = arrayAttrs->size();
        
        if (nAttrs == 0)
            throw SYSTEM_EXCEPTION(SCIDB_SE_OPERATOR, SCIDB_LE_OP_NORMALIZE_ERROR3);
        
        LOG4CXX_DEBUG(logger, "Found " << nAttrs << " valid attributes");
        Attributes attrs(nAttrs);
        
        for (size_t i = 0; i < nAttrs; ++i) {
            const Node& node = (*arrayAttrs)[i];
            LOG4CXX_DEBUG(logger, "\t" << node.getName());
            attrs[i] = AttributeDesc(0, node.getName(), TID_DOUBLE, 0, 0);
        }

        // Dimensions
        Dimensions outputDims;
        outputDims.push_back(DimensionDesc("v",
            0, 0, // start, end
            1, 0  // chunk interval, chunk overlap
        ));

        return ArrayDesc("rootloader_output", attrs, outputDims);
    }
};

REGISTER_LOGICAL_OPERATOR_FACTORY(LogicalRootLoader, "rootloader");
