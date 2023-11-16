// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

export module springpp.diagram;

import springpp.command;
import springpp.operation;
import springpp.selection;
import springpp.index_list;
import springpp.diagram_element;
import springpp.mouse_args;
import springpp.layout;
import wing;
import std.core;

export namespace springpp {

class Canvas;
class DiagramElement;

enum class DiagramFlags
{
    none, changed = 1 << 0, invalidated = 1 << 1, modified = 1 << 2
};

constexpr DiagramFlags operator|(DiagramFlags left, DiagramFlags right)
{
    return DiagramFlags(int(left) | int(right));
}

constexpr DiagramFlags operator&(DiagramFlags left, DiagramFlags right)
{
    return DiagramFlags(int(left) & int(right));
}

constexpr DiagramFlags operator~(DiagramFlags flags)
{
    return DiagramFlags(~int(flags));
}

class Diagram
{
public:
    Diagram();
    void SetCanvas(Canvas* canvas_);
    Canvas* GetCanvas() const { return canvas; }
    CommandList& GetCommandList() { return commandList; }
    Operation* GetOperation() const { return operation.get(); }
    Selection* GetSelection() const { return selection.get(); }
    void Save(const std::string& filePath);
    void Load(const std::string& filePath);
    void Draw(wing::Graphics& graphics);
    void SetMenuItems(wing::MenuItem* undoMenuItem, wing::MenuItem* redoMenuItem);
    void SetSelection(Selection* selection_);
    void ResetSelection();
    void DeleteSelection();
    void SetOperation(Operation* operation_);
    void ResetOperation();
    void EnableCopy();
    void EnableCut();
    void DisableCopy();
    void DisableCut();
    void AdvanceOperation(MouseArgs& mouseArgs);
    void CommitOperation(MouseArgs& mouseArgs);
    void CancelOperation();
    void DisposeContextMenu();
    void HideContextMenu();
    bool Changed() const { return (flags & DiagramFlags::changed) != DiagramFlags::none; }
    void SetChanged();
    void ResetChanged();
    bool IsModified() const { return (flags & DiagramFlags::modified) != DiagramFlags::none; }
    void SetModified();
    void ResetModified();
    bool Invalidated() const { return (flags & DiagramFlags::invalidated) != DiagramFlags::none;}
    void Invalidate();
    void ResetInvalidated();
    void Undo();
    void Redo();
    int NextIndex() const { return elements.Count(); }
    DiagramElement* GetElementByIndex(int index) const;
    void SetElementByIndex(DiagramElement* element, int index);
    int GetIndexOfElement(DiagramElement* element) const;
    void AddElement(DiagramElement* element);
    std::unique_ptr<DiagramElement> RemoveElementByIndex(int index);
    void InsertElement(DiagramElement* element, int index);
    DiagramElement* ReleaseElementByIndex(int index);
    int GetIndexOfElementAt(const wing::PointF& location) const;
    bool HasIntersectingContainerElements(const wing::RectF& rect) const;
    int GetNumberOfIntersectingContainerElements(const wing::RectF& rect) const;
    std::vector<int> GetIntersectingContainerElementIndeces(const wing::RectF& rect) const;
    void Offset(float dx, float dy);
    wing::RectF CalculateBoundingRect() const;
    void SaveImage(const std::string& fileName, const Padding& margins, wing::ImageFormat format);
    void Paste(const std::u32string& clipboardData);
private:
    DiagramFlags flags;
    Canvas* canvas;
    CommandList commandList;
    std::unique_ptr<Operation> operation;
    std::unique_ptr<Selection> selection;
    IndexList<DiagramElement> elements;
};

} // namespace springpp
