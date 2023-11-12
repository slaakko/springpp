// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

module springpp.operation;

import springpp.canvas;
import springpp.selection;
import springpp.diagram;
import springpp.relationship_element;
import springpp.tool;

namespace springpp {

Operation::Operation(OperationKind kind_, Diagram* diagram_, const wing::PointF& startLocation_) : kind(kind_), diagram(diagram_), startLocation(startLocation_)
{
}

Operation::Operation(OperationKind kind_, Diagram* diagram_) : Operation(kind_, diagram_, wing::PointF())
{
}

Operation::~Operation()
{
}

EmptyOperation::EmptyOperation(Diagram* diagram_) : Operation(OperationKind::emptyOperation, diagram_)
{
}

MoveContainerElementsOperation::MoveContainerElementsOperation(Diagram* diagram_, const wing::PointF& startLocation_) : 
    Operation(OperationKind::moveContainerElementsOperation, diagram_, startLocation_), resetSelection(false), moveCommand(new MoveCommand(diagram_))
{
    Diagram* diagram = GetDiagram();
    Selection* selection = diagram->GetSelection();
    selection->SetStartLocation(StartLocation());
}

void MoveContainerElementsOperation::ResetSelection()
{
    resetSelection = true;
}

bool MoveContainerElementsOperation::IsEmpty() const
{
    return elementIndeces.empty();
}

void MoveContainerElementsOperation::AddElementIndex(int index)
{
    elementIndeces.push_back(index);
}

void MoveContainerElementsOperation::SetElementIndeces(std::vector<int>&& elementIndeces_) 
{
    elementIndeces = std::move(elementIndeces_);
}

void MoveContainerElementsOperation::AdvanceTo(MouseArgs& args)
{
    Diagram* diagram = GetDiagram();
    wing::PointF startLocation = StartLocation();
    wing::PointF nextLocation = args.location;
    if (args.control)
    {
        Line line(startLocation, nextLocation);
        Vector v(line.ToVector());
        switch (MainDirection(v))
        {
            case 0:
            case 180:
            {
                nextLocation.Y = startLocation.Y;
                break;
            }
            case 90:
            case 270:
            {
                nextLocation.X = startLocation.X;
                break;
            }
        }
    }
    float dx = nextLocation.X - startLocation.X;
    float dy = nextLocation.Y - startLocation.Y;
    int n = elementIndeces.size();
    for (int i = 0; i < n; ++i)
    {
        int elementIndex = elementIndeces[i];
        CompoundLocation newLocation = originalLocations[i];
        newLocation.Offset(dx, dy);
        DiagramElement* element = diagram->GetElementByIndex(elementIndex);
        element->SetCompoundLocation(newLocation);
    }
    diagram->GetSelection()->Move(args);
}

void MoveContainerElementsOperation::Commit(MouseArgs& args)
{
    Diagram* diagram = GetDiagram();
    AdvanceTo(args);
    if (resetSelection)
    {
        diagram->ResetSelection();
    }
    for (int index : elementIndeces)
    {
        DiagramElement* element = diagram->GetElementByIndex(index);
        moveCommand->AddNewLocation(element->GetCompoundLocation());
    }
    diagram->GetCommandList().AddCommand(moveCommand.release());
}

void MoveContainerElementsOperation::Cancel()
{
    Diagram* diagram = GetDiagram();
    int n = elementIndeces.size();
    for (int i = 0; i < n; ++i)
    {
        int elementIndex = elementIndeces[i];
        DiagramElement* element = diagram->GetElementByIndex(elementIndex);
        element->SetCompoundLocation(originalLocations[i]);
    }
}

void MoveContainerElementsOperation::FetchOriginalLocations()
{
    Diagram* diagram = GetDiagram();
    for (int elementIndex : elementIndeces)
    {
        DiagramElement* element = diagram->GetElementByIndex(elementIndex);
        CompoundLocation compoundLocation = element->GetCompoundLocation();
        originalLocations.push_back(compoundLocation);
        moveCommand->AddOldLocation(elementIndex, compoundLocation);
    }
}

AddRelationshipOperation::AddRelationshipOperation(Diagram* diagram_, RelationshipElement* relationshipElement_) : 
    Operation(OperationKind::addRelationshipOperation, diagram_), relationshipElement(relationshipElement_)
{
}

void AddRelationshipOperation::AdvanceTo(MouseArgs& args)
{
    wing::PointF lastPoint = relationshipElement->LastPoint();
    wing::PointF targetPoint = args.location;
    if (args.control)
    {
        Line line(lastPoint, targetPoint);
        Vector v(line.ToVector());
        switch (MainDirection(v))
        {
            case 0:
            case 180:
            {
                targetPoint.Y = lastPoint.Y;
                break;
            }
            case 90:
            case 270:
            {
                targetPoint.X = lastPoint.X;
                break;
            }
        }
    }
    relationshipElement->Target().Point() = targetPoint;
}

void AddRelationshipOperation::Draw(wing::Graphics& graphics)
{
    relationshipElement->Draw(graphics);
}

void AddRelationshipOperation::Commit(MouseArgs& args)
{
    relationshipElement->AddToElements();
    Diagram* diagram = GetDiagram();
    std::unique_ptr<AddElementsCommand> addElementsCommand(new AddElementsCommand(diagram));
    addElementsCommand->AddIndex(diagram->NextIndex());
    diagram->AddElement(relationshipElement.release());
    Tools::Instance().SetCurrent(Tools::Instance().GetSelectTool());
    diagram->GetCommandList().AddCommand(addElementsCommand.release());
}

void AddRelationshipOperation::Cancel()
{
    relationshipElement->RemoveFromElements();
}

CreateRectangleSelectionOperation::CreateRectangleSelectionOperation(Diagram* diagram_, const wing::PointF& startLocation_) : 
    Operation(OperationKind::createRectangleSelectionOperation, diagram_, startLocation_), selection(new RectangleSelection(diagram_, startLocation_))
{
    Diagram* diagram = GetDiagram();
    diagram->SetSelection(selection);
}

bool CreateRectangleSelectionOperation::IsEmpty() const
{
    return selection->IsEmpty();
}

void CreateRectangleSelectionOperation::AdvanceTo(MouseArgs& args)
{
    if (args.left && !args.right)
    {
        wing::SizeF size(args.location.X - selection->Location().X, args.location.Y - selection->Location().Y);
        selection->SetSize(size);
    }
}

void CreateRectangleSelectionOperation::Commit(MouseArgs& mouseArgs)
{
    if (!selection->IsEmpty())
    {
        Diagram* diagram = GetDiagram();
        diagram->EnableCopy();
        diagram->EnableCut();
    }
}

ResizeRectangleSelectionOperation::ResizeRectangleSelectionOperation(Diagram* diagram_, const wing::PointF& startLocation_, ResizeHandle* resizeHandle_) : 
    Operation(OperationKind::resizeRectangleSelectionOperation, diagram_, startLocation_), resizeHandle(resizeHandle_)
{
    Selection* selection = resizeHandle->GetSelection();
    selection->SetStartLocation(StartLocation());
}

void ResizeRectangleSelectionOperation::AdvanceTo(MouseArgs& args)
{
    if (!resizeHandle) return;
    wing::PointF currentLocation = args.location;
    wing::PointF startLocation = StartLocation();
    float dx = currentLocation.X - startLocation.X;
    float dy = currentLocation.Y - startLocation.Y;
    resizeHandle->Move(dx, dy);
}

void ResizeRectangleSelectionOperation::Commit(MouseArgs& args) 
{
    if (!resizeHandle) return;
    Diagram* diagram = GetDiagram();
    AdvanceTo(args);
    if (!resizeHandle->GetSelection()->IsEmpty())
    {
        diagram->EnableCopy();
        diagram->EnableCut();
    }
    else
    {
        diagram->DisableCopy();
        diagram->DisableCut();
    }
    resizeHandle = nullptr;
}

void ResizeRectangleSelectionOperation::Cancel()
{
    if (!resizeHandle) return;
    resizeHandle->GetSelection()->SetRect(resizeHandle->GetSelection()->StartRect());
    resizeHandle = nullptr;
}

bool ResizeRectangleSelectionOperation::IsEmpty() const
{
    return resizeHandle == nullptr;
}

wing::Cursor* ResizeRectangleSelectionOperation::GetCursor() const
{
    if (resizeHandle)
    {
        return resizeHandle->GetCursor();
    }
    return nullptr;
}

GrabDiagramOperation::GrabDiagramOperation(Diagram* diagram_, const wing::PointF& startLocation_) : 
    Operation(OperationKind::grabDiagramOperation, diagram_, startLocation_), startViewOrigin(GetCanvas()->ViewOrigin())
{
}

void GrabDiagramOperation::AdvanceTo(MouseArgs& args)
{
    if (args.left && !args.right)
    {
        wing::PointF startLoc = StartLocation();
        wing::PointF loc = args.location;
        float dx = loc.X - startLoc.X;
        float dy = loc.Y - startLoc.Y;
        wing::PointF viewOrigin(std::max(startViewOrigin.X - dx, 0.0f), std::max(startViewOrigin.Y - dy, 0.0f));
        Canvas* canvas = GetCanvas();
        canvas->SetViewOrigin(viewOrigin);
    }
}

} // namespace springpp
