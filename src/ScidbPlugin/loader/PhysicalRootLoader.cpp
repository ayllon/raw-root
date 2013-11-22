#include <array/Metadata.h>
#include <array/TupleArray.h>
#include <query/Operator.h>
#include <TypeResolver.hpp>
#include <Walker.hpp>

#include "AttrFinder.hpp"
#include "RootLoaderArray.hpp"
#include "Utils.hpp"

using namespace scidb;
using namespace raw::root;

static log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("scidb.common.thread"));

/**
 * Physical Root Loader operator
 */
class PhysicalRootLoader: public PhysicalOperator
{
public:
    PhysicalRootLoader(const string& logicalName, const string& physicalName,
                        const Parameters& parameters, const ArrayDesc& schema):
        PhysicalOperator(logicalName, physicalName, parameters, schema)
    {
    }
    
    
    void getParameters(std::string& filePath, std::string& objPath)
    {
        filePath = ((boost::shared_ptr<OperatorParamPhysicalExpression>&)_parameters[0])->getExpression()->evaluate().getString();
        objPath = ((boost::shared_ptr<OperatorParamPhysicalExpression>&)_parameters[1])->getExpression()->evaluate().getString();
    }
    
    
    boost::shared_ptr<Array> execute(vector<boost::shared_ptr<Array> >& inputArrays, boost::shared_ptr<Query> query)
    {
        if (query->getCoordinatorID() != COORDINATOR_INSTANCE ) {
            return shared_ptr<Array> (new MemArray(_schema, query));
        }
        
        string filePath;
        string objPath;
        getParameters(filePath, objPath);
        
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
        
        // Get the attributes
        AttrFinder arrayFinder;
        walker.walk(rootNode, arrayFinder);
        
        boost::shared_ptr<std::vector<Node> > arrayAttrs = arrayFinder.getAttributes();
        if (arrayAttrs->size() != _schema.getAttributes().size()) {
            throw SYSTEM_EXCEPTION(SCIDB_SE_OPERATOR, SCIDB_LE_UNKNOWN_ERROR)
                    << "Schema number of attributes and inferred number of attributes do not match";
        }
        
        return boost::shared_ptr<Array>(new RootLoaderArray(_schema, query, arrayAttrs));
    }
};

REGISTER_PHYSICAL_OPERATOR_FACTORY(PhysicalRootLoader, "rootloader", "rootloader_impl");
