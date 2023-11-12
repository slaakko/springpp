// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

export module springpp.tool;

import springpp.diagram_element;
import springpp.mouse_args;
import wing;
import std.core;

export namespace springpp {

class Diagram;

enum class ToolKind
{
    selectTool, grabTool, classTool, objectTool, noteTool, inheritanceTool, compositionTool, aggregationTool, createInstanceTool, attachNoteTool, referenceTool
};

class Tool
{
public:
    Tool(ToolKind kind_);
    virtual ~Tool();
    ToolKind Kind() const { return kind; }
    bool IsSelectTool() const { return kind == ToolKind::selectTool; }
    bool IsGrabTool() const { return kind == ToolKind::grabTool; }
    bool IsClassTool() const { return kind == ToolKind::classTool; }
    bool IsObjectTool() const { return kind == ToolKind::objectTool; }
    bool IsNoteTool() const { return kind == ToolKind::noteTool; }
    bool IsInheritanceTool() const { return kind == ToolKind::inheritanceTool; }
    bool IsCompositionTool() const { return kind == ToolKind::compositionTool; }
    bool IsAggregationTool() const { return kind == ToolKind::aggregationTool; }
    bool IsCreateInstanceTool() const { return kind == ToolKind::createInstanceTool; }
    bool IsAttachNoteTool() const { return kind == ToolKind::attachNoteTool; }
    bool IsReferenceTool() const { return kind == ToolKind::referenceTool; }
    bool IsRelationshipTool() const 
    {
        switch (kind)
        {
            case ToolKind::inheritanceTool:
            case ToolKind::compositionTool:
            case ToolKind::aggregationTool:
            case ToolKind::createInstanceTool:
            case ToolKind::attachNoteTool:
            case ToolKind::referenceTool:
            {
                return true;
            }
        }
        return false;
    }
    void SetToolButton(wing::ToolButton* toolButton_);
    wing::ToolButton* GetToolButton() const { return toolButton; }
    void SetCursor(wing::Cursor* cursor_);
    wing::Cursor* GetCursor() const { return cursor; }
    void SetPressedCursor(wing::Cursor* pressedCursor_);
    wing::Cursor* GetPressedCursor() const;
    virtual bool CanMove() const { return false; }
    virtual bool IsSimpleClickTool() const { return false; }
    virtual void Initialize(Diagram* diagram) {}
    virtual void BeforeLeftClick(MouseArgs& args) {}
    virtual void AfterLeftClick(MouseArgs& args) {}
    virtual void BeforeRightClick(MouseArgs& args) {}
    virtual void AfterRightClick(MouseArgs& args) {}
    virtual void MouseClick(MouseArgs& args);
    virtual void MouseShiftLeftClick(MouseArgs& args) {}
    virtual void MouseControlLeftClick(MouseArgs& args) {}
    virtual void MouseNormalLeftClick(MouseArgs& args) {}
    virtual void MouseShiftRightClick(MouseArgs& args) {}
    virtual void MouseControlRightClick(MouseArgs& args) {}
    virtual void MouseNormalRightClick(MouseArgs& args) {}
    virtual void MouseDoubleClick(MouseArgs& args) {}
    virtual void CreateOperation(MouseArgs& args) {}
    virtual void ContinueOperation(MouseArgs& args) {}
    virtual void CancelOperation(MouseArgs& args) {}
    virtual void CommitOperation(MouseArgs& args) {}
private:
    void ToolButtonClicked();
    ToolKind kind;
    wing::ToolButton* toolButton;
    wing::Cursor* cursor;
    wing::Cursor* pressedCursor;
};

class SelectTool : public Tool
{
public:
    SelectTool();
    void Initialize(Diagram* diagram) override;
    bool CanMove() const override { return true; }
    void BeforeLeftClick(MouseArgs& args) override;
    void MouseNormalLeftClick(MouseArgs& args) override;
    void MouseControlLeftClick(MouseArgs& args) override;
    void MouseNormalRightClick(MouseArgs& args) override;
    void MouseDoubleClick(MouseArgs& args) override;
    void CreateOperation(MouseArgs& args) override;
    void ContinueOperation(MouseArgs& args) override;
    void CommitOperation(MouseArgs& args) override;
    void CancelOperation(MouseArgs& args) override;
};

class GrabTool : public Tool
{
public:
    GrabTool();
    bool CanMove() const override { return true; }
    void CreateOperation(MouseArgs& args) override;
    void ContinueOperation(MouseArgs& args) override;
    void CommitOperation(MouseArgs& args) override;
    void CancelOperation(MouseArgs& args) override;
};

class SimpleClickTool : public Tool
{
public:
    SimpleClickTool(ToolKind kind_);
    bool IsSimpleClickTool() const override { return true; }
};

class ClassTool : public SimpleClickTool
{
public:
    ClassTool();
    void Initialize(Diagram* diagram) override;
    void MouseNormalLeftClick(MouseArgs& args) override;
};

class ObjectTool : public SimpleClickTool
{
public:
    ObjectTool();
    void Initialize(Diagram* diagram) override;
    void MouseNormalLeftClick(MouseArgs& args) override;
};

class NoteTool : public SimpleClickTool
{
public:
    NoteTool();
    void Initialize(Diagram* diagram) override;
    void MouseNormalLeftClick(MouseArgs& args) override;
};

class RelationshipTool : public SimpleClickTool
{
public:
    RelationshipTool(ToolKind kind_);
    void Initialize(Diagram* diagram) override;
    virtual bool Accept(DiagramElement* element, DiagramElement* sourceElement, EndPointKind endPointKind);
    virtual RelationshipKind GetRelationshipKind() const = 0;
    void MouseNormalLeftClick(MouseArgs& args) override;
    void MouseControlLeftClick(MouseArgs& args) override;
    void MouseShiftLeftClick(MouseArgs& args) override;
};

class ClassRelationshipTool : public RelationshipTool
{
public:
    ClassRelationshipTool(ToolKind kind_);
    bool Accept(DiagramElement* element, DiagramElement* sourceElement, EndPointKind endpointKind) override;
};

class InheritanceTool : public ClassRelationshipTool
{
public:
    InheritanceTool();
    RelationshipKind GetRelationshipKind() const override { return RelationshipKind::inheritance; }
};

class CompositionTool : public ClassRelationshipTool
{
public:
    CompositionTool();
    RelationshipKind GetRelationshipKind() const override { return RelationshipKind::composition; }
};

class AggregationTool : public ClassRelationshipTool
{
public:
    AggregationTool();
    RelationshipKind GetRelationshipKind() const override { return RelationshipKind::aggregation; }
};

class CreateInstanceTool : public ClassRelationshipTool
{
public:
    CreateInstanceTool();
    RelationshipKind GetRelationshipKind() const override { return RelationshipKind::createInstance; }
};

class AttachNoteTool : public ClassRelationshipTool
{
public:
    AttachNoteTool();
    RelationshipKind GetRelationshipKind() const override { return RelationshipKind::attachNote; }
    bool Accept(DiagramElement* element, DiagramElement* sourceElement, EndPointKind endPointKind) override;
};

class ReferenceTool : public RelationshipTool
{
public:
    ReferenceTool();
    RelationshipKind GetRelationshipKind() const override { return RelationshipKind::reference; }
    bool Accept(DiagramElement* element, DiagramElement* sourceElement, EndPointKind endPointKind) override;
};

class Tools
{
public:
    static Tools& Instance();
    Tool* Current() const { return currentTool; }
    void SetCurrent(Tool* tool);
    void SetCurrentToolInternal(Tool* tool);
    SelectTool* GetSelectTool() { return &selectTool; }
    GrabTool* GetGrabTool() { return &grabTool; }
    ClassTool* GetClassTool() { return &classTool; }
    ObjectTool* GetObjectTool() { return &objectTool; }
    NoteTool* GetNoteTool() { return &noteTool; }
    InheritanceTool* GetInheritanceTool() { return &inheritanceTool; }
    CompositionTool* GetCompositionTool() { return &compositionTool; }
    AggregationTool* GetAggregationTool() { return &aggregationTool; }
    CreateInstanceTool* GetCreateInstanceTool() { return &createInstanceTool; }
    AttachNoteTool* GetAttachNoteTool() { return &attachNoteTool; }
    ReferenceTool* GetReferenceTool() { return &referenceTool; }
private:
    Tools();
    Tool* currentTool;
    SelectTool selectTool;
    GrabTool grabTool;
    ClassTool classTool;
    ObjectTool objectTool;
    NoteTool noteTool;
    InheritanceTool inheritanceTool;
    CompositionTool compositionTool;
    AggregationTool aggregationTool;
    CreateInstanceTool createInstanceTool;
    AttachNoteTool attachNoteTool;
    ReferenceTool referenceTool;
};

} // namespace springpp
