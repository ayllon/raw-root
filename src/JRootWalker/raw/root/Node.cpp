#include <RootWalker/Node.hpp>
#include <TFile.h>
#include "Node.h"
#include "Tools.h"

jstring JNICALL Java_raw_root_Node_getTypeName(JNIEnv *env, jobject self)
{
    raw::root::Node *node = GetNativePtr<raw::root::Node>(env, self);
    return env->NewStringUTF(node->getTypeName().c_str());
}



jstring JNICALL Java_raw_root_Node_getName(JNIEnv *env, jobject self)
{
    raw::root::Node *node = GetNativePtr<raw::root::Node>(env, self);
    return env->NewStringUTF(node->getName().c_str());
}



jboolean JNICALL Java_raw_root_Node_isBasic(JNIEnv *env, jobject self)
{
    raw::root::Node *node = GetNativePtr<raw::root::Node>(env, self);
    return node->isBasic();
}



jobject JNICALL Java_raw_root_Node_getData(JNIEnv *env, jobject self)
{
    return NULL;
}



jboolean JNICALL Java_raw_root_Node_isArray(JNIEnv *env, jobject self)
{
    raw::root::Node *node = GetNativePtr<raw::root::Node>(env, self);
    return node->isArray();
}



jstring JNICALL Java_raw_root_Node_getContainedType(JNIEnv *env, jobject self)
{
    raw::root::Node *node = GetNativePtr<raw::root::Node>(env, self);
    return env->NewStringUTF(node->getContainedType().c_str());
}



jboolean JNICALL Java_raw_root_Node_isNull(JNIEnv *env, jobject self)
{
    raw::root::Node *node = GetNativePtr<raw::root::Node>(env, self);
    return !(static_cast<bool>(node));
}



void JNICALL Java_raw_root_Node_initialize__(JNIEnv *env, jobject self)
{
    raw::root::Node *node = new raw::root::Node();
    SetNativePtr<raw::root::Node>(env, self, node);
    SetNativePtr<TFile>(env, self, "tFilePtr", nullptr);
}



void JNICALL Java_raw_root_Node_initialize__Ljava_lang_String_2(JNIEnv *env, jobject self, jstring path)
{
    const char* cpath = env->GetStringUTFChars(path, 0);
    TFile* file = new TFile(cpath);
    env->ReleaseStringUTFChars(path, cpath);

    if (file->IsZombie()) {
        delete file;
        env->ThrowNew(env->FindClass("java/lang/Exception"), "Could not open the ROOT file");
        return;
    }

    raw::root::Node *node = new raw::root::Node(*file);
    SetNativePtr<raw::root::Node>(env, self, node);
    SetNativePtr<TFile>(env, self, "tFilePtr", file);
}



void JNICALL Java_raw_root_Node_finalize(JNIEnv *env, jobject self)
{
    raw::root::Node *node = GetNativePtr<raw::root::Node>(env, self);
    //delete node;
    SetNativePtr<raw::root::Node>(env, self, nullptr);

    TFile* file = GetNativePtr<TFile>(env, self, "tFilePtr");
    //delete file;
    SetNativePtr<TFile>(env, self, "tFilePtr", nullptr);
}
