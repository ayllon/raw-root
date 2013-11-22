/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class raw_root_Walker */

#ifndef _Included_raw_root_Walker
#define _Included_raw_root_Walker
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     raw_root_Walker
 * Method:    walk
 * Signature: (Lraw/root/Node;Lraw/root/IVisitor;)V
 */
JNIEXPORT void JNICALL Java_raw_root_Walker_walk
  (JNIEnv *, jobject, jobject, jobject);

/*
 * Class:     raw_root_Walker
 * Method:    getChildNode
 * Signature: (Lraw/root/Node;Ljava/lang/String;)Lraw/root/Node;
 */
JNIEXPORT jobject JNICALL Java_raw_root_Walker_getChildNode
  (JNIEnv *, jobject, jobject, jstring);

/*
 * Class:     raw_root_Walker
 * Method:    initialize
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_raw_root_Walker_initialize
  (JNIEnv *, jobject);

/*
 * Class:     raw_root_Walker
 * Method:    finalize
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_raw_root_Walker_finalize
  (JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif
