#include <dlfcn.h>
#include <dirent.h>
#include <boost/concept_check.hpp>
#include "TObjectHandler.hpp"
#include "TypeResolver.hpp"


using namespace raw::root;

/// Return true if fName is a module (i.e. ends with .so)
static bool isModule(const std::string& fName)
{
    size_t dot = fName.rfind('.');
    return dot != std::string::npos &&
           fName.compare(dot, 3, ".so") == 0;
}

/// Private implementation, not exposed
class raw::root::TypeResolverImpl
{
protected:
    typedef std::vector<ITypeHandler*> ITypeHandlerList;
    ITypeHandlerList handlers;
    
    TypeResolver* publicApi;
    log4cxx::LoggerPtr logger;
    
public:
    TypeResolverImpl(TypeResolver* publicApi, log4cxx::LoggerPtr logger):
        publicApi(publicApi), logger(logger)
    {
    }
    
    
    ~TypeResolverImpl()
    {
        ITypeHandlerList::iterator i;
        for (i = handlers.begin(); i != handlers.end(); ++i) {
            delete *i;
        }
    }
    
    
    ITypeHandler* getHandlerForType(const std::string& typeName)
    {
        ITypeHandlerList::iterator i;
        for (i = handlers.begin(); i != handlers.end(); ++i) {
            if ((*i)->recognize(typeName))
                return *i;
        }
        return nullptr;
    }
    
    
    void registerHandler(ITypeHandler* handler)
    {
        handlers.push_back(handler);
    }
    
    
    void registerHandler(const std::string& path)
    {
        void* dl = dlopen(path.c_str(), RTLD_NOW);
        if (dl) {
            typedef void (*RegisterCall)(TypeResolver*);
            RegisterCall regCall = (RegisterCall)(dlsym(dl, "registerTypes"));
            if (regCall) {
                regCall(publicApi);
                LOG4CXX_DEBUG(logger, "Registered plugin " << path);
            }
            else {
                LOG4CXX_DEBUG(logger, "Tried to register plugin " << path << " but could not find registerTypes");
            }
        }
        else {
            LOG4CXX_DEBUG(logger, "Could not register plugin " << path << ": " << dlerror());
        }
    }
    
    
    void loadHandlersFromLocation(const std::string& handlerLocation,
                                  unsigned depth = 0)
    {
        if (depth > 5)
            return;
        
        LOG4CXX_TRACE(logger, "Searching plugins under  " << handlerLocation);
        
        DIR* dirp = ::opendir(handlerLocation.c_str());
        if (dirp == nullptr)
            return;
        
        struct dirent entry;
        struct dirent *entryPtr = NULL;
        while (::readdir_r(dirp, &entry, &entryPtr) == 0 && entryPtr != nullptr) {
            std::string fullPath = handlerLocation + '/' + entry.d_name;
            if (entry.d_type == DT_DIR && entry.d_name[0] != '.')
                loadHandlersFromLocation(fullPath, depth + 1);
            else if (entry.d_type == DT_REG && isModule(entry.d_name))
                this->registerHandler(fullPath);
        }
        ::closedir(dirp);
    }
};



TypeResolver::TypeResolver(const std::string& handlerLocation, log4cxx::LoggerPtr logger):
    implPtr(new TypeResolverImpl(this, logger))
{
    implPtr->loadHandlersFromLocation(handlerLocation);
    // Force generic object handler last
    implPtr->registerHandler(new TObjectHandler(this));
}


TypeResolver::~TypeResolver()
{
    delete implPtr;
}


ITypeHandler* TypeResolver::getHandlerForType(const std::string& typeName)
{
    return implPtr->getHandlerForType(typeName);
}


void TypeResolver::registerHandler(ITypeHandler* handler)
{
    implPtr->registerHandler(handler);
}
