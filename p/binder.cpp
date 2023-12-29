// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

module p.binder;

import p.visitor;
import p.ast;
import p.parsing_context;
import p.subroutine;
import p.code;
import p.type;
import p.constant;
import p.instruction;
import p.fnrep;
import p.variable;
import p.stdfn;
import p.rt;

namespace p {

TypeKind GetCommonType(TypeKind left, TypeKind right)
{
    if (left == TypeKind::booleanType && right == TypeKind::booleanType)
    {
        return TypeKind::booleanType;
    }
    else if (left == TypeKind::integerType)
    {
        if (right == TypeKind::integerType)
        {
            return TypeKind::integerType;
        }
        else if (right == TypeKind::realType)
        {
            return TypeKind::realType;
        }
    }
    else if (left == TypeKind::charType)
    {
        if (right == TypeKind::charType)
        {
            return TypeKind::charType;
        }
        else if (right == TypeKind::stringType)
        {
            return TypeKind::stringType;
        }
    }
    else if (left == TypeKind::realType)
    {
        if (right == TypeKind::realType)
        {
            return TypeKind::realType;
        }
        else if (right == TypeKind::integerType)
        {
            return TypeKind::realType;
        }
    }
    else if (left == TypeKind::stringType)
    {
        if (right == TypeKind::charType)
        {
            return TypeKind::stringType;
        }
        else if (right == TypeKind::stringType)
        {
            return TypeKind::stringType;
        }
    }
    else if (left == TypeKind::pointerType && right == TypeKind::pointerType)
    {
        return TypeKind::pointerType;
    }
    return TypeKind::none;
}

std::unique_ptr<BoundExpressionNode> BindExpression(ParsingContext* context, Subroutine* subroutine, soul::lexer::LexerBase<char>& lexer, Node* node, 
    const std::vector<std::unique_ptr<BoundExpressionNode>>& boundArguments);

std::unique_ptr<BoundExpressionNode> BindExpression(ParsingContext* context, Subroutine* subroutine, soul::lexer::LexerBase<char>& lexer, Node* node);

BoundExpressionNode* MakeBoundMethodCall(Subroutine* method, std::vector<std::unique_ptr<BoundExpressionNode>>& boundArguments, soul::lexer::LexerBase<char>& lexer, int64_t pos)
{
    std::unique_ptr<BoundMethodCallNode> boundMethodCall(new BoundMethodCallNode(method, boundArguments, pos));
    if (method->Parameters().size() != boundArguments.size())
    {
        ThrowError("error: method '" + method->FullName() + "' takes " + std::to_string(method->Parameters().size()) + " parameters, " + std::to_string(boundArguments.size()) +
            "arguments passed", lexer, pos);
    }
    int n = boundArguments.size();
    for (int i = 0; i < n; ++i)
    {
        const Parameter& parameter = method->Parameters()[i];
        if (parameter.GetType() == boundArguments[i]->GetType())
        {
            boundMethodCall->AddArgument(boundArguments[i].release());
        }
        else
        {
            Function* conversionFunction = GetConversionFunction(parameter.GetType(), boundArguments[i]->GetType(), lexer, pos, false);
            if (!conversionFunction)
            {
                ThrowError("error: no conversion for argument " + std::to_string(i) + " found for method call '" + method->FullName() + "'", lexer, pos);
            }
            boundMethodCall->AddArgument(new BoundConversionNode(conversionFunction, boundArguments[i]->Clone(), pos));
        }
    }
    if (!boundMethodCall->Arguments().empty())
    {
        BoundExpressionNode* subjectArg = boundMethodCall->Arguments()[0].get();
        if ((subjectArg->GetArgumentFlags() & ArgumentFlags::thisOrBaseArgument) == ArgumentFlags::none)
        {
            Type* type = subjectArg->GetType();
            if (type->IsObjectType())
            {
                ObjectType* objectType = static_cast<ObjectType*>(type);
                if (objectType->IsVirtual())
                {
                    boundMethodCall->SetArgumentFlags(boundMethodCall->GetArgumentFlags() | ArgumentFlags::virtualCall);
                }
            }
        }
    }
    return boundMethodCall.release();
}

BoundNode::BoundNode(BoundNodeKind kind_, int64_t pos_) : kind(kind_), pos(pos_)
{
}

BoundExpressionNode::BoundExpressionNode(BoundNodeKind kind_, int64_t pos_, Type* type_) : BoundNode(kind_, pos_), type(type_), argumentFlags(ArgumentFlags::none)
{
}

void BoundExpressionNode::Load(Emitter* emitter)
{
    throw std::runtime_error("cannot load this kind of bound expression");
}

void BoundExpressionNode::Store(Emitter* emitter)
{
    throw std::runtime_error("cannot store to this kind of bound expression");
}

Type* BinaryExprFunctionResultType(Function* operatorFunction, BoundExpressionNode* left, BoundExpressionNode* right)
{
    std::vector<Type*> argumentTypes;
    argumentTypes.push_back(left->GetType());
    argumentTypes.push_back(right->GetType());
    return operatorFunction->ResultType(argumentTypes);
}

BoundBinaryExpressionNode::BoundBinaryExpressionNode(Function* operatorFunction_, BoundExpressionNode* left_, BoundExpressionNode* right_, int64_t pos_) :
    BoundExpressionNode(BoundNodeKind::boundBinaryExprNode, pos_, BinaryExprFunctionResultType(operatorFunction_, left_, right_)), 
        operatorFunction(operatorFunction_), left(left_), right(right_)
{
}

void BoundBinaryExpressionNode::Load(Emitter* emitter) 
{
    left->Load(emitter);
    right->Load(emitter);
    operatorFunction->GenerateCode(emitter, Pos());
}

void BoundBinaryExpressionNode::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundExpressionNode* BoundBinaryExpressionNode::Clone() const
{
    return new BoundBinaryExpressionNode(operatorFunction, left->Clone(), right->Clone(), Pos());
}

Type* UnaryExprFunctionResultType(Function* operatorFunction, BoundExpressionNode* operand)
{
    std::vector<Type*> argumentTypes;
    argumentTypes.push_back(operand->GetType());
    return operatorFunction->ResultType(argumentTypes);
}

BoundUnaryExpressionNode::BoundUnaryExpressionNode(Function* operatorFunction_, BoundExpressionNode* operand_, int64_t pos_) :
    BoundExpressionNode(BoundNodeKind::boundUnaryExprNode, pos_, UnaryExprFunctionResultType(operatorFunction_, operand_)), operatorFunction(operatorFunction_), operand(operand_)
{
}

void BoundUnaryExpressionNode::Load(Emitter* emitter)
{
    operand->Load(emitter);
    operatorFunction->GenerateCode(emitter, Pos());
}

void BoundUnaryExpressionNode::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundExpressionNode* BoundUnaryExpressionNode::Clone() const
{
    return new BoundUnaryExpressionNode(operatorFunction, operand->Clone(), Pos());
}

BoundLiteralNode::BoundLiteralNode(int64_t pos_, Type* type_, Value* value_) : BoundExpressionNode(BoundNodeKind::boundLiteralNode, pos_, type_), value(value_)
{
}

void BoundLiteralNode::Load(Emitter* emitter)
{
    PushValueInstruction* pushValueInstruction = new PushValueInstruction();
    pushValueInstruction->SetValue(value.release());
    emitter->Emit(pushValueInstruction);
}

void BoundLiteralNode::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundExpressionNode* BoundLiteralNode::Clone() const
{
    return new BoundLiteralNode(Pos(), GetType(), static_cast<Value*>(value->Clone()));
}

BoundParameterNode::BoundParameterNode(int64_t pos_, Parameter* parameter_) :
    BoundExpressionNode(BoundNodeKind::boundParameterNode, pos_, parameter_->GetType()), parameter(parameter_)
{
}

BoundParameterNode::BoundParameterNode(int64_t pos_, Parameter* parameter_, Type* type_) :
    BoundExpressionNode(BoundNodeKind::boundParameterNode, pos_, type_), parameter(parameter_)
{
}

void BoundParameterNode::Load(Emitter* emitter)
{
    LoadLocalInstruction* loadLocalInstruction = new LoadLocalInstruction();
    loadLocalInstruction->SetLocalIndex(parameter->Index());
    emitter->Emit(loadLocalInstruction);
}

void BoundParameterNode::Store(Emitter* emitter)
{
    StoreLocalInstruction* storeLocalInstruction = new StoreLocalInstruction();
    storeLocalInstruction->SetLocalIndex(parameter->Index());
    emitter->Emit(storeLocalInstruction);
}

void BoundParameterNode::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundExpressionNode* BoundParameterNode::Clone() const
{
    return new BoundParameterNode(Pos(), parameter);
}

BoundVariableNode::BoundVariableNode(int64_t pos_, Variable* variable_) : BoundExpressionNode(BoundNodeKind::boundVariableNode, pos_, variable_->GetType()), variable(variable_)
{
}

void BoundVariableNode::Load(Emitter* emitter)
{
    if (variable->IsLocal() || variable->IsParameter())
    {
        LoadLocalInstruction* loadLocalInstruction = new LoadLocalInstruction();
        loadLocalInstruction->SetLocalIndex(variable->Index());
        emitter->Emit(loadLocalInstruction);
    }
    else if (variable->IsGlobal())
    {
        LoadGlobalInstruction* loadGlobalInstruction = new LoadGlobalInstruction();
        loadGlobalInstruction->SetGlobalIndex(variable->Index());
        emitter->Emit(loadGlobalInstruction);
    }
}

void BoundVariableNode::Store(Emitter* emitter)
{
    if (variable->IsLocal() || variable->IsParameter())
    {
        StoreLocalInstruction* storeLocalInstruction = new StoreLocalInstruction();
        storeLocalInstruction->SetLocalIndex(variable->Index());
        emitter->Emit(storeLocalInstruction);
    }
    else if (variable->IsGlobal())
    {
        StoreGlobalInstruction* storeGlobalInstruction = new StoreGlobalInstruction();
        storeGlobalInstruction->SetGlobalIndex(variable->Index());
        emitter->Emit(storeGlobalInstruction);
    }
}

void BoundVariableNode::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundFunctionResultNode::BoundFunctionResultNode(int64_t pos_, Variable* variable_) : 
    BoundExpressionNode(BoundNodeKind::boundFunctionResultNode, pos_, variable_->GetType()), variable(variable_)
{
}

void BoundFunctionResultNode::Load(Emitter* emitter)
{
    LoadResultVarInstruction* loadResultVarInstruction = new LoadResultVarInstruction();
    loadResultVarInstruction->SetResultVarIndex(variable->Index());
    emitter->Emit(loadResultVarInstruction);
}

void BoundFunctionResultNode::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundExpressionNode* BoundFunctionResultNode::Clone() const
{
    return new BoundFunctionResultNode(Pos(), variable);
}

BoundExpressionNode* BoundVariableNode::Clone() const
{
    return new BoundVariableNode(Pos(), variable);
}

BoundConstantNode::BoundConstantNode(int64_t pos_, Type* type_, Constant* constant_) : BoundExpressionNode(BoundNodeKind::boundConstantNode, pos_, type_), constant(constant_)
{
}

void BoundConstantNode::Load(Emitter* emitter)
{
    PushValueInstruction* pushValueInstruction = new PushValueInstruction();
    pushValueInstruction->SetValue(new PtrValue(constant->GetValue()));
    emitter->Emit(pushValueInstruction);
}

void BoundConstantNode::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundExpressionNode* BoundConstantNode::Clone() const
{
    return new BoundConstantNode(Pos(), GetType(), constant);
}

Type* ConversionFunctionResultType(Function* conversionFunction, BoundExpressionNode* operand)
{
    std::vector<Type*> argumentTypes;
    argumentTypes.push_back(operand->GetType());
    return conversionFunction->ResultType(argumentTypes);
}

BoundConversionNode::BoundConversionNode(Function* conversionFunction_, BoundExpressionNode* operand_, int64_t pos_) :
    BoundExpressionNode(BoundNodeKind::boundConversionNode, pos_, ConversionFunctionResultType(conversionFunction_, operand_)), 
    conversionFunction(conversionFunction_), operand(operand_)
{
}

void BoundConversionNode::Load(Emitter* emitter)
{
    operand->Load(emitter);
    conversionFunction->GenerateCode(emitter, Pos());
}

void BoundConversionNode::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundExpressionNode* BoundConversionNode::Clone() const
{
    return new BoundConversionNode(conversionFunction, operand->Clone(), Pos());
}

BoundValueConversionNode::BoundValueConversionNode(BoundExpressionNode* operand_, Type* type_, int64_t pos_) : 
    BoundExpressionNode(BoundNodeKind::boundValueConversionNode, pos_, type_), operand(operand_)
{
}

void BoundValueConversionNode::Load(Emitter* emitter)
{
    operand->Load(emitter);
}

void BoundValueConversionNode::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundExpressionNode* BoundValueConversionNode::Clone() const
{
    return new BoundValueConversionNode(operand->Clone(), GetType(), Pos());
}

BoundProcedureNode::BoundProcedureNode(Procedure* procedure_, int64_t pos_) : BoundExpressionNode(BoundNodeKind::boundProcedureNode, pos_, nullptr), procedure(procedure_)
{
}

void BoundProcedureNode::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundExpressionNode* BoundProcedureNode::Clone() const
{
    return new BoundProcedureNode(procedure, Pos());
}

Type* BoundFunctionResultType(Function* function, const std::vector<std::unique_ptr<BoundExpressionNode>>& boundArguments)
{
    std::vector<Type*> argumentTypes;
    for (const auto& arg : boundArguments)
    {
        argumentTypes.push_back(arg->GetType());
    }
    return function->ResultType(argumentTypes);
}

BoundFunctionNode::BoundFunctionNode(Function* function_, const std::vector<std::unique_ptr<BoundExpressionNode>>& boundArguments, int64_t pos_) :
    BoundExpressionNode(BoundNodeKind::boundFunctionNode, pos_, BoundFunctionResultType(function_, boundArguments)), function(function_)
{
}

void BoundFunctionNode::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundExpressionNode* BoundFunctionNode::Clone() const
{
    std::vector<std::unique_ptr<BoundExpressionNode>> boundArguments;
    return new BoundFunctionNode(function, boundArguments, Pos());
}

BoundMethodNode::BoundMethodNode(BoundExpressionNode* subject_, Subroutine* method_, int64_t pos_) : 
    BoundExpressionNode(BoundNodeKind::boundMethodNode, pos_, nullptr), subject(subject_), method(method_)
{
}

void BoundMethodNode::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundExpressionNode* BoundMethodNode::Clone() const
{
    return new BoundMethodNode(subject->Clone(), method, Pos());
}

Type* BoundFunctionCallResultType(Function* function, const std::vector<std::unique_ptr<BoundExpressionNode>>& boundArguments)
{
    std::vector<Type*> argumentTypes;
    for (const auto& argument : boundArguments)
    {
        argumentTypes.push_back(argument->GetType());
    }
    return function->ResultType(argumentTypes);
}

BoundFunctionCallNode::BoundFunctionCallNode(Function* function_, const std::vector<std::unique_ptr<BoundExpressionNode>>& boundArguments, int64_t pos_) : 
    BoundExpressionNode(BoundNodeKind::boundFunctionCallNode, pos_, BoundFunctionCallResultType(function_, boundArguments)), function(function_)
{
}

void BoundFunctionCallNode::AddArgument(BoundExpressionNode* argument)
{
    arguments.push_back(std::unique_ptr<BoundExpressionNode>(argument));
}

void BoundFunctionCallNode::Load(Emitter* emitter)
{
    for (const auto& argument : arguments)
    {
        argument->Load(emitter);
    }
    if (function->IsExternal())
    {
        ExternalSubroutine* subroutine = GetExternalSubroutine(function->ExternalSubroutineName());
        CallExternalInstruction* callExternalInstruction = new CallExternalInstruction();
        if (subroutine->Id() == -1)
        {
            ThrowError("subroutine ID not set in external function call '" + subroutine->FullName() + "'", emitter->Lexer(), Pos());
        }
        callExternalInstruction->SetId(subroutine->Id());
        emitter->Emit(callExternalInstruction);
    }
    else if (function->IsRegularFunction())
    {
        CallFunctionInstruction* callFunctionInstruction = new CallFunctionInstruction();
        if (function->ModuleId() == -1)
        {
            ThrowError("module ID not set in function call '" + function->FullName() + "'", emitter->Lexer(), Pos());
        }
        callFunctionInstruction->SetModuleId(function->ModuleId());
        if (function->IsDeclaration())
        {
            if (function->ImplementationId() == -1)
            {
                ThrowError("implementation ID not set in function call '" + function->FullName() + "'", emitter->Lexer(), Pos());
            }
            callFunctionInstruction->SetSubroutineId(function->ImplementationId());
        }
        else
        {
            if (function->Id() == -1)
            {
                ThrowError("implementation ID not set in function call '" + function->FullName() + "'", emitter->Lexer(), Pos());
            }
            callFunctionInstruction->SetSubroutineId(function->Id());
        }
        emitter->Emit(callFunctionInstruction);
    }
    else if (function->IsStandardFunction())
    {
        StandardFunction* stdfn = static_cast<StandardFunction*>(function);
        if (stdfn->Id() == -1)
        {
            ThrowError("standard function ID not set in function call '" + function->FullName() + "'", emitter->Lexer(), Pos());
        }
        CallStdFnInstruction* callStdFnInstruction = new CallStdFnInstruction();
        callStdFnInstruction->SetStdFnId(stdfn->Id());
        callStdFnInstruction->SetArgumentCount(arguments.size());
        emitter->Emit(callStdFnInstruction);
    }
}

void BoundFunctionCallNode::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundExpressionNode* BoundFunctionCallNode::Clone() const
{
    BoundFunctionCallNode* clone = new BoundFunctionCallNode(function, arguments, Pos());
    for (const auto& arg : arguments)
    {
        clone->AddArgument(arg->Clone());
    }
    return clone;
}

BoundConstructorCallNode::BoundConstructorCallNode(Constructor* constructor_, int64_t pos_) :
    BoundExpressionNode(BoundNodeKind::boundConstructorCallNode, pos_, nullptr), constructor(constructor_)
{
}

void BoundConstructorCallNode::AddArgument(BoundExpressionNode* argument)
{
    if (arguments.empty())
    {
        SetType(argument->GetType());
    }
    arguments.push_back(std::unique_ptr<BoundExpressionNode>(argument));
}

void BoundConstructorCallNode::Load(Emitter* emitter)
{
    for (const auto& argument : arguments)
    {
        argument->Load(emitter);
    }
    CallConstructorInstruction* callConstructorInstruction = new CallConstructorInstruction();
    callConstructorInstruction->SetModuleId(constructor->ModuleId());
    if (constructor->IsDeclaration())
    {
        callConstructorInstruction->SetSubroutineId(constructor->ImplementationId());
    }
    else
    {
        callConstructorInstruction->SetSubroutineId(constructor->Id());
    }
    emitter->Emit(callConstructorInstruction);
}

void BoundConstructorCallNode::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundExpressionNode* BoundConstructorCallNode::Clone() const
{
    BoundConstructorCallNode* clone = new BoundConstructorCallNode(constructor, Pos());
    for (const auto& arg : arguments)
    {
        clone->AddArgument(arg->Clone());
    }
    return clone;
}

Type* GetMethodType(Subroutine* method, const std::vector<std::unique_ptr<BoundExpressionNode>>& boundArguments)
{
    if (method->IsFunction())
    {
        Function* function = static_cast<Function*>(method);
        std::vector<Type*> argumentTypes;
        for (const auto& arg : boundArguments)
        {
            argumentTypes.push_back(arg->GetType());
        }
        return function->ResultType(argumentTypes);
    }
    else
    {
        return nullptr;
    }
}

BoundMethodCallNode::BoundMethodCallNode(Subroutine* method_, const std::vector<std::unique_ptr<BoundExpressionNode>>& boundArguments, int64_t pos_) : 
    BoundExpressionNode(BoundNodeKind::boundMethodCallNode, pos_, GetMethodType(method_, boundArguments)), method(method_)
{
}

void BoundMethodCallNode::AddArgument(BoundExpressionNode* argument)
{
    arguments.push_back(std::unique_ptr<BoundExpressionNode>(argument));
}

void BoundMethodCallNode::Load(Emitter* emitter)
{
    for (const auto& argument : arguments)
    {
        argument->Load(emitter);
    }
    if ((GetArgumentFlags() & ArgumentFlags::virtualCall) != ArgumentFlags::none)
    {
        if (!arguments.empty())
        {
            arguments.front()->Load(emitter);
            CallVirtualInstruction* instruction = new CallVirtualInstruction();
            instruction->SetMethod(method);
            emitter->Emit(instruction);
        }
        else
        {
            throw std::runtime_error("error: method '" + method->FullName() + "' arguments empty, cannot call virtual");
        }
    }
    else
    {
        if (method->IsExternal())
        {
            ExternalSubroutine* subroutine = GetExternalSubroutine(method->ExternalSubroutineName());
            CallExternalInstruction* callExternalInstruction = new CallExternalInstruction();
            callExternalInstruction->SetId(subroutine->Id());
            emitter->Emit(callExternalInstruction);
        }
        else if (method->IsProcedure())
        {
            CallProcedureInstruction* callProcedureInstruction = new CallProcedureInstruction();
            callProcedureInstruction->SetModuleId(method->ModuleId());
            if (method->IsDeclaration())
            {
                callProcedureInstruction->SetSubroutineId(method->ImplementationId());
            }
            else
            {
                callProcedureInstruction->SetSubroutineId(method->Id());
            }
            emitter->Emit(callProcedureInstruction);
        }
        else if (method->IsFunction())
        {
            CallFunctionInstruction* callFunctionInstruction = new CallFunctionInstruction();
            callFunctionInstruction->SetModuleId(method->ModuleId());
            if (method->IsDeclaration())
            {
                callFunctionInstruction->SetSubroutineId(method->ImplementationId());
            }
            else
            {
                callFunctionInstruction->SetSubroutineId(method->Id());
            }
            emitter->Emit(callFunctionInstruction);
        }
    }
}

void BoundMethodCallNode::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundExpressionNode* BoundMethodCallNode::Clone() const
{
    BoundMethodCallNode* clone = new BoundMethodCallNode(method, arguments, Pos());
    for (const auto& arg : arguments)
    {
        clone->AddArgument(arg->Clone());
    }
    return clone;
}

BoundNewExpressionNode::BoundNewExpressionNode(ObjectType* objectType_, int64_t pos_) : 
    BoundExpressionNode(BoundNodeKind::boundNewExpressionNode, pos_, objectType_), objectType(objectType_)
{
}

void BoundNewExpressionNode::Load(Emitter* emitter)
{
    NewObjectInstruction* newObjectInstruction = new NewObjectInstruction();
    newObjectInstruction->SetObjectTypeId(objectType->Id());
    emitter->Emit(newObjectInstruction);
}

void BoundNewExpressionNode::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundExpressionNode* BoundNewExpressionNode::Clone() const
{
    return new BoundNewExpressionNode(objectType, Pos());
}

BoundNewArrayExpressionNode::BoundNewArrayExpressionNode(ObjectType* objectType_, int32_t arraySize_, ArrayType* arrayType_, int64_t pos_) :
    BoundExpressionNode(BoundNodeKind::boundNewArrayExpressionNode, pos_, arrayType_), objectType(objectType_), arraySize(arraySize_)
{
}

void BoundNewArrayExpressionNode::Load(Emitter* emitter)
{
    NewArrayInstruction* newArrayInstruction = new NewArrayInstruction();
    newArrayInstruction->SetArrayTypeId(GetType()->Id());
    newArrayInstruction->SetLength(arraySize);
    emitter->Emit(newArrayInstruction);
}

void BoundNewArrayExpressionNode::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundExpressionNode* BoundNewArrayExpressionNode::Clone() const
{
    BoundNewArrayExpressionNode* clone = new BoundNewArrayExpressionNode(objectType, arraySize, static_cast<ArrayType*>(GetType()), Pos());
    return clone;
}

BoundMemberExprNode::BoundMemberExprNode(BoundExpressionNode* subject_, int32_t fieldIndex_, int64_t pos_, Type* type_) : 
    BoundExpressionNode(BoundNodeKind::boundMemberExprNode, pos_, type_), subject(subject_), fieldIndex(fieldIndex_)
{
}

void BoundMemberExprNode::Load(Emitter* emitter)
{
    subject->Load(emitter);
    LoadFieldInstruction* loadFieldInstruction = new LoadFieldInstruction();
    loadFieldInstruction->SetFieldIndex(fieldIndex);
    emitter->Emit(loadFieldInstruction);
}

void BoundMemberExprNode::Store(Emitter* emitter)
{
    subject->Load(emitter);
    StoreFieldInstruction* storeFieldInstruction = new StoreFieldInstruction();
    storeFieldInstruction->SetFieldIndex(fieldIndex);
    emitter->Emit(storeFieldInstruction);
}

void BoundMemberExprNode::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundExpressionNode* BoundMemberExprNode::Clone() const
{
    BoundMemberExprNode* clone = new BoundMemberExprNode(subject->Clone(), fieldIndex, Pos(), GetType());
    return clone;
}

Type* GetArrayElementType(Type* type)
{
    if (type->IsArrayType())
    {
        ArrayType* arrayType = static_cast<ArrayType*>(type);
        return arrayType->ElementType();
    }
    else
    {
        return nullptr;
    }
}

BoundIndexExprNode::BoundIndexExprNode(BoundExpressionNode* subject_, BoundExpressionNode* index_, int64_t pos_) :
    BoundExpressionNode(BoundNodeKind::boundIndexExprNode, pos_, GetArrayElementType(subject_->GetType())), subject(subject_), index(index_)
{
}

void BoundIndexExprNode::Load(Emitter* emitter)
{
    index->Load(emitter);
    subject->Load(emitter);
    emitter->Emit(new LoadElementInstruction());
}

void BoundIndexExprNode::Store(Emitter* emitter)
{
    index->Load(emitter);
    subject->Load(emitter);
    emitter->Emit(new StoreElementInstruction());
}

void BoundIndexExprNode::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundExpressionNode* BoundIndexExprNode::Clone() const
{
    return new BoundIndexExprNode(subject->Clone(), index->Clone(), Pos());
}

BoundArrayLengthNode::BoundArrayLengthNode(BoundExpressionNode* subject_, int64_t pos_, Type* integerType_) :
    BoundExpressionNode(BoundNodeKind::boundArrayLengthNode, pos_, integerType_), subject(subject_)
{
}

void BoundArrayLengthNode::Load(Emitter* emitter)
{
    subject->Load(emitter);
    emitter->Emit(new ArrayLengthInstruction());
}

void BoundArrayLengthNode::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundExpressionNode* BoundArrayLengthNode::Clone() const
{
    return new BoundArrayLengthNode(subject->Clone(), Pos(), GetType());
}

BoundStatementNode::BoundStatementNode(BoundNodeKind kind_, int64_t pos_) : BoundNode(kind_, pos_)
{
}

BoundEmptyStatementNode::BoundEmptyStatementNode(int64_t pos_) : BoundStatementNode(BoundNodeKind::boundEmptyStatementNode, pos_)
{
}

void BoundEmptyStatementNode::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundCompoundStatementNode::BoundCompoundStatementNode(int64_t pos_) : BoundStatementNode(BoundNodeKind::boundCompoundStatementNode, pos_)
{
}

void BoundCompoundStatementNode::AddStatement(BoundStatementNode* boundStatement)
{
    boundStatements.push_back(std::unique_ptr<BoundStatementNode>(boundStatement));
}

void BoundCompoundStatementNode::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundAssignmentStatementNode::BoundAssignmentStatementNode(BoundExpressionNode* target_, BoundExpressionNode* source_, int64_t pos_) : 
    BoundStatementNode(BoundNodeKind::boundAssignmentStatementNode, pos_), target(target_), source(source_)
{
}

void BoundAssignmentStatementNode::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundProcedureCallStatementNode::BoundProcedureCallStatementNode(Procedure* procedure_, int64_t pos_) : 
    BoundStatementNode(BoundNodeKind::boundProcedureCallStatementNode, pos_), procedure(procedure_)
{
}

void BoundProcedureCallStatementNode::AddArgument(BoundExpressionNode* argument)
{
    arguments.push_back(std::unique_ptr<BoundExpressionNode>(argument));
}

void BoundProcedureCallStatementNode::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundExpressionStatementNode::BoundExpressionStatementNode(BoundExpressionNode* boundExpression_, int64_t pos_) : 
    BoundStatementNode(BoundNodeKind::boundExpressionStatementNode, pos_), boundExpression(boundExpression_)
{
}

void BoundExpressionStatementNode::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundIfStatementNode::BoundIfStatementNode(BoundExpressionNode* condition_, BoundStatementNode* thenStatement_, BoundStatementNode* elseStatement_, int64_t pos_) :
    BoundStatementNode(BoundNodeKind::boundIfStatementNode, pos_), condition(condition_), thenStatement(thenStatement_), elseStatement(elseStatement_)
{
}

void BoundIfStatementNode::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundWhileStatementNode::BoundWhileStatementNode(BoundExpressionNode* condition_, BoundStatementNode* statement_, int64_t pos_) :
    BoundStatementNode(BoundNodeKind::boundWhileStatementNode, pos_), condition(condition_), statement(statement_)
{
}

void BoundWhileStatementNode::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundRepeatStatementNode::BoundRepeatStatementNode(int64_t pos_) : BoundStatementNode(BoundNodeKind::boundRepeatStatementNode, pos_)
{
}

void BoundRepeatStatementNode::AddStatement(BoundStatementNode* boundStatement)
{
    statements.push_back(std::unique_ptr<BoundStatementNode>(boundStatement));
}

void BoundRepeatStatementNode::SetCondition(BoundExpressionNode* condition_)
{
    condition.reset(condition_);
}

void BoundRepeatStatementNode::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundReturnFunctionResultStatementNode::BoundReturnFunctionResultStatementNode(BoundFunctionResultNode* functionResult_, int64_t pos_) :
    BoundStatementNode(BoundNodeKind::boundReturnFunctionResultStatementNode, pos_), functionResult(functionResult_)
{
}

void BoundReturnFunctionResultStatementNode::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

class ExpressionBinder : public Visitor
{
public:
    ExpressionBinder(ParsingContext* context_, Subroutine* subroutine_, soul::lexer::LexerBase<char>& lexer_, 
        const std::vector<std::unique_ptr<BoundExpressionNode>>& boundArguments_);
    std::unique_ptr<BoundExpressionNode> GetBoundExpressionNode();
    void Visit(BinaryExprNode& node) override;
    void Visit(UnaryExprNode& node) override;
    void Visit(ParenthesizedExprNode& node) override;
    void Visit(IntegerLiteralNode& node) override;
    void Visit(RealLiteralNode& node) override;
    void Visit(BooleanLiteralNode& node) override;
    void Visit(CharLiteralNode& node) override;
    void Visit(StringLiteralNode& node) override;
    void Visit(IdentifierNode& node) override;
    void Visit(QualifiedIdNode& node) override;
    void Visit(FieldNode& node) override;
    void Visit(MethodNode& node) override;
    void Visit(ThisNode& node) override;
    void Visit(BaseNode& node) override;
    void Visit(NilNode& node) override;
    void Visit(NewExprNode& node) override;
    void Visit(NewArrayExprNode& node) override;
    void Visit(ValueTypecastNode& node) override;
    void Visit(VariableTypecastNode& node) override;
    void Visit(InvokeExprNode& node) override;
    void Visit(IndexExprNode& node) override;
    void Visit(DotNode& node) override;
private:
    ParsingContext* context;
    Subroutine* subroutine;
    soul::lexer::LexerBase<char>& lexer;
    const std::vector<std::unique_ptr<BoundExpressionNode>>& boundArguments;
    std::unique_ptr<BoundExpressionNode> boundExpressionNode;
};

ExpressionBinder::ExpressionBinder(ParsingContext* context_, Subroutine* subroutine_, soul::lexer::LexerBase<char>& lexer_, 
    const std::vector<std::unique_ptr<BoundExpressionNode>>& boundArguments_) :
    context(context_), subroutine(subroutine_), lexer(lexer_), boundArguments(boundArguments_)
{
}

std::unique_ptr<BoundExpressionNode> ExpressionBinder::GetBoundExpressionNode()
{
    return std::move(boundExpressionNode);
}

void ExpressionBinder::Visit(BinaryExprNode& node)
{
    node.Left()->Accept(*this);
    std::unique_ptr<BoundExpressionNode> left(boundExpressionNode.release());
    node.Right()->Accept(*this);
    std::unique_ptr<BoundExpressionNode> right(boundExpressionNode.release());
    TypeKind commonType = GetCommonType(left->GetType()->Kind(), right->GetType()->Kind());
    if (commonType != TypeKind::none)
    {
        if (left->GetType()->Kind() != commonType)
        {
            Type* targetType = context->GetBlock()->GetFundamentalType(commonType, lexer, node.Pos());
            Type* leftType = left->GetType();
            BoundConversionNode* conversion = new BoundConversionNode(GetConversionFunction(targetType, leftType, lexer, node.Pos(), true), left.release(), node.Pos());
            left.reset(conversion);
        }
        if (right->GetType()->Kind() != commonType)
        {
            Type* targetType = context->GetBlock()->GetFundamentalType(commonType, lexer, node.Pos());
            Type* rightType = right->GetType();
            BoundConversionNode* conversion = new BoundConversionNode(GetConversionFunction(targetType, rightType, lexer, node.Pos(), true), right.release(), node.Pos());
            right.reset(conversion);
        }
        Type* type = context->GetBlock()->GetFundamentalType(commonType, lexer, node.Pos());
        Function* binaryOperatorFunction = GetBinaryOperatorFunction(node.Op(), type, lexer, node.Pos());
        boundExpressionNode.reset(new BoundBinaryExpressionNode(binaryOperatorFunction, left.release(), right.release(), node.Pos()));
    }
    else
    {
        ThrowError("invalid binary operation operand type", lexer, node.Pos());
    }
}

void ExpressionBinder::Visit(UnaryExprNode& node)
{
    node.Operand()->Accept(*this);
    std::unique_ptr<BoundExpressionNode> operand(boundExpressionNode.release());
    Function* unaryOperatorFunction = GetUnaryOperatorFunction(node.Op(), operand->GetType(), lexer, node.Pos());
    boundExpressionNode.reset(new BoundUnaryExpressionNode(unaryOperatorFunction, operand.release(), node.Pos()));
}

void ExpressionBinder::Visit(ParenthesizedExprNode& node)
{
    boundExpressionNode = BindExpression(context, subroutine, lexer, node.Expr(), boundArguments);
}

void ExpressionBinder::Visit(IntegerLiteralNode& node)
{
    boundExpressionNode.reset(new BoundLiteralNode(node.Pos(), context->GetBlock()->GetType("integer", lexer, node.Pos()), new IntegerValue(node.Value())));
}

void ExpressionBinder::Visit(RealLiteralNode& node)
{
    boundExpressionNode.reset(new BoundLiteralNode(node.Pos(), context->GetBlock()->GetType("real", lexer, node.Pos()), new RealValue(node.Value())));
}

void ExpressionBinder::Visit(BooleanLiteralNode& node)
{
    boundExpressionNode.reset(new BoundLiteralNode(node.Pos(), context->GetBlock()->GetType("boolean", lexer, node.Pos()), new BooleanValue(node.Value())));
}

void ExpressionBinder::Visit(CharLiteralNode& node)
{
    boundExpressionNode.reset(new BoundLiteralNode(node.Pos(), context->GetBlock()->GetType("char", lexer, node.Pos()), new CharValue(node.Chr())));
}

void ExpressionBinder::Visit(StringLiteralNode& node)
{
    boundExpressionNode.reset(new BoundLiteralNode(node.Pos(), context->GetBlock()->GetType("string", lexer, node.Pos()), new StringValue(node.Value())));
}

void ExpressionBinder::Visit(IdentifierNode& node)
{
    switch (node.Kind())
    {
        case IdentifierKind::constant:
        {
            Constant* constant = node.GetConstant();
            boundExpressionNode.reset(new BoundConstantNode(node.Pos(), constant->GetType(), constant));
            break;
        }
        case IdentifierKind::type:
        {
            // todo
            break;
        }
        case IdentifierKind::variable:
        {
            Variable* variable = node.GetVariable();
            boundExpressionNode.reset(new BoundVariableNode(node.Pos(), variable));
            break;
        }
        case IdentifierKind::object:
        {
            // todo
            break;
        }
        case IdentifierKind::procedure:
        {
            Procedure* procedure = node.GetProcedure();
            boundExpressionNode.reset(new BoundProcedureNode(procedure, node.Pos()));
            break;
        }
        case IdentifierKind::function:
        {
            Function* function = node.GetFunction();
            boundExpressionNode.reset(new BoundFunctionNode(function, boundArguments, node.Pos()));
            break;
        }
    }
}

void ExpressionBinder::Visit(QualifiedIdNode& node)
{

}

void ExpressionBinder::Visit(FieldNode& node)
{
    BoundMemberExprNode* boundMemberExprNode = new BoundMemberExprNode(
        new BoundParameterNode(node.Pos(), subroutine->Heading()->ThisParam()), node.FieldIndex(), node.Pos(), node.GetType());
    boundExpressionNode.reset(boundMemberExprNode);
}

void ExpressionBinder::Visit(MethodNode& node)
{
    BoundMethodNode* boundMethodNode = new BoundMethodNode(new BoundParameterNode(node.Pos(), subroutine->Heading()->ThisParam()), node.GetMethod(), node.Pos());
    boundExpressionNode.reset(boundMethodNode);
}

void ExpressionBinder::Visit(ThisNode& node)
{
    if (subroutine->Heading()->GetObjectType())
    {
        boundExpressionNode.reset(new BoundParameterNode(node.Pos(), subroutine->Heading()->ThisParam()));
        boundExpressionNode->SetArgumentFlags(ArgumentFlags::thisOrBaseArgument);
    }
    else
    {
        ThrowError("'this' requires object type context", lexer, node.Pos());
    }
}

void ExpressionBinder::Visit(BaseNode& node)
{
    ObjectType* objectType = subroutine->Heading()->GetObjectType();
    if (objectType)
    {
        ObjectType* baseType = objectType->BaseType();
        if (!baseType)
        {
            ThrowError("'base' requires base object type context; " + objectType->Name() + "' has no base type", lexer, node.Pos());
        }
        boundExpressionNode.reset(new BoundParameterNode(node.Pos(), subroutine->Heading()->ThisParam(), baseType));
        boundExpressionNode->SetArgumentFlags(ArgumentFlags::thisOrBaseArgument);
    }
    else
    {
        ThrowError("'base' requires object type context", lexer, node.Pos());
    }
}

void ExpressionBinder::Visit(NilNode& node)
{

}

void ExpressionBinder::Visit(NewExprNode& node)
{
    Block* block = context->GetBlock();
    Type* type = block->GetType(node.ObjectTypeId()->Str(), lexer, node.Pos());
    if (type->IsObjectType())
    {
        ObjectType* objectType = static_cast<ObjectType*>(type);
        boundExpressionNode.reset(new BoundNewExpressionNode(objectType, node.Pos()));
    }
    else
    {
        ThrowError("object type expected", lexer, node.Pos());
    }
}

void ExpressionBinder::Visit(NewArrayExprNode& node)
{
    Block* block = context->GetBlock();
    Type* type = block->GetType(node.ObjectTypeId()->Str(), lexer, node.Pos());
    if (type->IsObjectType())
    {
        ObjectType* objectType = static_cast<ObjectType*>(type);
        int32_t arraySize = node.ArraySize()->Value();
        ArrayType* arrayType = block->GetArrayType(objectType->Name(), lexer, node.Pos());
        boundExpressionNode.reset(new BoundNewArrayExpressionNode(objectType, arraySize, arrayType, node.Pos()));
    }
    else
    {
        ThrowError("object type expected", lexer, node.Pos());
    }
}

void ExpressionBinder::Visit(ValueTypecastNode& node)
{
    Type* type = context->GetBlock()->GetType(node.TypeName());
    if (!type)
    {
        ThrowError("type '" + node.TypeName() + "' not found", lexer, node.Pos());
    }
    node.Expression()->Accept(*this);
    boundExpressionNode.reset(new BoundValueConversionNode(boundExpressionNode.release(), type, node.Pos()));
}

void ExpressionBinder::Visit(VariableTypecastNode& node)
{
    // todo
}

void ExpressionBinder::Visit(InvokeExprNode& node)
{
    Subroutine* subroutine = nullptr;
    std::vector<std::unique_ptr<BoundExpressionNode>> boundArguments;
    for (const auto& argument : node.Arguments())
    {
        std::unique_ptr<BoundExpressionNode> boundArgumentNode = BindExpression(context, this->subroutine, lexer, argument.get());
        boundArguments.push_back(std::unique_ptr<BoundExpressionNode>(boundArgumentNode.release()));
    }
    std::unique_ptr<BoundExpressionNode> subjectNode = BindExpression(context, this->subroutine, lexer, node.Subject(), boundArguments);
    if (subjectNode->IsBoundMethodNode())
    {
        BoundMethodNode* boundMethodNode = static_cast<BoundMethodNode*>(subjectNode.get());
        boundArguments.insert(boundArguments.begin(), std::unique_ptr<BoundExpressionNode>(boundMethodNode->ReleaseSubject()));
        boundExpressionNode.reset(MakeBoundMethodCall(boundMethodNode->GetMethod(), boundArguments, lexer, node.Pos()));
    }
    else if (subjectNode->IsBoundFunctionNode())
    {
        BoundFunctionNode* boundFunctionNode = static_cast<BoundFunctionNode*>(subjectNode.get());
        if (boundFunctionNode->GetFunction()->MinParameterCount() > boundArguments.size() && 
            boundFunctionNode->GetFunction()->Heading()->GetObjectType() == this->subroutine->Heading()->GetObjectType())
        {
            boundArguments.insert(boundArguments.begin(), std::unique_ptr<BoundExpressionNode>(new BoundParameterNode(node.Pos(), boundFunctionNode->GetFunction()->Heading()->ThisParam())));
        }
        if (boundFunctionNode->GetFunction()->MinParameterCount() > boundArguments.size())
        {
            ThrowError("error: function '" + boundFunctionNode->GetFunction()->FullName() + "' takes " +
                std::to_string(boundFunctionNode->GetFunction()->MinParameterCount()) + " parameters; " +
                std::to_string(boundArguments.size()) + " arguments passed", lexer, node.Pos());
        }
        subroutine = boundFunctionNode->GetFunction();
    }
    else if (subjectNode->IsBoundNewExpressionNode())
    {
        BoundNewExpressionNode* boundNewExpressionNode = static_cast<BoundNewExpressionNode*>(subjectNode.get());
        ObjectType* objectType = boundNewExpressionNode->GetObjectType();
        boundArguments.insert(boundArguments.begin(), std::move(subjectNode));
        Constructor* constructor = objectType->GetConstructor(boundArguments, lexer, node.Pos());
        if (constructor)
        {
            subroutine = constructor;
        }
        else if (!boundArguments.empty())
        {
            ThrowError("matching constructor not found", lexer, node.Pos());
        }
        else
        {
            boundExpressionNode.reset(subjectNode.release());
        }
    }
    else
    {
        ThrowError("bound function or bound new expression node expected", lexer, node.Pos());
    }
    if (subroutine)
    {
        if (subroutine->IsFunction())
        {
            Function* function = static_cast<Function*>(subroutine);
            BoundFunctionCallNode* boundFunctionCallNode = new BoundFunctionCallNode(function, boundArguments, node.Pos());
            for (auto& boundArgument : boundArguments)
            {
                boundFunctionCallNode->AddArgument(boundArgument.release());
            }
            boundExpressionNode.reset(boundFunctionCallNode);
        }
        else if (subroutine->IsConstructor())
        {
            Constructor* constructor = static_cast<Constructor*>(subroutine);
            BoundConstructorCallNode* boundConstructorCallNode = new BoundConstructorCallNode(constructor, node.Pos());
            for (auto& boundArgument : boundArguments)
            {
                boundConstructorCallNode->AddArgument(boundArgument.release());
            }
            boundExpressionNode.reset(boundConstructorCallNode);
        }
    }
}

void ExpressionBinder::Visit(IndexExprNode& node)
{
    node.Subject()->Accept(*this);
    std::unique_ptr<BoundExpressionNode> subject = std::move(boundExpressionNode);
    node.Index()->Accept(*this);
    std::unique_ptr<BoundExpressionNode> index = std::move(boundExpressionNode);
    boundExpressionNode.reset(new BoundIndexExprNode(subject.release(), index.release(), node.Pos()));
}

void ExpressionBinder::Visit(DotNode& node)
{
    node.Subject()->Accept(*this);
    if (boundExpressionNode->GetType()->IsObjectType())
    {
        ObjectType* objectType = static_cast<ObjectType*>(boundExpressionNode->GetType());
        int32_t fieldIndex = objectType->GetFieldIndex(node.Id()->Str());
        if (fieldIndex != -1)
        {
            boundExpressionNode.reset(new BoundMemberExprNode(boundExpressionNode.release(), fieldIndex, node.Pos(), objectType->GetFieldType(fieldIndex)));
        }
        else
        {
            Subroutine* method = objectType->GetMethod(node.Id()->Str());
            if (method)
            {
                boundExpressionNode.reset(new BoundMethodNode(boundExpressionNode.release(), method, node.Pos()));
            }
            else
            {
                ThrowError("field or method '" + node.Id()->Str() + "' not found", lexer, node.Pos());
            }
        }
    }
    else if (boundExpressionNode->GetType()->IsArrayType())
    {
        if (node.Id()->Str() == "Length")
        {
            Type* integerType = context->GetBlock()->GetType("integer", lexer, node.Pos());
            boundExpressionNode.reset(new BoundArrayLengthNode(boundExpressionNode.release(), node.Pos(), integerType));
        }
        else
        {
            ThrowError("array.Length expected", lexer, node.Pos());
        }
    }
    else
    {
        ThrowError("object or array type subject expected", lexer, node.Pos());
    }
}

std::unique_ptr<BoundExpressionNode> BindExpression(ParsingContext* context, Subroutine* subroutine, soul::lexer::LexerBase<char>& lexer, Node* node,
    const std::vector<std::unique_ptr<BoundExpressionNode>>& boundArguments)
{
    ExpressionBinder binder(context, subroutine, lexer, boundArguments);
    node->Accept(binder);
    return binder.GetBoundExpressionNode();
}

std::unique_ptr<BoundExpressionNode> BindExpression(ParsingContext* context, Subroutine* subroutine, soul::lexer::LexerBase<char>& lexer, Node* node)
{
    std::vector<std::unique_ptr<BoundExpressionNode>> boundArguments;
    ExpressionBinder binder(context, subroutine, lexer, boundArguments);
    node->Accept(binder);
    return binder.GetBoundExpressionNode();
}

class StatementBinder : public Visitor
{
public:
    StatementBinder(ParsingContext* context_, Subroutine* subroutine_, soul::lexer::LexerBase<char>& lexer_);
    std::unique_ptr<BoundCompoundStatementNode> GetBoundCompoundStatement();
    void Visit(AssignmentStatementNode& node) override;
    void Visit(ProcedureCallStatementNode& node) override;
    void Visit(ExpressionStatementNode& node) override;
    void Visit(EmptyStatementNode& node) override;
    void Visit(CompoundStatementNode& node) override;
    void Visit(IfStatementNode& node) override;
    void Visit(CaseNode& node) override;
    void Visit(ConstantRangeNode& node) override;
    void Visit(CaseStatementNode& node) override;
    void Visit(RepeatStatementNode& node) override;
    void Visit(WhileStatementNode& node) override;
    void Visit(ForStatementNode& node) override;
private:
    ParsingContext* context;
    Subroutine* subroutine;
    soul::lexer::LexerBase<char>& lexer;
    std::unique_ptr<BoundStatementNode> boundStatement;
    std::stack<std::unique_ptr<BoundStatementNode>> boundStatementStack;
    int32_t level;
};

StatementBinder::StatementBinder(ParsingContext* context_, Subroutine* subroutine_, soul::lexer::LexerBase<char>& lexer_) : 
    context(context_), subroutine(subroutine_), lexer(lexer_), level(0)
{
}

std::unique_ptr<BoundCompoundStatementNode> StatementBinder::GetBoundCompoundStatement()
{
    if (!boundStatement)
    {
        throw std::runtime_error("bound statement is null");
    }
    if (boundStatement->IsBoundCompoundStatementNode())
    {
        return std::unique_ptr<BoundCompoundStatementNode>(static_cast<BoundCompoundStatementNode*>(boundStatement.release()));
    }
    else
    {
        ThrowError("bound compound statement expected", lexer, boundStatement->Pos());
    }
    return std::unique_ptr<BoundCompoundStatementNode>();
}

void StatementBinder::Visit(AssignmentStatementNode& node)
{
    std::unique_ptr<BoundExpressionNode> target = BindExpression(context, subroutine, lexer, node.Target());
    if (target->IsBoundFunctionNode())
    {
        BoundFunctionNode* boundFunction = static_cast<BoundFunctionNode*>(target.get());
        Function* function = boundFunction->GetFunction();
        if (function == subroutine)
        {
            Variable* resultVar = context->GetBlock()->GetVariable("@result");
            if (resultVar)
            {
                target.reset(new BoundVariableNode(node.Pos(), resultVar));
            }
            else
            {
                ThrowError("function result variable not found", lexer, node.Pos());
            }
        }
        else
        {
            ThrowError("invalid function result assignment", lexer, node.Pos());
        }
    }
    std::unique_ptr<BoundExpressionNode> source = BindExpression(context, subroutine, lexer, node.Source());
    if (target->GetType() != source->GetType() && !target->GetType()->IsArrayType() && !source->GetType()->IsArrayType())
    {
        Function* conversionFunction = GetConversionFunction(target->GetType(), source->GetType(), lexer, node.Pos(), false);
        if (!conversionFunction)
        {
            ThrowError("error: invalid assignment in subroutine '" + subroutine->FullName() + "'", lexer, node.Pos());
        }
        source.reset(new BoundConversionNode(conversionFunction, source.release(), node.Pos()));
    }
    boundStatement.reset(new BoundAssignmentStatementNode(target.release(), source.release(), node.Pos()));
}

void StatementBinder::Visit(ProcedureCallStatementNode& node)
{
    std::unique_ptr<BoundExpressionNode> procedureExprNode = BindExpression(context, subroutine, lexer, node.ProcedureId());
    if (!procedureExprNode->IsBoundProcedureNode())
    {
        ThrowError("bound procedure node expected", lexer, node.Pos());
    }
    BoundProcedureNode* boundProcedureNode = static_cast<BoundProcedureNode*>(procedureExprNode.get());
    if (boundProcedureNode->GetProcedure()->MinParameterCount() > node.Arguments().size())
    {
        ThrowError("error: procedure '" + boundProcedureNode->GetProcedure()->FullName() + "' takes " + 
            std::to_string(boundProcedureNode->GetProcedure()->MinParameterCount()) + " parameters; " + 
            std::to_string(node.Arguments().size()) + " arguments passed", lexer, node.Pos());
    }
    BoundProcedureCallStatementNode* boundProcedureCallStatementNode = new BoundProcedureCallStatementNode(boundProcedureNode->GetProcedure(), node.Pos());
    for (const auto& argument : node.Arguments())
    {
        std::unique_ptr<BoundExpressionNode> boundArgumentNode = BindExpression(context, subroutine, lexer, argument.get());
        boundProcedureCallStatementNode->AddArgument(boundArgumentNode.release());
    }
    boundStatement.reset(boundProcedureCallStatementNode);
}

void StatementBinder::Visit(ExpressionStatementNode& node)
{
    std::unique_ptr<BoundExpressionNode> boundExpressionNode = BindExpression(context, subroutine, lexer, node.Expr());
    boundStatement.reset(new BoundExpressionStatementNode(boundExpressionNode.release(), node.Pos()));
}

void StatementBinder::Visit(EmptyStatementNode& node)
{
    boundStatement.reset(new BoundEmptyStatementNode(node.Pos()));
}

void StatementBinder::Visit(CompoundStatementNode& node)
{
    std::unique_ptr<BoundCompoundStatementNode> boundCompoundStatement(new BoundCompoundStatementNode(node.Pos()));
    for (const auto& statement : node.Statements())
    {
        statement->Accept(*this);
        boundCompoundStatement->AddStatement(boundStatement.release());
    }
    if (level == 0 && subroutine->IsFunction())
    {
        Variable* functionResultVar = context->GetBlock()->GetVariable("@result");
        if (functionResultVar)
        {
            BoundFunctionResultNode* boundFunctionResult = new BoundFunctionResultNode(node.Pos(), functionResultVar);
            boundCompoundStatement->AddStatement(new BoundReturnFunctionResultStatementNode(boundFunctionResult, node.Pos()));
        }
        else
        {
            ThrowError("function result not found", lexer, node.Pos());
        }
    }
    boundStatement.reset(boundCompoundStatement.release());
}

void StatementBinder::Visit(IfStatementNode& node)
{
    std::unique_ptr<BoundExpressionNode> condition = BindExpression(context, subroutine, lexer, node.Condition());
    if (!condition->GetType()->IsBooleanType())
    {
        ThrowError("Boolean condition expected", lexer, node.Pos());
    }
    node.ThenStatement()->Accept(*this);
    std::unique_ptr<BoundStatementNode> thenStatement = std::move(boundStatement);
    std::unique_ptr<BoundStatementNode> elseStatement;
    if (node.ElseStatement())
    {
        node.ElseStatement()->Accept(*this);
        elseStatement = std::move(boundStatement);
    }
    std::unique_ptr<BoundIfStatementNode> boundIfStatement(new BoundIfStatementNode(condition.release(), thenStatement.release(), elseStatement.release(), node.Pos()));
    boundStatement.reset(boundIfStatement.release());
}

void StatementBinder::Visit(CaseNode& node)
{

}

void StatementBinder::Visit(ConstantRangeNode& node)
{

}

void StatementBinder::Visit(CaseStatementNode& node)
{

}

void StatementBinder::Visit(RepeatStatementNode& node)
{
    BoundRepeatStatementNode* boundRepeatStatementNode = new BoundRepeatStatementNode(node.Pos());
    for (const auto& statement : node.Statements())
    {
        statement->Accept(*this);
        boundRepeatStatementNode->AddStatement(boundStatement.release());
    }
    std::unique_ptr<BoundExpressionNode> condition = BindExpression(context, subroutine, lexer, node.Condition());
    if (!condition->GetType()->IsBooleanType())
    {
        ThrowError("Boolean condition expected", lexer, node.Pos());
    }
    boundRepeatStatementNode->SetCondition(condition.release());
}

void StatementBinder::Visit(WhileStatementNode& node)
{
    std::unique_ptr<BoundExpressionNode> condition = BindExpression(context, subroutine, lexer, node.Condition());
    if (condition->GetType()->IsBooleanType())
    {
        node.Statement()->Accept(*this);
        std::unique_ptr<BoundStatementNode> statement = std::move(boundStatement);
        boundStatement.reset(new BoundWhileStatementNode(condition.release(), statement.release(), node.Pos()));
    }
    else
    {
        ThrowError("Boolean condition expected", lexer, node.Pos());
    }
}

void StatementBinder::Visit(ForStatementNode& node)
{
    if (node.Dir() == Direction::to)
    {
        std::unique_ptr<CompoundStatementNode> toCompound(new CompoundStatementNode(node.Pos()));
        std::unique_ptr<BoundExpressionNode> initialValue = BindExpression(context, subroutine, lexer, node.InitialValue());
        Variable* initialTemp = subroutine->MakeTempVar(context->GetBlock(), initialValue->GetType());
        std::unique_ptr<IdentifierNode> initialTempId(MakeVariableId(context, initialTemp->Name(), node.Pos()));
        std::unique_ptr<BoundExpressionNode> finalValue = BindExpression(context, subroutine, lexer, node.FinalValue());
        Variable* finalTemp = subroutine->MakeTempVar(context->GetBlock(), finalValue->GetType());
        std::unique_ptr<IdentifierNode> finalTempId(MakeVariableId(context, finalTemp->Name(), node.Pos()));
        AssignmentStatementNode* assignInitialTemp = new AssignmentStatementNode(initialTempId->Clone(), node.InitialValue()->Clone(), node.Pos());
        toCompound->AddStatement(assignInitialTemp);
        AssignmentStatementNode* assignFinalTemp = new AssignmentStatementNode(finalTempId->Clone(), node.FinalValue()->Clone(), node.Pos());
        toCompound->AddStatement(assignFinalTemp);
        BinaryExprNode* cond = new BinaryExprNode(Operator::lessOrEqual, initialTempId->Clone(), finalTempId->Clone(), node.Pos());
        CompoundStatementNode* thenStatement = new CompoundStatementNode(node.Pos());
        thenStatement->AddStatement(new AssignmentStatementNode(node.ControlVariable()->Clone(), initialTempId->Clone(), node.Pos()));
        thenStatement->AddStatement(static_cast<StatementNode*>(node.Statement()->Clone()));
        BinaryExprNode* whileCond = new BinaryExprNode(Operator::notEqual, node.ControlVariable()->Clone(), finalTempId->Clone(), node.Pos());
        CompoundStatementNode* doStatement = new CompoundStatementNode(node.Pos());
        InvokeExprNode* succExpr = new InvokeExprNode(MakeFunctionId(context, "Succ", node.Pos()), node.Pos());
        succExpr->AddNode(node.ControlVariable()->Clone(), lexer, node.Pos());
        doStatement->AddStatement(new AssignmentStatementNode(node.ControlVariable()->Clone(), succExpr, node.Pos()));
        doStatement->AddStatement(static_cast<StatementNode*>(node.Statement()->Clone()));
        WhileStatementNode* whileStatement = new WhileStatementNode(whileCond, doStatement, node.Pos());
        thenStatement->AddStatement(whileStatement);
        IfStatementNode* ifStatement = new IfStatementNode(cond, thenStatement, node.Pos());
        toCompound->AddStatement(ifStatement);
        std::unique_ptr<BoundCompoundStatementNode> boundCompoundStatement = Bind(context, toCompound.get(), subroutine, lexer);
        boundStatement = std::move(boundCompoundStatement);
    }
    else if (node.Dir() == Direction::downto)
    {
        std::unique_ptr<CompoundStatementNode> downtoCompound(new CompoundStatementNode(node.Pos()));
        std::unique_ptr<BoundExpressionNode> initialValue = BindExpression(context, subroutine, lexer, node.InitialValue());
        Variable* initialTemp = subroutine->MakeTempVar(context->GetBlock(), initialValue->GetType());
        std::unique_ptr<IdentifierNode> initialTempId(MakeVariableId(context, initialTemp->Name(), node.Pos()));
        std::unique_ptr<BoundExpressionNode> finalValue = BindExpression(context, subroutine, lexer, node.FinalValue());
        Variable* finalTemp = subroutine->MakeTempVar(context->GetBlock(), finalValue->GetType());
        std::unique_ptr<IdentifierNode> finalTempId(MakeVariableId(context, finalTemp->Name(), node.Pos()));
        AssignmentStatementNode* assignInitialTemp = new AssignmentStatementNode(initialTempId->Clone(), node.InitialValue()->Clone(), node.Pos());
        downtoCompound->AddStatement(assignInitialTemp);
        AssignmentStatementNode* assignFinalTemp = new AssignmentStatementNode(finalTempId->Clone(), node.FinalValue()->Clone(), node.Pos());
        downtoCompound->AddStatement(assignFinalTemp);
        BinaryExprNode* cond = new BinaryExprNode(Operator::greaterOrEqual, initialTempId->Clone(), finalTempId->Clone(), node.Pos());
        CompoundStatementNode* thenStatement = new CompoundStatementNode(node.Pos());
        thenStatement->AddStatement(new AssignmentStatementNode(node.ControlVariable()->Clone(), initialTempId->Clone(), node.Pos()));
        thenStatement->AddStatement(static_cast<StatementNode*>(node.Statement()->Clone()));
        BinaryExprNode* whileCond = new BinaryExprNode(Operator::notEqual, node.ControlVariable()->Clone(), finalTempId->Clone(), node.Pos());
        CompoundStatementNode* doStatement = new CompoundStatementNode(node.Pos());
        InvokeExprNode* predExpr = new InvokeExprNode(MakeFunctionId(context, "Pred", node.Pos()), node.Pos());
        predExpr->AddNode(node.ControlVariable()->Clone(), lexer, node.Pos());
        doStatement->AddStatement(new AssignmentStatementNode(node.ControlVariable()->Clone(), predExpr, node.Pos()));
        doStatement->AddStatement(static_cast<StatementNode*>(node.Statement()->Clone()));
        WhileStatementNode* whileStatement = new WhileStatementNode(whileCond, doStatement, node.Pos());
        thenStatement->AddStatement(whileStatement);
        IfStatementNode* ifStatement = new IfStatementNode(cond, thenStatement, node.Pos());
        downtoCompound->AddStatement(ifStatement);
        std::unique_ptr<BoundCompoundStatementNode> boundCompoundStatement = Bind(context, downtoCompound.get(), subroutine, lexer);
        boundStatement = std::move(boundCompoundStatement);
    }
}

BoundNodeVisitor::BoundNodeVisitor()
{
}

BoundNodeVisitor::~BoundNodeVisitor()
{
}

std::unique_ptr<BoundCompoundStatementNode> Bind(ParsingContext* context, CompoundStatementNode* compoundStatement, Subroutine* subroutine, soul::lexer::LexerBase<char>& lexer)
{
    if (subroutine && subroutine->Heading()->GetObjectType() && subroutine->Heading()->GetObjectType()->Name() == "Arrow")
    {
        int x = 0;
    }
    StatementBinder binder(context, subroutine, lexer);
    compoundStatement->Accept(binder);
    std::unique_ptr<BoundCompoundStatementNode> statement = binder.GetBoundCompoundStatement();
    return statement;
}

} // namespace p
