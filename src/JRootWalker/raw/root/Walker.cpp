#include <stdio.h>
#include <stdlib.h>
#include <RootWalker/Walker.hpp>
#include "Walker.h"


void JNICALL Java_raw_root_Walker_walk(JNIEnv *env, jobject self, jobject node, jobject visitor)
{
}



jobject JNICALL Java_raw_root_Walker_getChildNode(JNIEnv *env, jobject self, jobject node, jstring path)
{
    return NULL;
}


void JNICALL Java_raw_root_Walker_initialize(JNIEnv *env, jobject self)
{
}


void JNICALL Java_raw_root_Walker_finalize(JNIEnv *env, jobject self)
{
}
