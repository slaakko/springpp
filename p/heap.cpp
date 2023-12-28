// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

module p.heap;

import p.type;

namespace p {

Heap::Heap() : Heap(defaultHeapSize)
{
}

Heap::~Heap()
{
    free(mem);
}

Heap::Heap(int32_t size_) : size(size_), mem(static_cast<uint8_t*>(malloc(size))), current(mem), collection(0)
{
    std::memset(current, 0, size);
}

void Heap::Collect()
{
}

HeapObject* Heap::Allocate(ObjectType* objectType, ExecutionContext* context)
{
    int32_t fieldCount = objectType->FieldCount();
    int32_t allocationSize = HeapObjectSize(fieldCount);
    int32_t currentSize = current - mem;
    if (currentSize + allocationSize > size)
    {
        Collect();
    }
    if (currentSize + allocationSize > size)
    {
        throw std::runtime_error("heap: could not grow");
    }
    HeapObject* heapObject = new (static_cast<void*>(current))HeapObject(objectType, fieldCount, collection);
    heapObject->Init(context);
    current += allocationSize;
    return heapObject;
}

ArrayObject* Heap::AllocateArray(ArrayType* arrayType, int32_t arraySize, ExecutionContext* context)
{
    int32_t allocationSize = sizeof(ArrayObject) + arraySize * valueSize;
    int32_t currentSize = current - mem;
    if (currentSize + allocationSize > size)
    {
        Collect();
    }
    if (currentSize + allocationSize > size)
    {
        throw std::runtime_error("heap: could not grow");
    }
    ArrayObject* arrayObject = new (static_cast<void*>(current))ArrayObject(arrayType, arraySize, collection);
    arrayObject->Init(context);
    current += allocationSize;
    return arrayObject;
}

StringObject* Heap::AllocateString(const std::string& str)
{
    int32_t allocationSize = sizeof(StringObject) + str.length() + 1;
    int32_t currentSize = current - mem;
    if (currentSize + allocationSize > size)
    {
        Collect();
    }
    if (currentSize + allocationSize > size)
    {
        throw std::runtime_error("heap: could not grow");
    }
    StringObject* stringObject = new (static_cast<void*>(current))StringObject(str, collection);
    current += allocationSize;
    return stringObject;
}

int32_t HeapObjectSize(int32_t fieldCount)
{
    return sizeof(HeapObject) + fieldCount * valueSize;
}

} // namespace p
