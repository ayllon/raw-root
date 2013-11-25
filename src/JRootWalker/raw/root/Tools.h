#ifndef __TOOLS_H
#define __TOOLS_H

#include <memory>

template <typename T>
class NativePtr
{
private:
    JNIEnv *env;
    jobject obj;
    const char *fieldName;
    jclass klass;
    jfieldID fieldId;
    std::shared_ptr<T> *pointerToShared;

public:
    NativePtr(JNIEnv *env, jobject obj, const char *fieldName = "ptr"):
        env(env), obj(obj), fieldName(fieldName)
    {
        klass = env->GetObjectClass(obj);
        fieldId = env->GetFieldID(klass, fieldName, "J");
        pointerToShared = reinterpret_cast<std::shared_ptr<T>*>(env->GetLongField(obj, fieldId));
    }

    ~NativePtr()
    {
        // Pass
    }

    std::shared_ptr<T>& getPtr()
    {
        return *pointerToShared;
    }

    std::shared_ptr<T>& operator -> ()
    {
        return *pointerToShared;
    }

    void initialize(std::shared_ptr<T> shPtr)
    {
        pointerToShared = new std::shared_ptr<T>(shPtr);
        env->SetLongField(obj, fieldId, reinterpret_cast<long>(pointerToShared));
    }

    void finalize()
    {
        pointerToShared->reset();
        delete pointerToShared;
        pointerToShared = nullptr;
        env->SetLongField(obj, fieldId, 0);
    }
};


template <typename T>
class ScopedJObject
{
private:
    JNIEnv *env;
    jobject obj;

public:
    ScopedJObject(JNIEnv *env, const char *type, const std::shared_ptr<T> ptr): env(env) {
        jclass klass = env->FindClass(type);
        jmethodID constructor = env->GetMethodID(klass, "<init>", "()V");
        obj = env->NewObject(klass, constructor);

        NativePtr<T> nPtr(env, obj);
        nPtr.initialize(ptr);
    }

    ~ScopedJObject() {
        env->DeleteLocalRef(obj);
    }

    jobject getObject() {
        return obj;
    }
};

#endif // __TOOLS_H
