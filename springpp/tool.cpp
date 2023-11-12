// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

module springpp.tool;

import springpp.mouse_args;
import springpp.diagram;
import springpp.canvas;
import springpp.class_element;
import springpp.container_element;
import springpp.relationship_element;
import springpp.action;

namespace springpp {

Tool::Tool(ToolKind kind_) : kind(kind_), toolButton(nullptr), cursor(nullptr), pressedCursor(nullptr)
{
}

Tool::~Tool()
{
}

void Tool::SetToolButton(wing::ToolButton* toolButton_)
{
    toolButton = toolButton_;
    toolButton->Click().AddHandler(this, &Tool::ToolButtonClicked);
}

void Tool::SetCursor(wing::Cursor* cursor_)
{
    cursor = cursor_;
}

void Tool::SetPressedCursor(wing::Cursor* pressedCursor_)
{
    pressedCursor = pressedCursor_;
}

wing::Cursor* Tool::GetPressedCursor() const
{
    if (pressedCursor)
    {
        return pressedCursor;
    }
    else
    {
        return cursor;
    }
}

void Tool::ToolButtonClicked()
{
    Tools::Instance().SetCurrentToolInternal(this);
}

void Tool::MouseClick(MouseArgs& args)
{
    if (args.left)
    {
        BeforeLeftClick(args);
        if (args.shift)
        {
            MouseShiftLeftClick(args);
        }
        else if (args.control)
        {
            MouseControlLeftClick(args);
        }
        else
        {
            MouseNormalLeftClick(args);
        }
        AfterLeftClick(args);
    }
    else if (args.right)
    {
        BeforeRightClick(args);
        if (args.shift)
        {
            MouseShiftRightClick(args);
        }
        else if (args.control)
        {
            MouseControlRightClick(args);
        }
        else
        {
            MouseNormalRightClick(args);
        }
        AfterRightClick(args);
    }
}

SelectTool::SelectTool() : Tool(ToolKind::selectTool)
{
}

void SelectTool::Initialize(Diagram* diagram)
{
    diagram->ResetSelection();
    diagram->ResetOperation();
}

void SelectTool::BeforeLeftClick(MouseArgs& args)
{
    Diagram* diagram = GetDiagram();
    diagram->DisposeContextMenu();
}

void SelectTool::MouseNormalLeftClick(MouseArgs& args)
{
    Diagram* diagram = GetDiagram();
    if (args.element)
    {
        Selection* selection = diagram->GetSelection();
        if (selection->Count() == 1 && selection->Contains(args.location))
        {
            diagram->ResetSelection();
        }
        else
        {
            ElementSelection* elementSelection = new ElementSelection(diagram);
            elementSelection->Add(args.elementIndex);
            diagram->SetSelection(elementSelection);
        }
    }
    else
    {
        diagram->ResetSelection();
    }
}

void SelectTool::MouseControlLeftClick(MouseArgs& args)
{
    Diagram* diagram = GetDiagram();
    DiagramElement* element = args.element;
    if (element)
    {
        if (element->IsSelected())
        {
            diagram->GetSelection()->Remove(args.elementIndex);
        }
        else
        {
            diagram->GetSelection()->Add(args.elementIndex);
        }
    }
}

void SelectTool::MouseNormalRightClick(MouseArgs& args)
{
    Diagram* diagram = GetDiagram();
    std::unique_ptr<wing::ContextMenu> contextMenu(new wing::ContextMenu());
    Selection* selection = diagram->GetSelection();
    if (!selection->IsRectangleSelection() && selection->Count() <= 1)
    {
        if (selection->Count() == 1)
        {
            args.elementIndex = selection->GetFirstIndex();
            if (args.elementIndex != -1)
            {
                args.element = diagram->GetElementByIndex(args.elementIndex);
            }
            else
            {
                args.element = nullptr;
            }
        }
        if (args.element != nullptr)
        {
            if (args.element->Contains(args.location))
            {
                args.element->AddActions(diagram, args.elementIndex, contextMenu.get());
            }
            else
            {
                diagram->DisposeContextMenu();
            }
        }
        else
        {
            diagram->DisposeContextMenu();
        }
    }
    else
    {
        if (selection->Contains(args.location))
        {
            selection->AddActions(contextMenu.get());
        }
        else
        {
            diagram->DisposeContextMenu();
        }
    }
    if (contextMenu->HasMenuItems())
    {
        Canvas* canvas = GetCanvas();
        wing::Point loc = canvas->ToControlLocation(args.location);
        wing::Point screenLoc = canvas->ClientToScreen(loc);
        wing::Window* window = canvas->GetWindow();
        window->ShowContextMenu(contextMenu.release(), screenLoc);
    }
    else
    {
        diagram->DisposeContextMenu();
    }
}

void SelectTool::MouseDoubleClick(MouseArgs& args)
{
    Diagram* diagram = GetDiagram();
    Selection* selection = diagram->GetSelection();
    selection->Clear();
    DiagramElement* element = args.element;
    if (element)
    {
        if (element->IsClassElement())
        {
            ClassElementPropertiesAction action(diagram, args.elementIndex, nullptr);
            action.Execute(diagram, args.elementIndex);
        }
        else if (element->IsObjectElement())
        {
            ObjectElementPropertiesAction action(diagram, args.elementIndex, nullptr);
            action.Execute(diagram, args.elementIndex);
        }
        else if (element->IsNoteElement())
        {
            NoteElementPropertiesAction action(diagram, args.elementIndex, nullptr);
            action.Execute(diagram, args.elementIndex);
        }
        else if (element->IsRelationshipElement())
        {
            RelationshipElementPropertiesAction action(diagram, args.elementIndex, nullptr);
            action.Execute(diagram, args.elementIndex);
        }
    }
}

void SelectTool::CreateOperation(MouseArgs& args)
{
    Diagram* diagram = GetDiagram();
    ResizeHandle* resizeHandle = nullptr;
    bool leftButtonPressed = args.left && !args.right && !args.control && !args.shift;
    if (leftButtonPressed && diagram->GetSelection()->IsRectangleSelection())
    {
        RectangleSelection* selection = static_cast<RectangleSelection*>(diagram->GetSelection());
        wing::PointF location = args.location;
        resizeHandle = selection->GetResizeHandle(location);
    }
    if (resizeHandle)
    {
        std::unique_ptr<Operation> operation(new ResizeRectangleSelectionOperation(diagram, args.location, resizeHandle));
        diagram->SetOperation(operation.release());
    }
    else
    {
        std::unique_ptr<Operation> operation(new MoveContainerElementsOperation(diagram, args.location));
        Selection* selection = diagram->GetSelection();
        if (selection->Count() <= 1)
        {
            if (selection->Count() == 1)
            {
                args.elementIndex = selection->GetFirstIndex();
                if (args.elementIndex != -1)
                {
                    args.element = diagram->GetElementByIndex(args.elementIndex);
                }
                else
                {
                    args.element = nullptr;
                }
            }
            if (args.element != nullptr)
            {
                if (args.element->IsContainerElement())
                {
                    ContainerElement* containerElement = static_cast<ContainerElement*>(args.element);
                    if (containerElement->Contains(args.location))
                    {
                        operation->AddElementIndex(args.elementIndex);
                        std::vector<RelationshipElement*> relationships = containerElement->GetAllRelationships();
                        for (auto relationship : relationships)
                        {
                            DiagramElement* sourceElement = relationship->Source().Element();
                            ContainerElement* sourceContainerElement = nullptr;
                            if (sourceElement)
                            {
                                sourceContainerElement = sourceElement->GetContainerElement();
                            }
                            DiagramElement* targetElement = relationship->Target().Element();
                            ContainerElement* targetContainerElement = nullptr;
                            if (targetContainerElement)
                            {
                                targetContainerElement = targetElement->GetContainerElement();
                            }
                            if (sourceContainerElement == containerElement && targetContainerElement == containerElement)
                            {
                                int relationshipIndex = diagram->GetIndexOfElement(relationship);
                                if (relationshipIndex != -1)
                                {
                                    operation->AddElementIndex(relationshipIndex);
                                }
                            }
                            else if (targetElement == nullptr && relationship->IsReference() && relationship->GetCardinality() == Cardinality::zero)
                            {
                                int relationshipIndex = diagram->GetIndexOfElement(relationship);
                                if (relationshipIndex != -1)
                                {
                                    operation->AddElementIndex(relationshipIndex);
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                operation.reset(new CreateRectangleSelectionOperation(diagram, args.location));
                diagram->SetOperation(operation.release());
                return;
            }
        }
        else
        {
            if (selection->GetContainerElementCount() > 0)
            {
                if (selection->Contains(args.location))
                {
                    std::vector<int> elementIndeces = selection->GetContainerElementIndeces();
                    std::vector<int> relationshipIndeces;
                    for (int index : elementIndeces)
                    {
                        DiagramElement* element = diagram->GetElementByIndex(index);
                        if (element->IsContainerElement())
                        {
                            ContainerElement* containerElement = static_cast<ContainerElement*>(element);
                            std::vector<RelationshipElement*> containerElementRelationships = containerElement->GetAllRelationships();
                            for (auto relationship : containerElementRelationships)
                            {
                                bool sourceInSelection = false;
                                DiagramElement* sourceElement = relationship->Source().Element();
                                if (sourceElement)
                                {
                                    ContainerElement* sourceContainerElement = sourceElement->GetContainerElement();
                                    if (sourceContainerElement)
                                    {
                                        int sourceContainerElementIndex = diagram->GetIndexOfElement(sourceContainerElement);
                                        if (sourceContainerElementIndex != -1)
                                        {
                                            if (std::find(elementIndeces.begin(), elementIndeces.end(), sourceContainerElementIndex) != elementIndeces.end())
                                            {
                                                sourceInSelection = true;
                                            }
                                        }
                                    }
                                }
                                bool targetInSelection = false;
                                DiagramElement* targetElement = relationship->Target().Element();
                                if (targetElement)
                                {
                                    ContainerElement* targetContainerElement = targetElement->GetContainerElement();
                                    if (targetContainerElement)
                                    {
                                        int targetContainerElementIndex = diagram->GetIndexOfElement(targetContainerElement);
                                        if (targetContainerElementIndex != -1)
                                        {
                                            if (std::find(elementIndeces.begin(), elementIndeces.end(), targetContainerElementIndex) != elementIndeces.end())
                                            {
                                                targetInSelection = true;
                                            }
                                        }
                                    }
                                }
                                else if (relationship->IsReference() && relationship->GetCardinality() == Cardinality::zero)
                                {
                                    targetInSelection = true;
                                }
                                if (sourceInSelection && targetInSelection)
                                {
                                    int relationshipIndex = diagram->GetIndexOfElement(relationship);
                                    if (relationshipIndex != -1)
                                    {
                                        if (std::find(relationshipIndeces.begin(), relationshipIndeces.end(), relationshipIndex) == relationshipIndeces.end())
                                        {
                                            relationshipIndeces.push_back(relationshipIndex);
                                        }
                                    }
                                }
                            }
                        }
                    }
                    for (int relationshipIndex : relationshipIndeces)
                    {
                        elementIndeces.push_back(relationshipIndex);
                    }
                    operation->SetElementIndeces(std::move(elementIndeces));
                }
            }
        }
        operation->FetchOriginalLocations();
        if (!operation->IsEmpty())
        {
            diagram->SetOperation(operation.release());
        }
    }
}

void SelectTool::ContinueOperation(MouseArgs& args)
{
    Diagram* diagram = GetDiagram();
    diagram->AdvanceOperation(args);
}

void SelectTool::CommitOperation(MouseArgs& args)
{
    Diagram* diagram = GetDiagram();
    diagram->CommitOperation(args);
}

void SelectTool::CancelOperation(MouseArgs& args)
{
    Diagram* diagram = GetDiagram();
    diagram->CancelOperation();
}

GrabTool::GrabTool() : Tool(ToolKind::grabTool)
{
}

void GrabTool::CreateOperation(MouseArgs& args)
{
    Diagram* diagram = GetDiagram();
    if (args.left && !args.right && !args.control && !args.shift)
    {
        std::unique_ptr<Operation> operation(new GrabDiagramOperation(diagram, args.location));
        diagram->SetOperation(operation.release());
    }
}

void GrabTool::ContinueOperation(MouseArgs& args)
{
    Diagram* diagram = GetDiagram();
    diagram->AdvanceOperation(args);
}

void GrabTool::CommitOperation(MouseArgs& args)
{
    Diagram* diagram = GetDiagram();
    diagram->CommitOperation(args);
}

void GrabTool::CancelOperation(MouseArgs& args)
{
    Diagram* diagram = GetDiagram();
    diagram->CancelOperation();
}

SimpleClickTool::SimpleClickTool(ToolKind kind_) : Tool(kind_)
{
}

ClassTool::ClassTool() : SimpleClickTool(ToolKind::classTool)
{
}

void ClassTool::Initialize(Diagram* diagram)
{
    diagram->ResetSelection();
    diagram->ResetOperation();
}

void ClassTool::MouseNormalLeftClick(MouseArgs& args)
{
    Diagram* diagram = GetDiagram();
    diagram->ResetSelection();
    std::unique_ptr<ClassElement> classElement(new ClassElement());
    classElement->SetName("Class");
    classElement->SetLocation(args.location);
    std::unique_ptr<AddElementsCommand> addElementsCommand(new AddElementsCommand(diagram));
    addElementsCommand->AddIndex(diagram->NextIndex());
    diagram->AddElement(classElement.release());
    diagram->SetChanged();
    diagram->Invalidate();
    diagram->GetCommandList().AddCommand(addElementsCommand.release());
    Tools::Instance().SetCurrent(Tools::Instance().GetSelectTool());
}

ObjectTool::ObjectTool() : SimpleClickTool(ToolKind::objectTool)
{
}

void ObjectTool::Initialize(Diagram* diagram)
{
    diagram->ResetSelection();
    diagram->ResetOperation();
}

void ObjectTool::MouseNormalLeftClick(MouseArgs& args)
{
    // todo
}

NoteTool::NoteTool() : SimpleClickTool(ToolKind::noteTool)
{
}

void NoteTool::Initialize(Diagram* diagram)
{
    diagram->ResetSelection();
    diagram->ResetOperation();
}

void NoteTool::MouseNormalLeftClick(MouseArgs& args)
{
    // todo
}

RelationshipTool::RelationshipTool(ToolKind kind_) : SimpleClickTool(kind_)
{
}

void RelationshipTool::Initialize(Diagram* diagram)
{
    diagram->ResetSelection();
    diagram->ResetOperation();
}

bool RelationshipTool::Accept(DiagramElement* element, DiagramElement* sourceElement, EndPointKind endPointKind)
{
    return false;
}

void RelationshipTool::MouseNormalLeftClick(MouseArgs& args)
{
    Diagram* diagram = GetDiagram();
    diagram->ResetSelection();
    DiagramElement* element = args.element;
    if (!element)
    {
        Operation* currentOperation = diagram->GetOperation();
        if (currentOperation->IsAddRelationshipOperation())
        {
            AddRelationshipOperation* operation = static_cast<AddRelationshipOperation*>(currentOperation);
            RelationshipElement* relationshipElement = operation->GetRelationshipElement();
            relationshipElement->IntermediatePoints().push_back(args.location);
        }
        return;
    }
    DiagramElement* sourceElement = nullptr;
    EndPointKind endPointKind = EndPointKind::source;
    Operation* currentOperation = diagram->GetOperation();
    if (currentOperation->IsAddRelationshipOperation())
    {
        AddRelationshipOperation* operation = static_cast<AddRelationshipOperation*>(currentOperation);
        RelationshipElement* relationshipElement = operation->GetRelationshipElement();
        sourceElement = relationshipElement->Source().Element();
        if (relationshipElement->Target().Element() == nullptr)
        {
            endPointKind = EndPointKind::target;
        }
    }
    if (!Accept(element, sourceElement, endPointKind)) return;
    std::vector<EndPoint> endPoints = element->GetEndPoints(endPointKind, this);
    if (!endPoints.empty())
    {
        std::vector<const EndPoint*> endPointPtrs;
        for (const auto& endPoint : endPoints)
        {
            endPointPtrs.push_back(&endPoint);
        }
        std::sort(endPointPtrs.begin(), endPointPtrs.end(), EndPointNearer(args.location));
        const EndPoint* nearestEndPoint = endPointPtrs.front();
        if (currentOperation->IsEmptyOperation())
        {
            RelationshipKind relationshipKind = GetRelationshipKind();
            std::unique_ptr<RelationshipElement> relationshipElement(new RelationshipElement(relationshipKind));
            if (endPointKind == EndPointKind::source)
            {
                if (nearestEndPoint->Element() != nullptr)
                {
                    nearestEndPoint->Element()->AddRelationship(relationshipElement.get());
                }
            }
            relationshipElement->Source() = *nearestEndPoint;
            std::unique_ptr<AddRelationshipOperation> operation(new AddRelationshipOperation(diagram, relationshipElement.release()));
            diagram->SetOperation(operation.release());
        }
        else if (currentOperation->IsAddRelationshipOperation())
        {
            AddRelationshipOperation* operation = static_cast<AddRelationshipOperation*>(currentOperation);
            RelationshipElement* relationshipElement = operation->GetRelationshipElement();
            if (relationshipElement->Source().Element() == element && relationshipElement->IntermediatePoints().empty())
            {
                relationshipElement->Source() = *nearestEndPoint;
            }
            else if (relationshipElement->Target().Element() == nullptr)
            {
                if (args.control)
                {
                    wing::PointF lastPoint = relationshipElement->LastPoint();
                    wing::PointF nextPoint = nearestEndPoint->Point();
                    Line line(lastPoint, nextPoint);
                    Vector v(line.ToVector());
                    switch (MainDirection(v))
                    {
                        case 0:
                        case 180:
                        {
                            lastPoint.Y = nextPoint.Y;
                            break;
                        }
                        case 90:
                        case 270:
                        {
                            lastPoint.X = nextPoint.X;
                            break;
                        }
                    }
                    relationshipElement->SetLastPoint(lastPoint);
                }
                relationshipElement->Target() = *nearestEndPoint;
                nearestEndPoint->Element()->AddRelationship(relationshipElement);
                diagram->CommitOperation(args);
                Tools::Instance().SetCurrent(Tools::Instance().GetSelectTool());
            }
        }
    }
}

void RelationshipTool::MouseControlLeftClick(MouseArgs& args)
{
    // todo
}

void RelationshipTool::MouseShiftLeftClick(MouseArgs& args)
{
    // todo
}

ClassRelationshipTool::ClassRelationshipTool(ToolKind kind_) : RelationshipTool(kind_)
{
}

bool ClassRelationshipTool::Accept(DiagramElement* element, DiagramElement* sourceElement, EndPointKind endPointKind)
{
    if (element->IsClassElement()) 
    {
        return true;
    }
    else
    {
        return RelationshipTool::Accept(element, sourceElement, endPointKind);
    }
}

InheritanceTool::InheritanceTool() : ClassRelationshipTool(ToolKind::inheritanceTool)
{
}

CompositionTool::CompositionTool() : ClassRelationshipTool(ToolKind::compositionTool)
{
}

AggregationTool::AggregationTool() : ClassRelationshipTool(ToolKind::aggregationTool)
{
}

CreateInstanceTool::CreateInstanceTool() : ClassRelationshipTool(ToolKind::createInstanceTool)
{
}

AttachNoteTool::AttachNoteTool() : ClassRelationshipTool(ToolKind::attachNoteTool)
{
}

bool AttachNoteTool::Accept(DiagramElement* element, DiagramElement* sourceElement, EndPointKind endPointKind)
{
    if (endPointKind == EndPointKind::target)
    {
        if (element->IsNoteElement())
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return ClassRelationshipTool::Accept(element, sourceElement, endPointKind);
    }
}

ReferenceTool::ReferenceTool() : RelationshipTool(ToolKind::referenceTool)
{
}

bool ReferenceTool::Accept(DiagramElement* element, DiagramElement* sourceElement, EndPointKind endPointKind)
{
    if (sourceElement == nullptr)
    {
        if (element->IsClassElement() || element->IsObjectElement())
        {
            return true;
        }
    }
    else if (sourceElement->IsClassElement())
    {
        if (element->IsClassElement())
        {
            return true;
        }
    }
    else if (sourceElement->IsFieldElement())
    {
        if (element->IsObjectElement())
        {
            return true;
        }
    }
    return RelationshipTool::Accept(element, sourceElement, endPointKind);
}

Tools::Tools() : currentTool(&selectTool)
{
}

Tools& Tools::Instance()
{
    static Tools instance;
    return instance;
}

void Tools::SetCurrentToolInternal(Tool* tool)
{
    currentTool = tool;
}

void Tools::SetCurrent(Tool* tool)
{
    if (tool != currentTool)
    {
        currentTool = tool;
        currentTool->GetToolButton()->SetState(wing::ToolButtonState::pressed);
        currentTool->Initialize(GetDiagram());
    }
}

} // namespace springpp
