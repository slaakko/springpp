// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

export module p.context;

import p.mod;
import p.block;
import std.core;

export namespace p {

class CallVirtualInstruction;

class Context
{
public:
    Context();
    ModuleMap* GetModuleMap() const { return moduleMap; }
    void SetModuleMap(ModuleMap* moduleMap_) { moduleMap = moduleMap_; }
    Block* GetBlock() const { return block; }
    void PushBlock(Block* block_);
    void PopBlock();
private:
    ModuleMap* moduleMap;
    Block* block;
    std::stack<Block*> blockStack;
};

} // namespace p
