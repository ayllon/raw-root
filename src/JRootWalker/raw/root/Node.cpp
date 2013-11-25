#include <RootWalker/Node.hpp>
#include <TFile.h>
#include "Node.h"
#include "Tools.h"

using namespace raw::root;

jstring JNICALL Java_raw_root_Node_getTypeName(JNIEnv *env, jobject self)
{
    NativePtr<Node> nNode(env, self);
    return env->NewStringUTF(nNode->getTypeName().c_str());
}



jstring JNICALL Java_raw_root_Node_getName(JNIEnv *env, jobject self)
{
    NativePtr<Node> nNode(env, self);
    return env->NewStringUTF(nNode->getName().c_str());
}



jboolean JNICALL Java_raw_root_Node_isBasic(JNIEnv *env, jobject self)
{
    NativePtr<Node> nNode(env, self);
    return nNode->isBasic();
}



jboolean JNICALL Java_raw_root_Node_isArray(JNIEnv *env, jobject self)
{
    NativePtr<Node> nNode(env, self);
    return nNode->isArray();
}



jstring JNICALL Java_raw_root_Node_getContainedType(JNIEnv *env, jobject self)
{
    NativePtr<Node> nNode(env, self);
    return env->NewStringUTF(nNode->getContainedType().c_str());
}



void JNICALL Java_raw_root_Node_initialize__(JNIEnv *env, jobject self)
{
    NativePtr<Node> nNode(env, self);
    nNode.initialize(std::shared_ptr<Node>(new Node()));
    NativePtr<TFile> nFile(env, self, "tFilePtr");
    nFile.initialize(std::shared_ptr<TFile>());
}



void JNICALL Java_raw_root_Node_initialize__Ljava_lang_String_2(JNIEnv *env, jobject self, jstring path)
{
    const char* cpath = env->GetStringUTFChars(path, 0);
    std::shared_ptr<TFile> shFile(new TFile(cpath));
    env->ReleaseStringUTFChars(path, cpath);

    if (shFile->IsZombie()) {
        env->ThrowNew(env->FindClass("java/lang/Exception"), "Could not open the ROOT file");
        return;
    }

    NativePtr<Node> nNode(env, self);
    nNode.initialize(std::shared_ptr<Node>(new Node(*shFile)));
    NativePtr<TFile> nFile(env, self, "tFilePtr");
    nFile.initialize(shFile);
}



void JNICALL Java_raw_root_Node_finalize(JNIEnv *env, jobject self)
{
    NativePtr<Node> nNode(env, self);
    nNode.finalize();
    NativePtr<TFile> nFile(env, self, "tFilePtr");
    nFile.finalize();
}
