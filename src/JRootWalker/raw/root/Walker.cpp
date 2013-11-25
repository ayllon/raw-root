#include <stdio.h>
#include <stdlib.h>
#include <RootWalker/TypeResolver.hpp>
#include <RootWalker/Walker.hpp>
#include "Node.h"
#include "Walker.h"
#include "Tools.h"

#ifndef BUILD_PATH
#  define BUILD_PATH ""
#endif

using namespace raw::root;


static std::string getHandlerLibraryPath(void)
{
    const char* envPath = getenv("HANDLER_LIBRARY_PATH");
    if (envPath)
        return envPath;
    return std::string(BUILD_PATH);
}



class BridgeVisitor: public raw::root::IVisitor
{
private:
    JNIEnv *jEnv;
    jobject jVisitor;

    jmethodID jPre, jPost, jUnknown, jLeafNamed, jLeafIndex;

public:
    BridgeVisitor(JNIEnv *env, jobject jv): jEnv(env), jVisitor(jv)
    {
        jclass vClass = jEnv->GetObjectClass(jVisitor);
        jPre       = jEnv->GetMethodID(vClass, "pre", "(Lraw/root/Node;)Z");
        jPost      = jEnv->GetMethodID(vClass, "post", "(Lraw/root/Node;)V");
        jUnknown   = jEnv->GetMethodID(vClass, "unknown", "(Lraw/root/Node;)V");
        //jLeafNamed = jEnv->GetMethodID(vClass, "leaf", "(Ljava/lang/String;Lraw/root/Data;)V");
        //jLeafIndex = jEnv->GetMethodID(vClass, "leaf", "Signature: (JLraw/root/Data;)V");
    }

    virtual ~BridgeVisitor()
    {
    }

    bool pre(const std::shared_ptr<Node> node)
    {
        ScopedJObject<const Node> jNode(jEnv, "raw/root/Node", node);
        int rvalue = jEnv->CallBooleanMethod(jVisitor, jPre, jNode.getObject());
        return rvalue != 0;
    }

    void post(const std::shared_ptr<Node> node)
    {
        ScopedJObject<const Node> jNode(jEnv, "raw/root/Node", node);
        jEnv->CallVoidMethod(jVisitor, jPost, jNode.getObject());
    }

    void unknown(const std::shared_ptr<Node> node)
    {

    }

    void leaf(const std::string& name, const std::shared_ptr<Data> data)
    {

    }

    void leaf(size_t index, const std::shared_ptr<Data> data)
    {

    }
};



void JNICALL Java_raw_root_Walker_walk(JNIEnv *env, jobject self, jobject node, jobject visitor)
{
    NativePtr<Node> cnode(env, node);
    NativePtr<Walker> walker(env, self);

    BridgeVisitor bVisitor(env, visitor);
    walker->walk(cnode.getPtr(), &bVisitor);
}



jobject JNICALL Java_raw_root_Walker_getChildNode(JNIEnv *env, jobject self, jobject node, jstring path)
{
    NativePtr<Node> nNode(env, node);
    NativePtr<Walker> nWalker(env, self);

    const char *cpath = env->GetStringUTFChars(path, 0);
    std::shared_ptr<Node> childNode = nWalker->getChildNode(nNode.getPtr(), cpath);
    env->ReleaseStringUTFChars(path, cpath);

    if (!childNode)
        return NULL;

    jclass nodeCls = env->FindClass("raw/root/Node");
    jmethodID nodeConstructor = env->GetMethodID(nodeCls, "<init>", "()V");
    jobject jChildNode = env->NewObject(nodeCls, nodeConstructor);
    NativePtr<Node> nChildNode(env, jChildNode);

    nChildNode.initialize(childNode);

    return jChildNode;
}



void JNICALL Java_raw_root_Walker_initialize(JNIEnv *env, jobject self)
{
    NativePtr<TypeResolver> nResolver(env, self, "typeResolverPtr");
    nResolver.initialize(std::shared_ptr<TypeResolver>(new TypeResolver(getHandlerLibraryPath())));

    NativePtr<Walker> nWalker(env, self);
    nWalker.initialize(std::shared_ptr<Walker>(new Walker(nResolver.getPtr())));
}


void JNICALL Java_raw_root_Walker_finalize(JNIEnv *env, jobject self)
{
    NativePtr<TypeResolver> nResolver(env, self, "typeResolverPtr");
    NativePtr<Walker> nWalker(env, self);

    nWalker.finalize();
    nResolver.finalize();
}
