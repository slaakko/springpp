// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

export module p.execute;

import p.object;
import p.variable;
import p.code;
import p.context;
import p.subroutine;
import p.instruction;
import p.heap;
import std.core;

export namespace p {

namespace execute {}

class ExecutionContext;

class Frame
{
public:
    Frame(int32_t size_);
    ~Frame();
    Object* GetObject(int32_t index);
    void SetObject(int32_t index, Object* object, ExecutionContext* context);
private:
    int32_t size;
    uint8_t* mem;
};

class Stack
{
public:
    Stack();
    void Push(Object* value);
    void Dup();
    std::unique_ptr<Object> Pop();
private:
    std::vector<std::unique_ptr<Object>> values;
};

class ExecutionContext : public Context
{
public:
    ExecutionContext();
    Stack* GetStack() { return &stack; }
    Frame* CurrentFrame();
    Heap* GetHeap() const { return heap; }
    void SetHeap(Heap* heap_) { heap = heap_; }
    void PushFrame(Frame* frame);
    void PopFrame();
    void PushSubroutine(Subroutine* subroutine);
    void PopSubroutine();
    Subroutine* CurrentSubroutine() const { return currentSubroutine; }
    int32_t ArgumentCount() const { return argumentCount; }
    void PushArgumentCount(int32_t argumentCount_);
    void PopArgumentCount();
    void SetNext(Instruction* next_) { next = next_; }
    Instruction* Next() const { return next; }
    void SetGlobalVariableMap(GlobalVariableMap* globalVariableMap_) { globalVariableMap = globalVariableMap_; }
    GlobalVariableMap* GetGlobalVariableMap() const { return globalVariableMap; }
private:
    Stack stack;
    std::vector<std::unique_ptr<Frame>> frames;
    Subroutine* currentSubroutine;
    std::stack<Subroutine*> subroutineStack;
    Instruction* next;
    Heap* heap;
    int32_t argumentCount;
    std::stack<int32_t> argumentCountStack;
    GlobalVariableMap* globalVariableMap;
};

} // namespace p
