#ifndef __TOOLS_H
#define __TOOLS_H

inline jfieldID GetPtrField(JNIEnv *env, jobject obj, const char *fieldName)
{
    jclass c = env->GetObjectClass(obj);
    return env->GetFieldID(c, fieldName, "J");
}

template <typename T>
T *GetNativePtr(JNIEnv *env, jobject obj, const char *fieldName)
{
    return reinterpret_cast<T*>(env->GetLongField(obj, GetPtrField(env, obj, fieldName)));
}

template <typename T>
T *GetNativePtr(JNIEnv *env, jobject obj)
{
    return GetNativePtr<T>(env, obj, "ptr");
}

template <typename T>
void SetNativePtr(JNIEnv *env, jobject obj, const char *fieldName, T* ptr)
{
    env->SetLongField(obj, GetPtrField(env, obj, fieldName), reinterpret_cast<long>(ptr));
}

template <typename T>
void SetNativePtr(JNIEnv *env, jobject obj, T* ptr)
{
    SetNativePtr<T>(env, obj, "ptr", ptr);
}

template <typename T>
class ScopedJObject
{
private:
    JNIEnv *env;
    jobject obj;

public:
    ScopedJObject(JNIEnv *env, const std::string& type, T* ptr): env(env) {
        jclass klass = env->FindClass(type.c_str());
        jmethodID constructor = env->GetMethodID(klass, "<init>", "()V");
        obj = env->NewObject(klass, constructor);
        SetNativePtr<T>(env, obj, ptr);
    }

    ~ScopedJObject() {
        env->DeleteLocalRef(obj);
    }

    jobject getObject() {
        return obj;
    }
};

#endif // __TOOLS_H
