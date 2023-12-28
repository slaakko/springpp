// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

module p.execute;

import p.value;

namespace p {

Frame::Frame(int32_t size_) : size(size_), mem(static_cast<uint8_t*>(malloc(size * valueSize)))
{
    std::memset(mem, 0, size * valueSize);
}

Frame::~Frame()
{
    free(mem);
}

Object* Frame::GetObject(int32_t index)
{
    if (index >= 0 && index < size)
    {
        uint8_t* addr = mem + index * valueSize;
        if (HasNullContent(addr))
        {
            return nullptr;
        }
        return static_cast<Object*>(static_cast<void*>(addr));
    }
    else
    {
        throw std::runtime_error("invalid frame index");
    }
}

void Frame::SetObject(int32_t index, Object* object, ExecutionContext* context)
{
    if (index >= 0 && index < size)
    {
        uint8_t* addr = mem + index * valueSize;
        void* ptr = static_cast<void*>(addr);
        switch (object->Kind())
        {
            case ObjectKind::valueObject:
            {
                Value* value = static_cast<Value*>(object);
                switch (value->Kind())
                {
                    case ValueKind::booleanValue:
                    {
                        BooleanValue* booleanValue = static_cast<BooleanValue*>(value);
                        new (ptr)BooleanValue(*booleanValue);
                        break;
                    }
                    case ValueKind::integerValue:
                    {
                        IntegerValue* integerValue = static_cast<IntegerValue*>(value);
                        new (ptr)IntegerValue(*integerValue);
                        break;
                    }
                    case ValueKind::charValue:
                    {
                        CharValue* charValue = static_cast<CharValue*>(value);
                        new (ptr)CharValue(*charValue);
                        break;
                    }
                    case ValueKind::realValue:
                    {
                        RealValue* realValue = static_cast<RealValue*>(value);
                        new (ptr)RealValue(*realValue);
                        break;
                    }
                    case ValueKind::stringValue:
                    {
                        StringValue* stringValue = static_cast<StringValue*>(value);
                        Heap* heap = context->GetHeap();
                        StringObjectPtr stringObjectPtr(heap->AllocateString(stringValue->GetStringValue()));
                        new (ptr)StringObjectPtr(stringObjectPtr);
                        break;
                    }
                    case ValueKind::genericPointerValue:
                    {
                        GenericPointerValue* pointerValue = static_cast<GenericPointerValue*>(value);
                        new (ptr)GenericPointerValue(*pointerValue);
                        break;
                    }
                }
                break;
            }
            case ObjectKind::heapObjectPtr:
            {
                HeapObjectPtr* heapObjectPtr = static_cast<HeapObjectPtr*>(object);
                new (ptr)HeapObjectPtr(*heapObjectPtr);
                break;
            }
            case ObjectKind::heapObject:
            {
                HeapObjectPtr heapObjectPtr(static_cast<HeapObject*>(object));
                new (ptr)HeapObjectPtr(heapObjectPtr);
                break;
            }
            case ObjectKind::arrayObjectPtr:
            {
                ArrayObjectPtr* arrayObjectPtr = static_cast<ArrayObjectPtr*>(object);
                new (ptr)ArrayObjectPtr(*arrayObjectPtr);
                break;
            }
            case ObjectKind::arrayObject:
            {
                ArrayObjectPtr arrayObjectPtr(static_cast<ArrayObject*>(object));
                new (ptr)ArrayObjectPtr(arrayObjectPtr);
                break;
            }
            case ObjectKind::stringObject:
            {
                StringObjectPtr stringObjectPtr(static_cast<StringObject*>(object));
                new (ptr)StringObjectPtr(stringObjectPtr);
                break;
            }
            case ObjectKind::stringObjectPtr:
            {
                StringObjectPtr* stringObjectPtr = static_cast<StringObjectPtr*>(object);
                new (ptr)StringObjectPtr(*stringObjectPtr);
                break;
            }
        }
    }
    else
    {
        throw std::runtime_error("invalid frame index");
    }
}

Stack::Stack()
{
}

void Stack::Push(Object* value)
{
    values.push_back(std::unique_ptr<Object>(value));
}

void Stack::Dup()
{
    if (values.empty())
    {
        throw std::runtime_error("stack is empty");
    }
    values.push_back(std::unique_ptr<Object>(values.back()->Clone()));
}

std::unique_ptr<Object> Stack::Pop()
{
    if (values.empty())
    {
        throw std::runtime_error("stack is empty");
    }
    std::unique_ptr<Object> value = std::move(values.back());
    values.pop_back();
    return value;
}

ExecutionContext::ExecutionContext() : currentSubroutine(nullptr), next(nullptr), heap(nullptr), globalVariableMap(nullptr)
{
}

Frame* ExecutionContext::CurrentFrame()
{
    if (frames.empty())
    {
        throw std::runtime_error("current_frame: frames empty");
    }
    return frames.back().get();
}

void ExecutionContext::PushFrame(Frame* frame)
{
    frames.push_back(std::unique_ptr<Frame>(frame));
}

void ExecutionContext::PopFrame()
{
    if (frames.empty())
    {
        throw std::runtime_error("pop_frame: frames empty");
    }
    frames.pop_back();
}

void ExecutionContext::PushSubroutine(Subroutine* subroutine)
{
    subroutineStack.push(currentSubroutine);
    currentSubroutine = subroutine;
}

void ExecutionContext::PopSubroutine()
{
    currentSubroutine = subroutineStack.top();
    subroutineStack.pop();
}

void ExecutionContext::PushArgumentCount(int32_t argumentCount_)
{
    argumentCountStack.push(argumentCount);
    argumentCount = argumentCount_;
}

void ExecutionContext::PopArgumentCount()
{
    argumentCount = argumentCountStack.top();
    argumentCountStack.pop();
}

} // namespace p
