// =================================
// Copyright (c) 2024 Seppo Laakko
// Distributed under the MIT license
// =================================

export module springpp.selection;

import springpp.diagram_element;
import springpp.connector;
import springpp.layout;
import wing;
import std.core;

export namespace springpp {

class Diagram;
class MouseArgs;
class DiagramAction;

enum class SelectionKind
{
    emptySelection, elementSelection, rectangleSelection
};

class Selection
{
public:
    Selection(SelectionKind kind_, Diagram* diagram_);
    virtual ~Selection();
    SelectionKind Kind() const { return kind; }
    Diagram* GetDiagram() const { return diagram; }
    bool IsEmptySelection() const { return kind == SelectionKind::emptySelection; }
    bool IsElementSelection() const { return kind == SelectionKind::elementSelection; }
    bool IsRectangleSelection() const { return kind == SelectionKind::rectangleSelection; }
    virtual bool IsEmpty() const { return true; }
    virtual int Count() const { return 0; }
    virtual bool Contains(const wing::PointF& location) const { return false; }
    virtual int GetContainerElementCount() const { return 0; }
    virtual std::vector<int> GetContainerElementIndeces() const { return std::vector<int>(); }
    virtual void Clear() {}
    virtual void Delete() {}
    virtual void SetStartLocation(const wing::PointF& startLocation_) {}
    virtual void Move(MouseArgs& args) {}
    virtual std::vector<std::unique_ptr<DiagramElement>> Clone() const { return std::vector<std::unique_ptr<DiagramElement>>(); }
    virtual int GetFirstIndex() const { return -1; }
    virtual void Add(int index) {}
    virtual void Remove(int index) {}
    virtual void Draw(wing::Graphics& graphics) {}
    virtual void AddActions(wing::ContextMenu* contextMenu);
    std::u32string Copy();
    void SaveImage(const std::string& fileName, const Padding& margins, wing::ImageFormat imageFormat);
private:
    SelectionKind kind;
    Diagram* diagram;
};

class EmptySelection : public Selection
{
public:
    EmptySelection(Diagram* diagram_);
};

class ElementSelection : public Selection
{
public:
    ElementSelection(Diagram* diagram_);
    bool IsEmpty() const override;
    int Count() const override;
    bool Contains(const wing::PointF& location) const override;
    int GetContainerElementCount() const override;
    std::vector<int> GetContainerElementIndeces() const override;
    void Clear() override;
    void Delete() override;
    std::vector<std::unique_ptr<DiagramElement>> Clone() const override;
    int GetFirstIndex() const override;
    void Add(int index) override;
    void Remove(int index) override;
    void AddActions(wing::ContextMenu* contextMenu) override;
    void SortHorizontally();
    void SortVertically();
    void AlignTop();
    void AlignBottom();
    void AlignVerticalCenter();
    void AlignLeftSide();
    void AlignRightSide();
    void AlignHorizontalCenter();
    void SpaceEvenlyVertically();
    void SpaceEvenlyHorizontally();
    void CombineInheritanceRelationships();
private:
    std::vector<int> indeces;
};

class RectangleSelection;

class ResizeHandle
{
public:
    ResizeHandle(RectangleSelection* selection_, const wing::PointF& location_, const Connector& connector_);
    wing::Cursor* GetCursor() { return &cursor; }
    RectangleSelection* GetSelection() const { return selection; }
    bool Contains(const wing::PointF& loc) const;
    void Draw(wing::Graphics& graphics);
    void Locate(const wing::RectF& rect);
    void Move(float dx, float dy);
private:
    RectangleSelection* selection;
    wing::PointF location;
    wing::PointF startLocation;
    Connector connector;
    wing::Cursor cursor;
    float width;
};

class RectangleSelection : public Selection
{
public:
    RectangleSelection(Diagram* diagram_, const wing::PointF& startLocation_);
    bool IsEmpty() const override;
    int Count() const override;
    bool Contains(const wing::PointF& location) const override;
    int GetContainerElementCount() const override;
    std::vector<int> GetContainerElementIndeces() const override;
    void Clear() override;
    void Delete() override;
    void Move(MouseArgs& args) override;
    int GetFirstIndex() const override;
    std::vector<std::unique_ptr<DiagramElement>> Clone() const override;
    void AddActions(wing::ContextMenu* contextMenu) override;
    void Draw(wing::Graphics& graphics) override;
    wing::PointF Location() const { return wing::PointF(rect.X, rect.Y); }
    wing::SizeF GetSize() const { return wing::SizeF(rect.Width, rect.Height); }
    void SetSize(const wing::SizeF& size);
    const wing::PointF& StartLocation() const { return startLocation; }
    void SetStartLocation(const wing::PointF& startLocation_) override;
    const wing::RectF& Rect() const { return rect; }
    void SetRect(const wing::RectF& rect_);
    const wing::RectF& StartRect() const { return startRect; }
    wing::Cursor* GetCursor(const wing::PointF& location) const;
    ResizeHandle* GetResizeHandle(const wing::PointF& location) const;
private:
    void LocateResizeHandles();
    void DrawResizeHandles(wing::Graphics& graphics);
    wing::PointF startLocation;
    wing::RectF rect;
    wing::RectF startRect;
    std::vector<std::unique_ptr<ResizeHandle>> resizeHandles;
    wing::Cursor arrowCursor;
};

} // namespace springpp
