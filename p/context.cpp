// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

module p.context;

import p.instruction;
import p.subroutine;

namespace p {

Context::Context() : moduleMap(nullptr), block(nullptr)
{
}

void Context::PushBlock(Block* block_)
{
    blockStack.push(block);
    block = block_;
}

void Context::PopBlock()
{
    block = blockStack.top();
    blockStack.pop();
}

} // namespace p
