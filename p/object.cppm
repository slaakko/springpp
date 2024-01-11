// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

export module p.object;

import std.core;

export namespace p {

class ObjectType;
class ArrayType;
class StringObject;
class ExecutionContext;
class Frame;

enum class ObjectKind : int32_t
{
    ptrObject, valueObject, heapObject, heapObjectPtr, arrayObject, arrayObjectPtr, stringObject, stringObjectPtr, nilObject
};

bool HasNullContent(uint8_t* object);

class Object
{
public:
    Object(ObjectKind kind_);
    ObjectKind Kind() const { return kind; }
    virtual ~Object();
    virtual Object* GetObject() { return this; }
    virtual void SetObject(Object* object_) {}
    virtual std::string KindStr() const { return "object"; }
    virtual Object* Clone() const = 0;
    bool IsPtrObject() const { return kind == ObjectKind::ptrObject; }
    bool IsValueObject() const { return kind == ObjectKind::valueObject; }
    bool IsHeapObject() const { return kind == ObjectKind::heapObject; }
    bool IsArrayObject() const { return kind == ObjectKind::arrayObject; }
    bool IsStringObject() const { return kind == ObjectKind::stringObject; }
    virtual bool IsNilObject() const { return kind == ObjectKind::nilObject; }
    uint8_t* Ptr() const { return static_cast<uint8_t*>(static_cast<void*>(const_cast<Object*>(this))); }
    virtual bool ToBoolean() const;
    virtual int32_t ToInteger() const;
    virtual char ToChar() const;
    virtual float ToReal() const;
    virtual std::string ToString() const;
    virtual StringObject* ToStringObject(ExecutionContext* context);
    void SetOwner(Frame* owner_) { owner = owner_; }
    Frame* Owner() const { return owner; }
    void SetOwnerIndex(int32_t ownerIndex_) { ownerIndex = ownerIndex_; }
    int32_t OwnerIndex() const { return ownerIndex; }
private:
    ObjectKind kind;
    Frame* owner;
    int32_t ownerIndex;
};

class PtrObject : public Object
{
public:
    PtrObject(Object* value_);
    Object* GetObject() override;
    Object* Clone() const override;
    void SetObject(Object* object_) override;
    std::string KindStr() const override { return "ptr_object"; }
private:
    Object* object;
};

const int32_t valueSize = 48;

class HeapObject : public Object
{
public:
    HeapObject(ObjectType* type_, int32_t fieldCount_, int32_t collection_);
    Object* Clone() const override;
    std::string KindStr() const override { return "heap_object"; }
    int32_t Collection() const { return collection; }
    void SetCollection(int32_t collection_) { collection = collection_; }
    ObjectType* GetType() const { return type; }
    void SetType(ObjectType* type_) { type = type_; }
    void Init(ExecutionContext* context);
    Object* GetField(int32_t fieldIndex) const;
    void SetField(int32_t fieldIndex, Object* object, ExecutionContext* context);
private:
    ObjectType* type;
    int32_t fieldCount;
    int32_t collection;
};

class HeapObjectPtr : public Object
{
public:
    HeapObjectPtr(HeapObject* heapObject_);
    Object* Clone() const override;
    Object* GetObject() override;
    void SetObject(Object* value_) override;
    std::string KindStr() const override { return "heap_object_ptr"; }
private:
    HeapObject* heapObject;
};

class ArrayObject : public Object
{
public:
    ArrayObject(ArrayType* arrayType_, int32_t length_, int32_t collection_);
    Object* Clone() const override;
    int32_t Collection() const { return collection; }
    void SetCollection(int32_t collection_) { collection = collection_; }
    std::string KindStr() const override { return "array_object"; }
    void Init(ExecutionContext* context);
    Object* GetElement(int32_t elementIndex) const;
    void SetElement(int32_t elementIndex, Object* element, ExecutionContext* context);
    int32_t Length() const { return length; }
private:
    ArrayType* arrayType;
    int32_t length;
    int32_t collection;
};

class ArrayObjectPtr : public Object
{
public:
    ArrayObjectPtr(ArrayObject* arrayObject_);
    Object* Clone() const override;
    Object* GetObject() override;
    void SetObject(Object* value_) override;
    std::string KindStr() const override { return "array_object_ptr"; }
private:
    ArrayObject* arrayObject;
};

class StringObject : public Object
{
public:
    StringObject(const std::string& value_, int32_t collection_);
    Object* Clone() const override;
    int32_t Length() const { return length; }
    const char* Value() const { return value; }
    std::string KindStr() const override { return "string_object"; }
    int32_t Collection() const { return collection; }
    StringObject* ToStringObject(ExecutionContext* context) override;
private:
    int32_t length;
    const char* value;
    int32_t collection;
};

class StringObjectPtr : public Object
{
public:
    StringObjectPtr(StringObject* stringObject_);
    Object* Clone() const override;
    std::string KindStr() const override { return "string_object_ptr"; }
    Object* GetObject() override;
    void SetObject(Object* value_) override;
    StringObject* ToStringObject(ExecutionContext* context) override;
private:
    StringObject* stringObject;
};

class NilObject : public Object
{
public:
    NilObject();
    Object* Clone() const override;
    std::string KindStr() const override { return "nil_object"; }
    static Object* Ptr();
};

} // namespace p
