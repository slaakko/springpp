// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

module springpp.action;

import springpp.diagram;
import springpp.diagram_util;
import springpp.class_properties_dialog;
import springpp.object_properties_dialog;
import springpp.note_properties_dialog;
import springpp.class_element;
import springpp.object_element;
import springpp.note_element;
import springpp.canvas;
import springpp.relationship_element;
import springpp.save_image_dialog;
import springpp.relationship_properties_dialog;
import springpp.container_element;

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
    Selection* selection = diagram->GetSelection();
    if (selection->IsElementSelection())
    {
        ElementSelection* elementSelection = static_cast<ElementSelection*>(selection);
        elementSelection->CombineInheritanceRelationships();
    }
}

DeleteSelectionAction::DeleteSelectionAction(Diagram* diagram_, wing::MenuItem* menuItem) : DiagramAction(diagram_, menuItem)
{
}

void DeleteSelectionAction::Execute(Diagram* diagram)
{
    diagram->GetSelection()->Delete();
    diagram->ResetSelection();
    diagram->SetChanged();
    diagram->Invalidate();
}

DeleteDiagramElementAction::DeleteDiagramElementAction(Diagram* diagram_, int elementIndex_, wing::MenuItem* menuItem_) : 
    DiagramElementAction(diagram_, elementIndex_, menuItem_)
{
}

