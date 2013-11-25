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

    bool pre(const raw::root::Node& node)
    {
        ScopedJObject<const raw::root::Node> jNode(jEnv, "raw/root/Node", &node);
        int rvalue = jEnv->CallBooleanMethod(jVisitor, jPre, jNode.getObject());
        return rvalue != 0;
    }

    void post(const raw::root::Node& node)
    {
        ScopedJObject<const raw::root::Node> jNode(jEnv, "raw/root/Node", &node);
        jEnv->CallVoidMethod(jVisitor, jPost, jNode.getObject());
    }

    void unknown(const raw::root::Node& node)
    {

    }

    void leaf(const std::string& name, const raw::root::Data& data)
    {

    }

    void leaf(size_t index, const raw::root::Data& data)
    {

    }
};



void JNICALL Java_raw_root_Walker_walk(JNIEnv *env, jobject self, jobject node, jobject visitor)
{
    raw::root::Node *cnode = GetNativePtr<raw::root::Node>(env, node);
    raw::root::Walker *walker = GetNativePtr<raw::root::Walker>(env, self);

    if (!cnode) {
        env->ThrowNew(env->FindClass("java/lang/Exception"), "Node is NULL");
        return;
    }

    BridgeVisitor bVisitor(env, visitor);
    walker->walk(*cnode, bVisitor);
}



jobject JNICALL Java_raw_root_Walker_getChildNode(JNIEnv *env, jobject self, jobject node, jstring path)
{
    raw::root::Node *cnode = GetNativePtr<raw::root::Node>(env, node);
    raw::root::Walker *walker = GetNativePtr<raw::root::Walker>(env, self);

    const char *cpath = env->GetStringUTFChars(path, 0);
    raw::root::Node *newcNode = new raw::root::Node(walker->getChildNode(*cnode, cpath));
    env->ReleaseStringUTFChars(path, cpath);

    if (!newcNode)
        return NULL;

    if (newcNode == cnode)
        return node;

    jclass nodeCls = env->FindClass("raw/root/Node");
    jmethodID nodeConstructor = env->GetMethodID(nodeCls, "<init>", "()V");
    jobject rnode = env->NewObject(nodeCls, nodeConstructor);

    SetNativePtr<raw::root::Node>(env, rnode, newcNode);
    return rnode;
}



void JNICALL Java_raw_root_Walker_initialize(JNIEnv *env, jobject self)
{
    raw::root::TypeResolver *resolver = new raw::root::TypeResolver(getHandlerLibraryPath());
    raw::root::Walker *walker = new raw::root::Walker(*resolver);

    SetNativePtr<raw::root::TypeResolver>(env, self, "typeResolverPtr", resolver);
    SetNativePtr<raw::root::Walker>(env, self, walker);
}


void JNICALL Java_raw_root_Walker_finalize(JNIEnv *env, jobject self)
{
    raw::root::Walker *walker = GetNativePtr<raw::root::Walker>(env, self);
    raw::root::TypeResolver *resolver = GetNativePtr<raw::root::TypeResolver>(env, self, "typeResolverPtr");

    //delete walker;
    //delete resolver;

    SetNativePtr<raw::root::TypeResolver>(env, self, "typeResolverPtr", nullptr);
    SetNativePtr<raw::root::Walker>(env, self, nullptr);
}
