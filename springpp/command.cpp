// =================================
// Copyright (c) 2024 Seppo Laakko
// Distributed under the MIT license
// =================================

module springpp.command;

import springpp.diagram_element;
import springpp.container_element;
import springpp.relationship_element;
import springpp.diagram;

namespace springpp {

struct DecreasingIndexOrder
{
    bool operator()(int left, int right) const
    {
        return left > right;
    }
};

struct DecreasingElementIndexPairOrder
{
    bool operator()(const ElementIndex& left, const ElementIndex& right) const
    {
        return left.Index() > right.Index();
    }
};

Command::Command(Diagram* diagram_) : diagram(diagram_)
{
}

Command::~Command()
{
}

ElementIndex::ElementIndex(DiagramElement* element_, int index_) : element(element_), index(index_)
{
}

void ElementIndex::Set(std::unique_ptr<DiagramElement>&& element_)
{
    element = std::move(element_);
}

AddElementsCommand::AddElementsCommand(Diagram* diagram_) : Command(diagram_)
{
}

void AddElementsCommand::AddIndex(int index)
{
    indeces.push_back(index);
}

void AddElementsCommand::Finalize()
{
    std::sort(indeces.begin(), indeces.end(), DecreasingIndexOrder());
}

void AddElementsCommand::Undo()
{
    Diagram* diagram = GetDiagram();
    for (int index : indeces)
    {
        std::unique_ptr<DiagramElement> element = diagram->RemoveElementByIndex(index);
        addedElements.push_back(ElementIndex(element.release(), index));
    }
}

void AddElementsCommand::Redo()
{
    Diagram* diagram = GetDiagram();
    int n = addedElements.size();
    for (int i = n - 1; i >= 0; --i)
    {
        ElementIndex& addedElement = addedElements[i];
        addedElement.Get()->ResetSelected();
        int index = addedElement.Index();
        diagram->InsertElement(addedElement.Release(), index);
    }
}

DeleteElementsCommand::DeleteElementsCommand(Diagram* diagram_) : Command(diagram_)
{
}

void DeleteElementsCommand::AddDeletedElement(DiagramElement* element, int index)
{
    deletedElements.push_back(ElementIndex(element, index));
}

void DeleteElementsCommand::Finalize()
{
    std::sort(deletedElements.begin(), deletedElements.end(), DecreasingElementIndexPairOrder());
}

void DeleteElementsCommand::Undo()
{
    Diagram* diagram = GetDiagram();
    int n = deletedElements.size();
    for (int i = n - 1; i >= 0; --i)
    {
        ElementIndex& deletedElement = deletedElements[i];
        deletedElement.Get()->ResetSelected();
        diagram->InsertElement(deletedElement.Release(), deletedElement.Index());
    }
}

void DeleteElementsCommand::Redo()
{
    Diagram* diagram = GetDiagram();
    for (auto& deleted : deletedElements)
    {
        deleted.Set(diagram->RemoveElementByIndex(deleted.Index()));
    }
}

ReplaceElementCommand::ReplaceElementCommand(Diagram* diagram_, int elementIndex_) : Command(diagram_), elementIndex(elementIndex_)
{
    Diagram* diagram = GetDiagram();
    element.reset(diagram->ReleaseElementByIndex(elementIndex));
}

void ReplaceElementCommand::Undo()
{
    Diagram* diagram = GetDiagram();
    std::unique_ptr<DiagramElement> newElement(diagram->ReleaseElementByIndex(elementIndex));
    if (element->IsContainerElement())
    {
        ContainerElement* containerElement = static_cast<ContainerElement*>(element.get());
        std::map<DiagramElement*, DiagramElement*> cloneMap;
        std::map<DiagramElement*, DiagramElement*> reverseCloneMap;
        cloneMap[newElement.get()] = containerElement;
        containerElement->MapChildObjects(static_cast<ContainerElement*>(newElement.get()), cloneMap, reverseCloneMap);
    }
    element->ResetSelected();
    diagram->SetElementByIndex(element.release(), elementIndex);
    element = std::move(newElement);
}

void ReplaceElementCommand::Redo()
{
    Diagram* diagram = GetDiagram();
    std::unique_ptr<DiagramElement> oldElement(diagram->ReleaseElementByIndex(elementIndex));
    if (element->IsContainerElement())
    {
        ContainerElement* containerElement = static_cast<ContainerElement*>(element.get());
        std::map<DiagramElement*, DiagramElement*> cloneMap;
        std::map<DiagramElement*, DiagramElement*> reverseCloneMap;
        cloneMap[oldElement.get()] = containerElement;
        containerElement->MapChildObjects(static_cast<ContainerElement*>(oldElement.get()), cloneMap, reverseCloneMap);
    }
    element->ResetSelected();
    diagram->SetElementByIndex(element.release(), elementIndex);
    element = std::move(oldElement);
}

MoveCommand::MoveCommand(Diagram* diagram_) : Command(diagram_)
{
}

void MoveCommand::AddOldLocation(int index, const CompoundLocation& oldLocation)
{
    indeces.push_back(index);
    oldLocations.push_back(oldLocation);
}

void MoveCommand::AddNewLocation(const CompoundLocation& newLocation)
{
    newLocations.push_back(newLocation);
}

void MoveCommand::Undo()
{
    Diagram* diagram = GetDiagram();
    int n = indeces.size();
    for (int i = 0; i < n; ++i)
    {
        int index = indeces[i];
        DiagramElement* element = diagram->GetElementByIndex(index);
        element->SetCompoundLocation(oldLocations[i]);
    }
}

void MoveCommand::Redo()
{
    Diagram* diagram = GetDiagram();
    int n = indeces.size();
    for (int i = 0; i < n; ++i)
    {
        int index = indeces[i];
        DiagramElement* element = diagram->GetElementByIndex(index);
        element->SetCompoundLocation(newLocations[i]);
    }
    newLocations.clear();
}

CommandList::CommandList() : currentIndex(0), commands(), undoMenuItem(nullptr), redoMenuItem(nullptr)
{
}

void CommandList::AddCommand(Command* command)
{
    command->Finalize();
    if (currentIndex >= commands.size())
    {
        commands.push_back(std::unique_ptr<Command>(command));
        currentIndex = static_cast<int>(commands.size());
    }
    else
    {
        commands[currentIndex].reset(command);
        ++currentIndex;
    }
    commands.resize(currentIndex);
    UpdateMenuItems();
}

void CommandList::SetMenuItems(wing::MenuItem* undoMenuItem_, wing::MenuItem* redoMenuItem_)
{
    undoMenuItem = undoMenuItem_;
    redoMenuItem = redoMenuItem_;
    UpdateMenuItems();
}

bool CommandList::CanUndo() const
{
    return currentIndex > 0;
}

void CommandList::Undo()
{
    if (CanUndo())
    {
        --currentIndex;
        Command* command = commands[currentIndex].get();
        command->Undo();
    }
    UpdateMenuItems();
}

bool CommandList::CanRedo() const
{
    return currentIndex < commands.size();
}

void CommandList::Redo()
{
    if (CanRedo())
    {
        Command* command = commands[currentIndex].get();
        command->Redo();
        ++currentIndex;
    }
    UpdateMenuItems();
}

void CommandList::UpdateMenuItems()
{
    if (CanUndo())
    {
        undoMenuItem->Enable();
    }
    else
    {
        undoMenuItem->Disable();
    }
    if (CanRedo())
    {
        redoMenuItem->Enable();
    }
    else
    {
        redoMenuItem->Disable();
    }
}

void CommandList::Clear()
{
    commands.clear();
    currentIndex = 0;
}

} // namespace springpp
