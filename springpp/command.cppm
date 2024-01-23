// =================================
// Copyright (c) 2024 Seppo Laakko
// Distributed under the MIT license
// =================================

export module springpp.command;

import springpp.primitive;
import wing;
import std.core;

export namespace springpp {

class Diagram;
class DiagramElement;
class RelationshipElement;

class Command
{
public:
    Command(Diagram* diagram_);
    virtual ~Command();
    Diagram* GetDiagram() { return diagram; }
    virtual void Finalize() {}
    virtual void Undo() = 0;
    virtual void Redo() = 0;
private:
    Diagram* diagram;
};

class ElementIndex
{
public:
    ElementIndex(DiagramElement* element_, int index_);
    DiagramElement* Get() const { return element.get(); }
    void Set(std::unique_ptr<DiagramElement>&& element_);
    DiagramElement* Release() { return element.release(); }
    int Index() const { return index; }
private:
    std::unique_ptr<DiagramElement> element;
    int index;
};

class AddElementsCommand : public Command
{
public:
    AddElementsCommand(Diagram* diagram_);
    void AddIndex(int index);
    void Finalize() override;
    void Undo() override;
    void Redo() override;
private:
    std::vector<int> indeces;
    std::vector<ElementIndex> addedElements;
};

class DeleteElementsCommand : public Command
{
public:
    DeleteElementsCommand(Diagram* diagram_);
    void AddDeletedElement(DiagramElement* element, int index);
    void Finalize() override;
    void Undo() override;
    void Redo() override;
private:
    std::vector<ElementIndex> deletedElements;
};

class ReplaceElementCommand : public Command
{
public:
    ReplaceElementCommand(Diagram* diagram_, int elementIndex_);
    void Undo() override;
    void Redo() override;
private:
    int elementIndex;
    std::unique_ptr<DiagramElement> element;
};

class MoveCommand : public Command
{
public:
    MoveCommand(Diagram* diagram_);
    void AddOldLocation(int index, const CompoundLocation& oldLocation);
    void AddNewLocation(const CompoundLocation& newLocation);
    void Undo() override;
    void Redo() override;
private:
    std::vector<int> indeces;
    std::vector<CompoundLocation> oldLocations;
    std::vector<CompoundLocation> newLocations;
};

class CommandList
{
public:
    CommandList();
    void AddCommand(Command* command);
    void SetMenuItems(wing::MenuItem* undoMenuItem_, wing::MenuItem* redoMenuItem_);
    bool CanUndo() const;
    void Undo();
    bool CanRedo() const;
    void Redo();
    void Clear();
private:
    void UpdateMenuItems();
    int currentIndex;
    std::vector<std::unique_ptr<Command>> commands;
    wing::MenuItem* undoMenuItem;
    wing::MenuItem* redoMenuItem;
};

} // namespace springpp
