// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

module springpp.action;

import springpp.diagram;
import springpp.class_properties_dialog;
import springpp.class_element;
import springpp.canvas;
import springpp.relationship_element;
import springpp.save_image_dialog;
import springpp.relationship_properties_dialog;

namespace springpp {

ExecuteGuard::ExecuteGuard(Diagram* diagram_) : diagram(diagram_)
{
}

ExecuteGuard::~ExecuteGuard()
{
    diagram->HideContextMenu();
    diagram->SetChanged();
    diagram->Invalidate();
}

DiagramAction::DiagramAction(Diagram* diagram_, wing::MenuItem* menuItem) : wing::ClickAction(menuItem), diagram(diagram_)
{
}

void DiagramAction::Execute()
{
    ExecuteGuard guard(diagram);
    Execute(diagram);
}

DiagramElementAction::DiagramElementAction(Diagram* diagram_, int elementIndex_, wing::MenuItem* menuItem_) : 
    wing::ClickAction(menuItem_), diagram(diagram_), elementIndex(elementIndex_)
{
}

void DiagramElementAction::Execute()
{
    ExecuteGuard guard(diagram);
    Execute(diagram, elementIndex);
}

SaveSelectionAsImageAction::SaveSelectionAsImageAction(Diagram* diagram_, wing::MenuItem* menuItem) : DiagramAction(diagram_, menuItem)
{
}

void SaveSelectionAsImageAction::Execute(Diagram* diagram)
{
    Canvas* canvas = GetCanvas();
    wing::Window* window = canvas->GetWindow();
    diagram->HideContextMenu();
    std::string diagramFileName;
    SaveImageDialog dialog(diagramFileName);
    if (dialog.ShowDialog(*window) == wing::DialogResult::ok)
    {
        Selection* selection = diagram->GetSelection();
        selection->SaveImage(dialog.FileName(), dialog.Margins(), dialog.Format());
    }
}

AlignTopAction::AlignTopAction(Diagram* diagram_, wing::MenuItem* menuItem) : DiagramAction(diagram_, menuItem)
{
}

void AlignTopAction::Execute(Diagram* diagram)
{
    Selection* selection = diagram->GetSelection();
    ElementSelection elementSelection(diagram);
    std::vector<int> containerElementIndeces = selection->GetContainerElementIndeces();
    for (auto index : containerElementIndeces)
    {
        elementSelection.Add(index);
    }
    elementSelection.AlignTop();
}

AlignBottomAction::AlignBottomAction(Diagram* diagram_, wing::MenuItem* menuItem) : DiagramAction(diagram_, menuItem)
{
}

void AlignBottomAction::Execute(Diagram* diagram)
{
    Selection* selection = diagram->GetSelection();
    ElementSelection elementSelection(diagram);
    std::vector<int> containerElementIndeces = selection->GetContainerElementIndeces();
    for (auto index : containerElementIndeces)
    {
        elementSelection.Add(index);
    }
    elementSelection.AlignBottom();
}

AlignVerticalCenterAction::AlignVerticalCenterAction(Diagram* diagram_, wing::MenuItem* menuItem) : DiagramAction(diagram_, menuItem)
{
}

void AlignVerticalCenterAction::Execute(Diagram* diagram)
{
    Selection* selection = diagram->GetSelection();
    ElementSelection elementSelection(diagram);
    std::vector<int> containerElementIndeces = selection->GetContainerElementIndeces();
    for (auto index : containerElementIndeces)
    {
        elementSelection.Add(index);
    }
    elementSelection.AlignVerticalCenter();
}

AlignLeftSideAction::AlignLeftSideAction(Diagram* diagram_, wing::MenuItem* menuItem) : DiagramAction(diagram_, menuItem)
{
}

void AlignLeftSideAction::Execute(Diagram* diagram)
{
    Selection* selection = diagram->GetSelection();
    ElementSelection elementSelection(diagram);
    std::vector<int> containerElementIndeces = selection->GetContainerElementIndeces();
    for (auto index : containerElementIndeces)
    {
        elementSelection.Add(index);
    }
    elementSelection.AlignLeftSide();
}

AlignRightSideAction::AlignRightSideAction(Diagram* diagram_, wing::MenuItem* menuItem) : DiagramAction(diagram_, menuItem)
{
}

void AlignRightSideAction::Execute(Diagram* diagram)
{
    Selection* selection = diagram->GetSelection();
    ElementSelection elementSelection(diagram);
    std::vector<int> containerElementIndeces = selection->GetContainerElementIndeces();
    for (auto index : containerElementIndeces)
    {
        elementSelection.Add(index);
    }
    elementSelection.AlignRightSide();
}

AlignHorizontalCenterAction::AlignHorizontalCenterAction(Diagram* diagram_, wing::MenuItem* menuItem) : DiagramAction(diagram_, menuItem)
{
}

void AlignHorizontalCenterAction::Execute(Diagram* diagram)
{
    Selection* selection = diagram->GetSelection();
    ElementSelection elementSelection(diagram);
    std::vector<int> containerElementIndeces = selection->GetContainerElementIndeces();
    for (auto index : containerElementIndeces)
    {
        elementSelection.Add(index);
    }
    elementSelection.AlignHorizontalCenter();
}

SpaceEvenlyVerticallyAction::SpaceEvenlyVerticallyAction(Diagram* diagram_, wing::MenuItem* menuItem) : DiagramAction(diagram_, menuItem)
{
}

void SpaceEvenlyVerticallyAction::Execute(Diagram* diagram)
{
    Selection* selection = diagram->GetSelection();
    ElementSelection elementSelection(diagram);
    std::vector<int> containerElementIndeces = selection->GetContainerElementIndeces();
    for (auto index : containerElementIndeces)
    {
        elementSelection.Add(index);
    }
    elementSelection.SpaceEvenlyVertically();
}

SpaceEvenlyHorizontallyAction::SpaceEvenlyHorizontallyAction(Diagram* diagram_, wing::MenuItem* menuItem) : DiagramAction(diagram_, menuItem)
{
}

void SpaceEvenlyHorizontallyAction::Execute(Diagram* diagram)
{
    Selection* selection = diagram->GetSelection();
    ElementSelection elementSelection(diagram);
    std::vector<int> containerElementIndeces = selection->GetContainerElementIndeces();
    for (auto index : containerElementIndeces)
    {
        elementSelection.Add(index);
    }
    elementSelection.SpaceEvenlyHorizontally();
}

CombineInheritancesAction::CombineInheritancesAction(Diagram* diagram_, wing::MenuItem* menuItem) : DiagramAction(diagram_, menuItem)
{
}

void CombineInheritancesAction::Execute(Diagram* diagram)
{
    // todo
}

DeleteSelectionAction::DeleteSelectionAction(Diagram* diagram_, wing::MenuItem* menuItem) : DiagramAction(diagram_, menuItem)
{
}

void DeleteSelectionAction::Execute(Diagram* diagram)
{
    // todo
}

DeleteDiagramElementAction::DeleteDiagramElementAction(Diagram* diagram_, int elementIndex_, wing::MenuItem* menuItem_) : 
    DiagramElementAction(diagram_, elementIndex_, menuItem_)
{
}

void DeleteDiagramElementAction::Execute(Diagram* diagram, int elementIndex)
{
    // todo
}

ClassElementPropertiesAction::ClassElementPropertiesAction(Diagram* diagram_, int elementIndex_, wing::MenuItem* menuItem_) : 
    DiagramElementAction(diagram_, elementIndex_, menuItem_)
{
}

void ClassElementPropertiesAction::Execute(Diagram* diagram, int elementIndex)
{
    DiagramElement* element = diagram->GetElementByIndex(elementIndex);
    if (element->IsClassElement())
    {
        ClassElement* classElement = static_cast<ClassElement*>(element);
        std::unique_ptr<ClassElement> clone(static_cast<ClassElement*>(classElement->Clone()));
        std::unique_ptr<ClassPropertiesDialog> classPropertiesDialog(new ClassPropertiesDialog(clone.get()));
        Canvas* canvas = GetCanvas();
        wing::Window* window = canvas->GetWindow();
        diagram->HideContextMenu();
        wing::DialogResult result = classPropertiesDialog->ShowDialog(*window);
        if (result == wing::DialogResult::ok)
        {
            if (!clone->IsAbstract())
            {
                clone->ResetAbstractOperations();
            }
            std::unique_ptr<ReplaceElementCommand> replaceElementCommand(new ReplaceElementCommand(diagram, elementIndex));
            std::map<DiagramElement*, DiagramElement*> cloneMap;
            cloneMap[classElement] = clone.get();
            clone->MapChildObjects(classElement, cloneMap);
            std::vector<RelationshipElement*> relationships = classElement->GetAllRelationships();
            for (auto relationship : relationships)
            {
                relationship->MapContainerElements(cloneMap);
                relationship->AddToElements();
            }
            diagram->SetElementByIndex(clone.release(), elementIndex);
            diagram->SetChanged();
            diagram->Invalidate();
            diagram->GetCommandList().AddCommand(replaceElementCommand.release());
        }
    }
}

ObjectElementPropertiesAction::ObjectElementPropertiesAction(Diagram* diagram_, int elementIndex_, wing::MenuItem* menuItem_) : 
    DiagramElementAction(diagram_, elementIndex_, menuItem_)
{
}

void ObjectElementPropertiesAction::Execute(Diagram* diagram, int elementIndex)
{
    DiagramElement* element = diagram->GetElementByIndex(elementIndex);
    if (element->IsObjectElement())
    {
        // todo
    }
}

NoteElementPropertiesAction::NoteElementPropertiesAction(Diagram* diagram_, int elementIndex_, wing::MenuItem* menuItem_) :
    DiagramElementAction(diagram_, elementIndex_, menuItem_)
{
}

void NoteElementPropertiesAction::Execute(Diagram* diagram, int elementIndex)
{
    DiagramElement* element = diagram->GetElementByIndex(elementIndex);
    if (element->IsNoteElement())
    {
        // todo
    }
}

RelationshipElementPropertiesAction::RelationshipElementPropertiesAction(Diagram* diagram_, int elementIndex_, wing::MenuItem* menuItem_) :
    DiagramElementAction(diagram_, elementIndex_, menuItem_)
{
}

void RelationshipElementPropertiesAction::Execute(Diagram* diagram, int elementIndex)
{
    DiagramElement* element = diagram->GetElementByIndex(elementIndex);
    if (element->IsRelationshipElement())
    {
        RelationshipElement* relationshipElement = static_cast<RelationshipElement*>(element);
        DiagramElement* sourceElement = relationshipElement->Source().Element();
        Snap sourceSnap = relationshipElement->Source().GetSnap();
        DiagramElement* targetElement = relationshipElement->Target().Element();
        Snap targetSnap = relationshipElement->Target().GetSnap();
        std::unique_ptr<RelationshipElement> clone(static_cast<RelationshipElement*>(relationshipElement->Clone()));
        std::unique_ptr<RelationshipPropertiesDialog> relationshipPropertiesDialog(new RelationshipPropertiesDialog(clone.get()));
        Canvas* canvas = diagram->GetCanvas();
        wing::Window* window = canvas->GetWindow();
        diagram->HideContextMenu();
        wing::DialogResult result = relationshipPropertiesDialog->ShowDialog(*window);
        if (result == wing::DialogResult::ok)
        {
            std::unique_ptr<ReplaceElementCommand> replaceElementCommand(new ReplaceElementCommand(diagram, elementIndex));
            if (relationshipElement->Source().Element() != nullptr)
            {
                relationshipElement->Source().Element()->RemoveRelationship(relationshipElement);
                relationshipElement->Source().Element()->AddRelationship(clone.get());
                EndPoint endPoint = relationshipElement->Source().Element()->GetEndPoint(relationshipElement->Source().GetSnap());
                clone->Source().SetSnap(endPoint.GetSnap());
                clone->Source().SetPoint(endPoint.Point());
            }
            for (EndPoint& sourceEndPoint : relationshipElement->SourceEndPoints())
            {
                if (sourceEndPoint.Element() != nullptr)
                {
                    sourceEndPoint.Element()->RemoveRelationship(relationshipElement);
                    sourceEndPoint.Element()->AddRelationship(clone.get());
                    EndPoint endPoint = sourceEndPoint.Element()->GetEndPoint(relationshipElement->Source().GetSnap());
                    clone->Source().SetSnap(endPoint.GetSnap());
                    clone->Source().SetPoint(endPoint.Point());
                }
            }
            if (relationshipElement->Target().Element() != nullptr)
            {
                relationshipElement->Target().Element()->RemoveRelationship(relationshipElement);
                relationshipElement->Target().Element()->AddRelationship(clone.get());
                EndPoint endPoint = relationshipElement->Target().Element()->GetEndPoint(relationshipElement->Target().GetSnap());
                clone->Target().SetSnap(endPoint.GetSnap());
                clone->Target().SetPoint(endPoint.Point());
            }
            diagram->SetElementByIndex(clone.release(), elementIndex);
            diagram->SetChanged();
            diagram->Invalidate();
            diagram->GetCommandList().AddCommand(replaceElementCommand.release());
        }
    }
}

StraightenRelationshipElementAction::StraightenRelationshipElementAction(Diagram* diagram_, int elementIndex_, wing::MenuItem* menuItem_) : 
    DiagramElementAction(diagram_, elementIndex_, menuItem_)
{
}

void StraightenRelationshipElementAction::Execute(Diagram* diagram, int elementIndex)
{
    DiagramElement* diagramElement = diagram->GetElementByIndex(elementIndex);
    if (diagramElement->IsRelationshipElement())
    {
        RelationshipElement* relationship = static_cast<RelationshipElement*>(diagramElement);
        relationship->Straighten();
    }
}

SplitRelationshipElementAction::SplitRelationshipElementAction(Diagram* diagram_, int elementIndex_, wing::MenuItem* menuItem_) :
    DiagramElementAction(diagram_, elementIndex_, menuItem_)
{
}

void SplitRelationshipElementAction::Execute(Diagram* diagram, int elementIndex)
{
    // todo
}

} // namespace springpp
