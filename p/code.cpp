// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

module p.code;

import p.execute;
import p.block;
import p.ast;
import p.binder;
import p.stdfn;
import p.subroutine;
import p.basic_block;
import p.rt;

namespace p {

Patch::Patch(BasicBlock* block_) : block(block_)
{
}

Patch::~Patch()
{
}

void Patch::Apply()
{
}

BranchTruePatch::BranchTruePatch(BasicBlock* block_, BranchInstruction* branchInstruction_) : Patch(block_), branchInstruction(branchInstruction_)
{
}

void BranchTruePatch::Apply()
{
    branchInstruction->SetTrueNext(GetBasicBlock()->First());
}

BranchFalsePatch::BranchFalsePatch(BasicBlock* block_, BranchInstruction* branchInstruction_) : Patch(block_), branchInstruction(branchInstruction_)
{
}

void BranchFalsePatch::Apply()
{
    branchInstruction->SetFalseNext(GetBasicBlock()->First());
}

JumpNextPatch::JumpNextPatch(BasicBlock* block_, JumpInstruction* jumpInstruction_) : Patch(block_), jumpInstruction(jumpInstruction_)
{
}

void JumpNextPatch::Apply()
{
    jumpInstruction->SetTarget(GetBasicBlock()->First());
}

Emitter::Emitter(soul::lexer::LexerBase<char>& lexer_, int64_t pos, Context* context_) : 
    lexer(lexer_), currentSubroutine(nullptr), currentBasicBlock(nullptr), instructionIndex(0), context(context_)
{
}

void Emitter::SetCurrentSubroutine(Subroutine* currentSubroutine_)
{
    currentSubroutine = currentSubroutine_;
    currentBasicBlock = currentSubroutine->AddBasicBlock();
}

void Emitter::PushBasicBlock(BasicBlock* basicBlock)
{
    basicBlockStack.push(currentBasicBlock);
    currentBasicBlock = basicBlock;
}

void Emitter::PopBasicBlock()
{
    currentBasicBlock = basicBlockStack.top();
    basicBlockStack.pop();
}

void Emitter::Emit(Instruction* instruction)
{
    currentBasicBlock->AddInstruction(instruction);
    instruction->SetInstIndex(instructionIndex++);
    ApplyPatches();
}

void Emitter::EnqueuePatch(Patch* patch)
{
    std::vector<std::unique_ptr<Patch>>& patches = patchMap[patch->GetBasicBlock()];
    patches.push_back(std::unique_ptr<Patch>(patch));
}

void Emitter::ApplyPatches()
{
    if (currentBasicBlock && !currentBasicBlock->IsEmpty())
    {
        auto it = patchMap.find(currentBasicBlock);
        if (it != patchMap.end())
        {
            std::vector<std::unique_ptr<Patch>>& patches = patchMap[currentBasicBlock];
            for (const auto& patch : patches)
            {
                patch->Apply();
            }
            patchMap.erase(currentBasicBlock);
        }
    }
}

class CodeGeneratorVisitor : public BoundNodeVisitor
{
public:
    CodeGeneratorVisitor(Subroutine* subroutine_, Block* block_, soul::lexer::LexerBase<char>& lexer_, int64_t pos_, Context* context_);
    void Visit(BoundBinaryExpressionNode& node) override;
    void Visit(BoundUnaryExpressionNode& node) override;
    void Visit(BoundLiteralNode& node) override;
    void Visit(BoundVariableNode& node) override;
    void Visit(BoundFunctionResultNode& node) override;
    void Visit(BoundConstantNode& node) override;
    void Visit(BoundConversionNode& node) override;
    void Visit(BoundValueConversionNode& node) override;
    void Visit(BoundProcedureNode& node) override;
    void Visit(BoundFunctionNode& node) override;
    void Visit(BoundMethodNode& node) override;
    void Visit(BoundFunctionCallNode& node) override;
    void Visit(BoundMethodCallNode& node) override;
    void Visit(BoundNewExpressionNode& node) override;
    void Visit(BoundNewArrayExpressionNode& node) override;
    void Visit(BoundMemberExprNode& node) override;
    void Visit(BoundIndexExprNode& node) override;
    void Visit(BoundEmptyStatementNode& node) override;
    void Visit(BoundCompoundStatementNode& node) override;
    void Visit(BoundAssignmentStatementNode& node) override;
    void Visit(BoundProcedureCallStatementNode& node) override;
    void Visit(BoundExpressionStatementNode& node) override;
    void Visit(BoundIfStatementNode& node) override;
    void Visit(BoundWhileStatementNode& node) override;
    void Visit(BoundRepeatStatementNode& node) override;
    void Visit(BoundReturnFunctionResultStatementNode& node) override;
private:
    Subroutine* subroutine;
    Block* block;
    Emitter emitter;
    int level;
};

CodeGeneratorVisitor::CodeGeneratorVisitor(Subroutine* subroutine_, Block* block_, soul::lexer::LexerBase<char>& lexer_, int64_t pos_, Context* context_) : 
    subroutine(subroutine_), block(block_), emitter(lexer_, pos_, context_), level(0)
{
    emitter.SetCurrentSubroutine(subroutine);
}

void CodeGeneratorVisitor::Visit(BoundBinaryExpressionNode& node)
{
    node.Load(&emitter);
}

void CodeGeneratorVisitor::Visit(BoundUnaryExpressionNode& node)
{
    node.Load(&emitter);
}

void CodeGeneratorVisitor::Visit(BoundLiteralNode& node)
{
    node.Load(&emitter);
}

void CodeGeneratorVisitor::Visit(BoundVariableNode& node)
{
    node.Load(&emitter);
}

void CodeGeneratorVisitor::Visit(BoundFunctionResultNode& node)
{
    node.Load(&emitter);
}

void CodeGeneratorVisitor::Visit(BoundConstantNode& node)
{
    node.Load(&emitter);
}

void CodeGeneratorVisitor::Visit(BoundConversionNode& node)
{
    node.Load(&emitter);
}

void CodeGeneratorVisitor::Visit(BoundValueConversionNode& node)
{
    node.Load(&emitter);
}

void CodeGeneratorVisitor::Visit(BoundProcedureNode& node) 
{
    ThrowError("cannot generate code for bound procedure node", emitter.Lexer(), node.Pos());
}

void CodeGeneratorVisitor::Visit(BoundFunctionNode& node)
{
    ThrowError("cannot generate code for bound function node", emitter.Lexer(), node.Pos());
}

void CodeGeneratorVisitor::Visit(BoundMethodNode& node)
{
    ThrowError("cannot generate code for bound method node", emitter.Lexer(), node.Pos());
}

void CodeGeneratorVisitor::Visit(BoundFunctionCallNode& node)
{
    node.Load(&emitter);
}

void CodeGeneratorVisitor::Visit(BoundMethodCallNode& node)
{
    node.Load(&emitter);
}

void CodeGeneratorVisitor::Visit(BoundNewExpressionNode& node)
{
    node.Load(&emitter);
}

void CodeGeneratorVisitor::Visit(BoundNewArrayExpressionNode& node)
{
    node.Load(&emitter);
}

void CodeGeneratorVisitor::Visit(BoundMemberExprNode& node)
{
    node.Load(&emitter);
}

void CodeGeneratorVisitor::Visit(BoundIndexExprNode& node) 
{
    node.Load(&emitter);
}

void CodeGeneratorVisitor::Visit(BoundEmptyStatementNode& node)
{
    emitter.Emit(new NoOperationInstruction());
}

void CodeGeneratorVisitor::Visit(BoundCompoundStatementNode& node)
{
    bool basicBlockPushed = false;
    BasicBlock* b = emitter.CurrentBasicBlock();
    if (!b)
    {
        b = emitter.GetCurrentSubroutine()->AddBasicBlock();
        emitter.PushBasicBlock(b);
        basicBlockPushed = true;
    }
    if (level == 0)
    {
        ReceiveInstruction* receiveInstruction = new ReceiveInstruction();
        receiveInstruction->SetCount(subroutine->ParameterCount());
        emitter.Emit(receiveInstruction);
    }
    ++level;
    for (const auto& boundStatement : node.BoundStatements())
    {
        boundStatement->Accept(*this);
    }
    --level;
    if (emitter.CurrentBasicBlock()->IsEmpty())
    {
        emitter.Emit(new NoOperationInstruction());
    }
    if (basicBlockPushed)
    {
        emitter.PopBasicBlock();
    }
}

void CodeGeneratorVisitor::Visit(BoundAssignmentStatementNode& node)
{
    node.Source()->Load(&emitter);
    node.Target()->Store(&emitter);
}

void CodeGeneratorVisitor::Visit(BoundProcedureCallStatementNode& node)
{
    for (const auto& argument : node.Arguments())
    {
        argument->Load(&emitter);
    }
    Procedure* procedure = node.GetProcedure();
    if (procedure->IsExternal())
    {
        ExternalSubroutine* subroutine = GetExternalSubroutine(procedure->ExternalSubroutineName());
        CallExternalInstruction* callExternalInstruction = new CallExternalInstruction();
        if (subroutine->Id() == -1)
        {
            ThrowError("subroutine ID not set in external procedure call '" + subroutine->FullName() + "'", emitter.Lexer(), node.Pos());
        }
        callExternalInstruction->SetId(subroutine->Id());
        emitter.Emit(callExternalInstruction);
    }
    else if (procedure->IsRegularProcedure())
    {
        CallProcedureInstruction* callProcedureInstruction = new CallProcedureInstruction();
        if (procedure->ModuleId() == -1)
        {
            ThrowError("module ID not set in procedure call '" + procedure->FullName() + "'", emitter.Lexer(), node.Pos());
        }
        callProcedureInstruction->SetModuleId(procedure->ModuleId());
        if (procedure->IsDeclaration())
        {
            if (procedure->ImplementationId() == -1)
            {
                ThrowError("implemenation ID not set in procedure call '" + procedure->FullName() + "'", emitter.Lexer(), node.Pos());
            }
            callProcedureInstruction->SetSubroutineId(procedure->ImplementationId());
        }
        else
        {
            if (procedure->Id() == -1)
            {
                ThrowError("procedure ID not set in procedure call '" + procedure->FullName() + "'", emitter.Lexer(), node.Pos());
            }
            callProcedureInstruction->SetSubroutineId(procedure->Id());
        }
        emitter.Emit(callProcedureInstruction);
    }
    else if (procedure->IsStandardProcedure())
    {
        StandardProcedure* stdProc = static_cast<StandardProcedure*>(procedure);
        CallStdProcInstruction* callStdProcInstruction = new CallStdProcInstruction();
        if (stdProc->Id() == -1)
        {
            ThrowError("standard procedure ID not set in procedure call '" + procedure->FullName() + "'", emitter.Lexer(), node.Pos());
        }
        callStdProcInstruction->SetStdProcId(stdProc->Id());
        callStdProcInstruction->SetArgumentCount(node.Arguments().size());
        emitter.Emit(callStdProcInstruction);
    }
}

void CodeGeneratorVisitor::Visit(BoundExpressionStatementNode& node)
{
    node.BoundExpression()->Load(&emitter);
    if (node.BoundExpression()->IsBoundMethodCallNode())
    {
        BoundMethodCallNode* methodCall = static_cast<BoundMethodCallNode*>(node.BoundExpression());
        if (methodCall->GetMethod()->IsFunction())
        {
            emitter.Emit(new PopValueInstruction());
        }
    }
    else
    {
        emitter.Emit(new PopValueInstruction());
    }
}

void CodeGeneratorVisitor::Visit(BoundIfStatementNode& node)
{
    node.Condition()->Load(&emitter);
    BranchInstruction* branchInstruction = new BranchInstruction();
    emitter.Emit(branchInstruction);
    BasicBlock* t = emitter.GetCurrentSubroutine()->AddBasicBlock();
    emitter.PushBasicBlock(t);
    node.ThenStatement()->Accept(*this);
    branchInstruction->SetTrueNext(t->First());
    JumpInstruction* jumpThenNext = new JumpInstruction();
    emitter.Emit(jumpThenNext);
    emitter.PopBasicBlock();
    JumpInstruction* jumpElseNext = nullptr;
    if (node.ElseStatement())
    {
        BasicBlock* e = emitter.GetCurrentSubroutine()->AddBasicBlock();
        emitter.PushBasicBlock(e);
        node.ElseStatement()->Accept(*this);
        branchInstruction->SetFalseNext(e->First());
        jumpElseNext = new JumpInstruction();
        emitter.Emit(jumpElseNext);
        emitter.PopBasicBlock();
    }
    BasicBlock* r = emitter.GetCurrentSubroutine()->AddBasicBlock();
    emitter.PushBasicBlock(r);
    emitter.EnqueuePatch(new JumpNextPatch(r, jumpThenNext));
    if (jumpElseNext)
    {
        emitter.EnqueuePatch(new JumpNextPatch(r, jumpElseNext));
    }
    else
    {
        emitter.EnqueuePatch(new BranchFalsePatch(r, branchInstruction));
    }
}

void CodeGeneratorVisitor::Visit(BoundWhileStatementNode& node)
{
    JumpInstruction* jumpInstruction = new JumpInstruction();
    emitter.Emit(jumpInstruction);
    BasicBlock* p = emitter.GetCurrentSubroutine()->AddBasicBlock();
    emitter.PushBasicBlock(p);
    node.Condition()->Load(&emitter);
    jumpInstruction->SetTarget(p->First());
    BranchInstruction* branchInstruction = new BranchInstruction();
    emitter.Emit(branchInstruction);
    emitter.PopBasicBlock();
    BasicBlock* q = emitter.GetCurrentSubroutine()->AddBasicBlock();
    emitter.PushBasicBlock(q);
    node.Statement()->Accept(*this);
    branchInstruction->SetTrueNext(q->First());
    JumpInstruction* jumpBackInstruction = new JumpInstruction();
    emitter.Emit(jumpBackInstruction);
    jumpBackInstruction->SetTarget(p->First());
    emitter.PopBasicBlock();
    BasicBlock* r = emitter.GetCurrentSubroutine()->AddBasicBlock();
    emitter.PushBasicBlock(r);
    emitter.EnqueuePatch(new BranchFalsePatch(r, branchInstruction));
}

void CodeGeneratorVisitor::Visit(BoundRepeatStatementNode& node)
{
    BasicBlock* p = emitter.GetCurrentSubroutine()->AddBasicBlock();
    emitter.PushBasicBlock(p);
    for (const auto& statement : node.Statements())
    {
        statement->Accept(*this);
    }
    node.Condition()->Load(&emitter);
    BranchInstruction* branchInstruction = new BranchInstruction();
    emitter.Emit(branchInstruction);
    branchInstruction->SetFalseNext(p->First());
    emitter.PopBasicBlock();
    BasicBlock* r = emitter.GetCurrentSubroutine()->AddBasicBlock();
    emitter.PushBasicBlock(r);
    emitter.EnqueuePatch(new BranchTruePatch(r, branchInstruction));
}

void CodeGeneratorVisitor::Visit(BoundReturnFunctionResultStatementNode& node)
{
    node.FunctionResult()->Load(&emitter);
}

void GenerateCode(Subroutine* subroutine, Block* block, BoundCompoundStatementNode* boundCompoundStatementNode, soul::lexer::LexerBase<char>& lexer, int64_t pos, Context* context)
{
    CodeGeneratorVisitor visitor(subroutine, block,  lexer, pos, context);
    boundCompoundStatementNode->Accept(visitor);
}

} // namespace p
