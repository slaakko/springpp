// =================================
// Copyright (c) 2024 Seppo Laakko
// Distributed under the MIT license
// =================================

export module springpp.action;

import wing;
import std.core;

export namespace springpp {

class Diagram;

class ExecuteGuard
{
public:
    ExecuteGuard(Diagram* diagram_);
    ~ExecuteGuard();
private:
    Diagram* diagram;
};

class DiagramAction : public wing::ClickAction
{
public:
    DiagramAction(Diagram* diagram_, wing::MenuItem* menuItem);
    void Execute() override;
    virtual void Execute(Diagram* diagram) = 0;
private:
    Diagram* diagram;
};

class DiagramElementAction : public wing::ClickAction
{
public:
    DiagramElementAction(Diagram* diagram_, int elementIndex_, wing::MenuItem* menuItem_);
    void Execute() override;
    virtual void Execute(Diagram* diagram, int elementIndex) = 0;
private:
    Diagram* diagram;
    int elementIndex;
};

class SaveSelectionAsImageAction : public DiagramAction
{
public:
    SaveSelectionAsImageAction(Diagram* diagram_, wing::MenuItem* menuItem);
    void Execute(Diagram* diagram) override;
};

class AlignTopAction : public DiagramAction
{
public:
    AlignTopAction(Diagram* diagram_, wing::MenuItem* menuItem);
    void Execute(Diagram* diagram) override;
};

class AlignBottomAction : public DiagramAction
{
public:
    AlignBottomAction(Diagram* diagram_, wing::MenuItem* menuItem);
    void Execute(Diagram* diagram) override;
};

class AlignVerticalCenterAction : public DiagramAction
{
public:
    AlignVerticalCenterAction(Diagram* diagram_, wing::MenuItem* menuItem);
    void Execute(Diagram* diagram) override;
};

class AlignLeftSideAction : public DiagramAction
{
public:
    AlignLeftSideAction(Diagram* diagram_, wing::MenuItem* menuItem);
    void Execute(Diagram* diagram) override;
};

class AlignRightSideAction : public DiagramAction
{
public:
    AlignRightSideAction(Diagram* diagram_, wing::MenuItem* menuItem);
    void Execute(Diagram* diagram) override;
};

class AlignHorizontalCenterAction : public DiagramAction
{
public:
    AlignHorizontalCenterAction(Diagram* diagram_, wing::MenuItem* menuItem);
    void Execute(Diagram* diagram) override;
};

class SpaceEvenlyVerticallyAction : public DiagramAction
{
public:    
    SpaceEvenlyVerticallyAction(Diagram* diagram_, wing::MenuItem* menuItem);
    void Execute(Diagram* diagram) override;
};

class SpaceEvenlyHorizontallyAction : public DiagramAction
{
public:
    SpaceEvenlyHorizontallyAction(Diagram* diagram_, wing::MenuItem* menuItem);
    void Execute(Diagram* diagram) override;
};

class CombineInheritancesAction : public DiagramAction
{
public:
    CombineInheritancesAction(Diagram* diagram_, wing::MenuItem* menuItem);
    void Execute(Diagram* diagram) override;
};

class DeleteSelectionAction : public DiagramAction
{
public:
    DeleteSelectionAction(Diagram* diagram_, wing::MenuItem* menuItem);
    void Execute(Diagram* diagram) override;
};

class DeleteDiagramElementAction : public DiagramElementAction
{
public:
    DeleteDiagramElementAction(Diagram* diagram_, int elementIndex_, wing::MenuItem* menuItem_);
    void Execute(Diagram* diagram, int elementIndex) override;
};

class ClassElementPropertiesAction : public DiagramElementAction
{
public:
    ClassElementPropertiesAction(Diagram* diagram_, int elementIndex_, wing::MenuItem* menuItem_);
    void Execute(Diagram* diagram, int elementIndex) override;
};

class ObjectElementPropertiesAction : public DiagramElementAction
{
public:
    ObjectElementPropertiesAction(Diagram* diagram_, int elementIndex_, wing::MenuItem* menuItem_);
    void Execute(Diagram* diagram, int elementIndex) override;
};

class NoteElementPropertiesAction : public DiagramElementAction
{
public:
    NoteElementPropertiesAction(Diagram* diagram_, int elementIndex_, wing::MenuItem* menuItem_);
    void Execute(Diagram* diagram, int elementIndex) override;
};

class RelationshipElementPropertiesAction : public DiagramElementAction
{
public:
    RelationshipElementPropertiesAction(Diagram* diagram_, int elementIndex_, wing::MenuItem* menuItem_);
    void Execute(Diagram* diagram, int elementIndex) override;
};

class StraightenRelationshipElementAction : public DiagramElementAction
{
public:
    StraightenRelationshipElementAction(Diagram* diagram_, int elementIndex_, wing::MenuItem* menuItem_);
    void Execute(Diagram* diagram, int elementIndex) override;
};

class SetCenterConnectorsRelationshipElementAction : public DiagramElementAction
{
public:
    SetCenterConnectorsRelationshipElementAction(Diagram* diagram_, int elementIndex_, wing::MenuItem* menuItem_);
    void Execute(Diagram* diagram, int elementIndex) override;
};

class SplitRelationshipElementAction : public DiagramElementAction
{
public:    
    SplitRelationshipElementAction(Diagram* diagram_, int elementIndex_, wing::MenuItem* menuItem_);
    void Execute(Diagram* diagram, int elementIndex) override;
};

} // namespace springpp
