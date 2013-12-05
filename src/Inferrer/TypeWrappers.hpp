#ifndef __TYPE_WRAPPERS_HPP
#define __TYPE_WRAPPERS_HPP

#include <map>
#include <memory>
#include <sstream>
#include <string>

class BaseType {
protected:
    JNIEnv *jvEnv;

public:
    BaseType(JNIEnv *jvEnv): jvEnv(jvEnv) {}
    virtual ~BaseType() {}

    virtual std::string toString() = 0;

    virtual void append(const std::string& name, std::shared_ptr<BaseType> child) = 0;

    virtual jobject getJObject(void) = 0;
};


class UnknownType: public BaseType {
protected:
	std::string typeName;

public:
	UnknownType(JNIEnv *jvEnv): BaseType(jvEnv), typeName("uninitialized")
	{
	}

    UnknownType(JNIEnv *jvEnv, std::shared_ptr<raw::root::Node> node):
    	BaseType(jvEnv)
    {
    	typeName = node->getTypeName();
    }

    virtual ~UnknownType() {}

    std::string toString()
    {
        return std::string("UnknownType(") + typeName + ")";
    }

    void append(const std::string& name, std::shared_ptr<BaseType> child)
    {
    }

    jobject getJObject(void)
    {
    	jobject   type        = jvEnv->NewStringUTF(typeName.c_str());
        jclass    klass       = jvEnv->FindClass("raw/root/types/UnknownType");
        jmethodID constructor = jvEnv->GetMethodID(klass, "<init>", "(Ljava/lang/String;)V");
        return jvEnv->NewObject(klass, constructor, type);
    }
};

class IntegerType: public BaseType {
public:
    IntegerType(JNIEnv *jvEnv): BaseType(jvEnv)
    {
    }

    virtual ~IntegerType() {}

    std::string toString()
    {
        return "IntegerType()";
    }

    void append(const std::string& name, std::shared_ptr<BaseType> child)
    {
    }

    jobject getJObject(void)
    {
        jclass    klass       = jvEnv->FindClass("raw/root/types/IntegerType");
        jmethodID constructor = jvEnv->GetMethodID(klass, "<init>", "()V");
        return jvEnv->NewObject(klass, constructor);
    }
};


class DoubleType: public BaseType {
public:
    DoubleType(JNIEnv *jvEnv): BaseType(jvEnv)
    {
    }

    virtual ~DoubleType() {}

    std::string toString()
    {
        return "DoubleType()";
    }

    void append(const std::string& name, std::shared_ptr<BaseType> child)
    {
    }

    jobject getJObject(void)
    {
        jclass    klass       = jvEnv->FindClass("raw/root/types/DoubleType");
        jmethodID constructor = jvEnv->GetMethodID(klass, "<init>", "()V");
        return jvEnv->NewObject(klass, constructor);
    }
};


class StringType: public BaseType {
public:
    StringType(JNIEnv *jvEnv): BaseType(jvEnv)
    {
    }

    virtual ~StringType() {}

    std::string toString()
    {
        return "StringType()";
    }

    void append(const std::string& name, std::shared_ptr<BaseType> child)
    {
    }

    jobject getJObject(void)
    {
        jclass    klass       = jvEnv->FindClass("raw/root/types/StringType");
        jmethodID constructor = jvEnv->GetMethodID(klass, "<init>", "()V");
        return jvEnv->NewObject(klass, constructor);
    }
};


class CollectionType: public BaseType {
protected:
    std::shared_ptr<BaseType> containedType;

public:
    CollectionType(JNIEnv *jvEnv): BaseType(jvEnv), containedType(new UnknownType(jvEnv))
    {
    }

    virtual ~CollectionType() {}

    std::string toString()
    {
        std::ostringstream str;
        if (containedType)
            str << "CollectionType(" << containedType->toString() << ")";
        else
            str << "CollectionType(?)";
        return str.str();
    }

    void append(const std::string& name, std::shared_ptr<BaseType> child)
    {
        containedType = child;
    }

    jobject getJObject(void)
    {
        if (!containedType)
            return NULL;

        jobject   contained   = containedType->getJObject();
        jclass    klass       = jvEnv->FindClass("raw/root/types/CollectionType");
        jmethodID constructor = jvEnv->GetMethodID(klass, "<init>", "(Lraw/root/types/Type;)V");
        jobject   collection  = jvEnv->NewObject(klass, constructor, contained);
        return collection;
    }
};


class DictionaryType: public BaseType {
protected:
    typedef std::map<std::string, std::shared_ptr<BaseType> > AttrMap;
    AttrMap attrs;

public:
    DictionaryType(JNIEnv *jvEnv): BaseType(jvEnv)
    {
    }

    virtual ~DictionaryType() {}

    std::string toString()
    {
        std::ostringstream str;
        str << "DictionaryType{" << std::endl;
        for (AttrMap::const_iterator i = attrs.begin(); i != attrs.end(); ++i) {
            str << "\t" << i->first << ": ";
            if (i->second)
                str << i->second->toString();
            else
                str << "NULL";
            str << std::endl;
        }
        str << "}";
        return str.str();
    }

    void append(const std::string& name, std::shared_ptr<BaseType> child)
    {
        attrs.insert(std::make_pair(name, child));
    }

    jobject getJObject(void)
    {
        jclass    klass       = jvEnv->FindClass("raw/root/types/RecordType");
        jmethodID constructor = jvEnv->GetMethodID(klass, "<init>", "()V");
        jobject   record      = jvEnv->NewObject(klass, constructor);

        jmethodID append = jvEnv->GetMethodID(klass, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");

        for (AttrMap::const_iterator i = attrs.begin(); i != attrs.end(); ++i) {
            if (i->second) {
                jobject key   = jvEnv->NewStringUTF(i->first.c_str());
                jobject value = i->second->getJObject();
                jvEnv->CallObjectMethod(record, append, key, value);
            }
        }

        return record;
    }
};


#endif // __TYPE_WRAPPERS_HPP