void DeleteDiagramElementAction::Execute(Diagram* diagram, int elementIndex)
{
    std::unique_ptr<DeleteElementsCommand> deleteElementsCommand(new DeleteElementsCommand(diagram));
    std::unique_ptr<DiagramElement> element = diagram->RemoveElementByIndex(elementIndex);
    if (element->IsContainerElement())
    {
        ContainerElement* containerElement = static_cast<ContainerElement*>(element.get());
        std::vector<RelationshipElement*> containerElementRelationships = containerElement->GetAllRelationships();
        std::vector<RelationshipElement*> relationships;
        for (RelationshipElement* relationship : containerElementRelationships)
        {
            if (std::find(relationships.begin(), relationships.end(), relationship) == relationships.end())
            {
                relationships.push_back(relationship);
            }
        }
        std::vector<int> relationshipIndeces;
        for (RelationshipElement* relationship : relationships)
        {
            int index = diagram->GetIndexOfElement(relationship);
            if (index != -1)
            {
                if (std::find(relationshipIndeces.begin(), relationshipIndeces.end(), index) == relationshipIndeces.end())
                {
                    relationshipIndeces.push_back(index);
                }
            }
        }
        std::sort(relationshipIndeces.begin(), relationshipIndeces.end());
        int m = relationshipIndeces.size();
        for (int i = m - 1; i >= 0; --i)
        {
            int index = relationshipIndeces[i];
            std::unique_ptr<DiagramElement> element = diagram->RemoveElementByIndex(index);
            deleteElementsCommand->AddDeletedElement(element.release(), index);
        }
    }
    deleteElementsCommand->AddDeletedElement(element.release(), elementIndex);
    diagram->GetCommandList().AddCommand(deleteElementsCommand.release());
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
        ObjectElement* objectElement = static_cast<ObjectElement*>(element);
        std::unique_ptr<ObjectElement> clone(static_cast<ObjectElement*>(objectElement->Clone()));
        std::unique_ptr<ObjectPropertiesDialog> objectPropertiesDialog(new ObjectPropertiesDialog(clone.get()));
        Canvas* canvas = GetCanvas();
        wing::Window* window = canvas->GetWindow();
        diagram->HideContextMenu();
        wing::DialogResult result = objectPropertiesDialog->ShowDialog(*window);
        if (result == wing::DialogResult::ok)
        {
            std::unique_ptr<ReplaceElementCommand> replaceElementCommand(new ReplaceElementCommand(diagram, elementIndex));
            std::map<DiagramElement*, DiagramElement*> cloneMap;
            cloneMap[objectElement] = clone.get();
            clone->MapChildObjects(objectElement, cloneMap);
            std::vector<RelationshipElement*> relationships = objectElement->GetAllRelationships();
            for (RelationshipElement* relationship : relationships)
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

NoteElementPropertiesAction::NoteElementPropertiesAction(Diagram* diagram_, int elementIndex_, wing::MenuItem* menuItem_) :
    DiagramElementAction(diagram_, elementIndex_, menuItem_)
{
}

void NoteElementPropertiesAction::Execute(Diagram* diagram, int elementIndex)
{
    DiagramElement* element = diagram->GetElementByIndex(elementIndex);
    if (element->IsNoteElement())
    {
        NoteElement* noteElement = static_cast<NoteElement*>(element);
        std::unique_ptr<NoteElement> clone(static_cast<NoteElement*>(noteElement->Clone()));
        std::unique_ptr<NotePropertiesDialog> notePropertiesDialog(new NotePropertiesDialog(clone.get()));
        Canvas* canvas = diagram->GetCanvas();
        wing::Window* window = canvas->GetWindow();
        diagram->HideContextMenu();
        wing::DialogResult result = notePropertiesDialog->ShowDialog(*window);
        if (result == wing::DialogResult::ok)
        {
            std::unique_ptr<ReplaceElementCommand> replaceElementCommand(new ReplaceElementCommand(diagram, elementIndex));
            std::map<DiagramElement*, DiagramElement*> cloneMap;
            cloneMap[noteElement] = clone.get();
            clone->MapChildObjects(noteElement, cloneMap);
            std::vector<RelationshipElement*> relationships = noteElement->GetAllRelationships();
            for (RelationshipElement* relationship : relationships)
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
        Connector sourceConnector = relationshipElement->Source().GetConnector();
        DiagramElement* targetElement = relationshipElement->Target().Element();
        Connector targetConnector = relationshipElement->Target().GetConnector();
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
                EndPoint endPoint = relationshipElement->Source().Element()->GetEndPoint(clone->Source().GetConnector());
                clone->Source().SetPoint(endPoint.Point());
            }
            for (EndPoint& sourceEndPoint : relationshipElement->SourceEndPoints())
            {
                if (sourceEndPoint.Element() != nullptr)
                {
                    sourceEndPoint.Element()->RemoveRelationship(relationshipElement);
                    sourceEndPoint.Element()->AddRelationship(clone.get());
                    EndPoint endPoint = sourceEndPoint.Element()->GetEndPoint(clone->Source().GetConnector());
                    clone->Source().SetPoint(endPoint.Point());
                }
            }
            if (relationshipElement->Target().Element() != nullptr)
            {
                relationshipElement->Target().Element()->RemoveRelationship(relationshipElement);
                relationshipElement->Target().Element()->AddRelationship(clone.get());
                EndPoint endPoint = relationshipElement->Target().Element()->GetEndPoint(clone->Target().GetConnector());
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
    DiagramElement* diagramElement = diagram->GetElementByIndex(elementIndex);
    if (diagramElement->IsRelationshipElement())
    {
        RelationshipElement* relationship = static_cast<RelationshipElement*>(diagramElement);
        if (relationship->IsCombinedInheritance())
        {
            std::vector<std::unique_ptr<RelationshipElement>> relationships = SplitCombinedInheritanceRelationship(relationship);
            std::unique_ptr<DeleteElementsCommand> deleteCommand(new DeleteElementsCommand(diagram));
            std::unique_ptr<DiagramElement> element(diagram->RemoveElementByIndex(elementIndex));
            deleteCommand->AddDeletedElement(element.release(), elementIndex);
            diagram->GetCommandList().AddCommand(deleteCommand.release());
            std::unique_ptr<AddElementsCommand> addElementsCommand(new AddElementsCommand(diagram));
            for (std::unique_ptr<RelationshipElement>& relationship : relationships)
            {
                int addIndex = diagram->NextIndex();
                addElementsCommand->AddIndex(addIndex);
                diagram->AddElement(relationship.release());
            }
            diagram->GetCommandList().AddCommand(addElementsCommand.release());
        }
    }
}

} // namespace springpp
