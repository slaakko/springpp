// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

module p.parsing_context;

import p.code;
import p.execute;

namespace p {

ParsingContext::ParsingContext() : mod(new Module()), modulePart(nullptr), flags(Flags::none), currentValue(nullptr), emitter(nullptr)
{
}

void ParsingContext::SetModulePart(ModulePart* modulePart_)
{
    modulePart = modulePart_;
}

void ParsingContext::SetTypeName(const std::string& typeName_)
{
    typeName = typeName_;
}

void ParsingContext::SetConstantName(const std::string& constantName_)
{
    constantName = constantName_;
}

void ParsingContext::SetFieldName(const std::string& fieldName_)
{
    fieldName = fieldName_;
}

void ParsingContext::PushFlags(Flags flags_)
{
    flagStack.push(flags);
    flags = flags_;
}

void ParsingContext::PopFlags()
{
    flags = flagStack.top();
    flagStack.pop();
}

void ParsingContext::PushCurrentValue(Value* currentValue_)
{
    valueStack.push(currentValue);
    currentValue = currentValue_;
}

void ParsingContext::PopCurrentValue()
{
    currentValue = valueStack.top();
    valueStack.pop();
}

} // namespace p
