// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

export module springpp.operation;

import springpp.primitive;
import springpp.command;
import wing;
import std.core;

export namespace springpp {

class Diagram;
class RelationshipElement;
class ResizeHandle;
class MouseArgs;
class RectangleSelection;

enum class OperationKind
{
    emptyOperation, moveContainerElementsOperation, addRelationshipOperation, createRectangleSelectionOperation, resizeRectangleSelectionOperation, grabDiagramOperation
};

class Operation
{
public:
    Operation(OperationKind kind_, Diagram* diagram_, const wing::PointF& startLocation_);
    Operation(OperationKind kind_, Diagram* diagram_);
    virtual ~Operation();
    virtual bool IsEmpty() const { return IsEmptyOperation(); }
    virtual void AdvanceTo(MouseArgs& mouseArgs) {}
    virtual void Commit(MouseArgs& mouseArgs) {}
    virtual void Cancel() {}
    virtual void Draw(wing::Graphics& graphics) {}
    virtual void AddElementIndex(int index) {}
    virtual void SetElementIndeces(std::vector<int>&& elementIndeces) {}
    virtual void FetchOriginalLocations() {}
    OperationKind Kind() const { return kind; }
    Diagram* GetDiagram() const { return diagram; }
    const wing::PointF& StartLocation() const { return startLocation; }
    bool IsEmptyOperation() const { return kind == OperationKind::emptyOperation; }
    bool IsMoveContainerElementsOperation() const { return kind == OperationKind::moveContainerElementsOperation; }
    bool IsAddRelationshipOperation() const { return kind == OperationKind::addRelationshipOperation; }
    bool IsCreateRectangleSelectionOperation() const { return kind == OperationKind::createRectangleSelectionOperation; }
    bool IsResizeRectangleSelectionOperation() const { return kind == OperationKind::resizeRectangleSelectionOperation; }
    bool IsGrabDiagramOperation() const { return kind == OperationKind::grabDiagramOperation; }
private:
    OperationKind kind;
    Diagram* diagram;
    wing::PointF startLocation;
};

class EmptyOperation : public Operation
{
public:
    EmptyOperation(Diagram* diagram_);
};

class MoveContainerElementsOperation : public Operation
{
public:
    MoveContainerElementsOperation(Diagram* diagram_, const wing::PointF& startLocation_);
    bool IsEmpty() const override;
    void AddElementIndex(int index) override;
    void SetElementIndeces(std::vector<int>&& elementIndeces_) override;
    void AdvanceTo(MouseArgs& args) override;
    void Commit(MouseArgs& args) override;
    void Cancel() override;
    void FetchOriginalLocations() override;
    void ResetSelection();
private:
    std::vector<int> elementIndeces;
    std::vector<CompoundLocation> originalLocations;
    bool resetSelection;
    std::unique_ptr<MoveCommand> moveCommand;

};

class AddRelationshipOperation : public Operation
{
public:
    AddRelationshipOperation(Diagram* diagram_, RelationshipElement* relationshipElement_);
    bool IsEmpty() const override { return false; }
    void AdvanceTo(MouseArgs& args) override;
    void Draw(wing::Graphics& graphics) override;
    void Commit(MouseArgs& args) override;
    void Cancel();
    RelationshipElement* GetRelationshipElement() const { return relationshipElement.get(); }
private:
    std::unique_ptr<RelationshipElement> relationshipElement;
};

class CreateRectangleSelectionOperation : public Operation
{
public:
    CreateRectangleSelectionOperation(Diagram* diagram_, const wing::PointF& startLocation_);
    bool IsEmpty() const override;
    void AdvanceTo(MouseArgs& args) override;
    void Commit(MouseArgs& mouseArgs) override;
private:
    RectangleSelection* selection;
};

class ResizeRectangleSelectionOperation : public Operation
{
public:
    ResizeRectangleSelectionOperation(Diagram* diagram_, const wing::PointF& startLocation_, ResizeHandle* resizeHandle_);
    bool IsEmpty() const override;
    void AdvanceTo(MouseArgs& args) override;
    void Commit(MouseArgs& args) override;
    void Cancel() override;
    wing::Cursor* GetCursor() const;
private:
    ResizeHandle* resizeHandle;
};

class GrabDiagramOperation : public Operation
{
public:
    GrabDiagramOperation(Diagram* diagram_, const wing::PointF& startLocation_);
    void AdvanceTo(MouseArgs& args) override;
private:
    wing::PointF startViewOrigin;
};

} // namespace springpp
