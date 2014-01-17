#include <list>
#include <TFile.h>
#include <Walker.hpp>
#include <TypeResolver.hpp>

#include "Inferrer.hpp"
#include "InferrerException.hpp"
#include "TypeWrappers.hpp"

#ifndef BUILD_PATH
#  define BUILD_PATH ""
#endif

using namespace raw::root;


static std::string getHandlerLibraryPath(const std::string& path)
{
    if (!path.empty())
        return path;
    const char* envPath = getenv("HANDLER_LIBRARY_PATH");
    if (envPath)
        return envPath;
    return std::string(BUILD_PATH);
}



static std::string jstring2str(JNIEnv *jvEnv, jstring jvStr)
{
    if (jvStr == NULL)
        return std::string();
    const char *c = jvEnv->GetStringUTFChars(jvStr, 0);
    std::string stdStr(c);
    jvEnv->ReleaseStringUTFChars(jvStr, c);
    return stdStr;
}

std::shared_ptr<BaseType> buildJavaNode(JNIEnv *jvEnv, std::shared_ptr<Node> node,
        std::list<std::shared_ptr<BaseType> > stack)
{
    std::shared_ptr<BaseType> obj;
    switch (node->getType()) {
        case Node::kDictionary:
            obj.reset(new DictionaryType(jvEnv));
            break;
        case Node::kCollection:
            obj.reset(new CollectionType(jvEnv));
            break;
        case Node::kInt8: case Node::kInt32: case Node::kInt64:
        case Node::kUInt8: case Node::kUInt32: case Node::kUInt64:
            obj.reset(new IntegerType(jvEnv));
            break;
        case Node::kFloat: case Node::kDouble:
            obj.reset(new DoubleType(jvEnv));
            break;
        case Node::kString:
            obj.reset(new StringType(jvEnv));
            break;
        default:
            obj.reset(new UnknownType(jvEnv));
            break;
    }

    if (stack.size() > 0) {
        std::shared_ptr<BaseType> parent = stack.back();
        parent->append(node->getName(), obj);
    }

    return obj;
}

class Inferrer: public IVisitor
{
protected:
    JNIEnv *jvEnv;
    std::list<std::shared_ptr<BaseType>> stack;
    std::shared_ptr<BaseType> root;

public:
    Inferrer(JNIEnv *jvEnv): jvEnv(jvEnv)
    {
    }

    virtual ~Inferrer()
    {
    }

    bool pre(const std::shared_ptr<Node> node)
    {
        std::shared_ptr<BaseType> jvNode = buildJavaNode(jvEnv, node, stack);
        if (jvNode) {
            stack.push_back(jvNode);
            if (!root)
                root = jvNode;
        }
        return true;
    }

    void post(const std::shared_ptr<Node> node)
    {
        if (stack.size() > 0)
            stack.pop_back();
    }

    void unknown(const std::shared_ptr<Node> node)
    {
    }

    jobject getSchema()
    {
        if (root)
            return root->getJObject();
        else
            return NULL;
    }
};



jobject JNICALL Java_raw_root_Inferrer_infer(JNIEnv *jvEnv, jobject jvSelf, jstring jvFile, jstring jvNode, jstring jvHandlerPath)
{
    std::string filePath = jstring2str(jvEnv, jvFile);
    std::string nodePath = jstring2str(jvEnv, jvNode);
    std::string handlerPath = jstring2str(jvEnv, jvHandlerPath);

    std::shared_ptr<TypeResolver> resolver(new TypeResolver(getHandlerLibraryPath(handlerPath)));
    Walker walker(resolver);

    try {
        TFile rootFile(filePath.c_str());
        if (rootFile.IsZombie())
            throw InferrerException("Could not open the root file");

        std::shared_ptr<Node> rootNode(new Node(rootFile));
        if (!nodePath.empty() && rootNode)
            rootNode = walker.getChildNode(rootNode, nodePath);

        if (!rootNode)
            throw InferrerException("Could not load the given node");

        std::shared_ptr<Inferrer> inferrer(new Inferrer(jvEnv));
        walker.walk(rootNode, inferrer);

        return inferrer->getSchema();
    }
    catch (const std::exception& e) {
        jvEnv->ThrowNew(jvEnv->FindClass("java/lang/Exception"), e.what());
    }

    return NULL;
}
