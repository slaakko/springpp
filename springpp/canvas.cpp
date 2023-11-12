// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

module;
#include <Windows.h>
#include <gdiplus.h>

module springpp.canvas;

import springpp.diagram;
import springpp.tool;

namespace springpp {

CanvasCreateParams& CanvasCreateParams::Defaults()
{
    WindowClassName("springpp.canvas");
    WindowClassStyle(wing::DoubleClickWindowClassStyle());
    WindowStyle(wing::DefaultChildWindowStyle());
    WindowClassBackgroundColor(COLOR_WINDOW);
    BackgroundColor(wing::Color::White);
    Text("canvas");
    Location(wing::DefaultLocation());
    SetSize(wing::DefaultSize());
    SetAnchors(wing::Anchors::left | wing::Anchors::top);
    SetDock(wing::Dock::fill);
    return *this;
}

CanvasCreateParams& CanvasCreateParams::WindowClassName(const std::string& windowClassName_)
{
    controlCreateParams.WindowClassName(windowClassName_);
    return *this;
}

CanvasCreateParams& CanvasCreateParams::WindowClassStyle(uint32_t windowClassStyle_)
{
    controlCreateParams.WindowClassStyle(windowClassStyle_);
    return *this;
}

CanvasCreateParams& CanvasCreateParams::WindowStyle(int windowStyle_)
{
    controlCreateParams.WindowStyle(windowStyle_);
    return *this;
}

CanvasCreateParams& CanvasCreateParams::WindowClassBackgroundColor(int windowClassBackgroundColor_)
{
    controlCreateParams.WindowClassBackgroundColor(windowClassBackgroundColor_);
    return *this;
}

CanvasCreateParams& CanvasCreateParams::BackgroundColor(const wing::Color& backgroundColor_)
{
    controlCreateParams.BackgroundColor(backgroundColor_);
    return *this;
}

CanvasCreateParams& CanvasCreateParams::Text(const std::string& text_)
{
    controlCreateParams.Text(text_);
    return *this;
}

CanvasCreateParams& CanvasCreateParams::Location(wing::Point location_)
{
    controlCreateParams.Location(location_);
    return *this;
}

CanvasCreateParams& CanvasCreateParams::SetSize(wing::Size size_)
{
    controlCreateParams.SetSize(size_);
    return *this;
}

CanvasCreateParams& CanvasCreateParams::SetAnchors(wing::Anchors anchors_)
{
    controlCreateParams.SetAnchors(anchors_);
    return *this;
}

CanvasCreateParams& CanvasCreateParams::SetDock(wing::Dock dock_)
{
    controlCreateParams.SetDock(dock_);
    return *this;
}

Canvas::Canvas(CanvasCreateParams& createParams) :
    wing::Control(createParams.controlCreateParams),
    noteCornerBitmap(nullptr),
    modifiedStatusBarItem(nullptr),
    copyMenuItem(nullptr),
    cutMenuItem(nullptr),
    pasteMenuItem(nullptr),
    undoMenuItem(nullptr),
    redoMenuItem(nullptr),
    operationCreated(false),
    buttonPressed(false)
{
    diagram.reset(new Diagram());
    diagram->SetCanvas(this);
}

void Canvas::SetDiagram(Diagram* diagram_)
{
    if (diagram)
    {
        diagram->GetCommandList().Clear();
    }
    diagram.reset(diagram_);
    diagram->SetMenuItems(undoMenuItem, redoMenuItem);
    diagram->SetCanvas(this);
    diagram->SetChanged();
    diagram->Invalidate();
    diagram->ResetModified();
    Invalidate();
}

void Canvas::SetNoteCornerBitmap(wing::Bitmap* noteCornerBitmap_)
{
    noteCornerBitmap = noteCornerBitmap_;
}

void Canvas::SetModifiedStatusBarItem(wing::StatusBarItem* modifiedStatusBarItem_)
{
    modifiedStatusBarItem = modifiedStatusBarItem_;
}

void Canvas::SetCopyMenuItem(wing::MenuItem* copyMenuItem_)
{
    copyMenuItem = copyMenuItem_;
}

void Canvas::SetCutMenuItem(wing::MenuItem* cutMenuItem_)
{
    cutMenuItem = cutMenuItem_;
}

void Canvas::SetPasteMenuItem(wing::MenuItem* pasteMenuItem_)
{
    pasteMenuItem = pasteMenuItem_;
}

void Canvas::SetUndoMenuItem(wing::MenuItem* undoMenuItem_)
{
    undoMenuItem = undoMenuItem_;
}

void Canvas::SetRedoMenuItem(wing::MenuItem* redoMenuItem_)
{
    redoMenuItem = redoMenuItem_;
}

void Canvas::SetViewOrigin(const wing::PointF& viewOrigin_)
{
    viewOrigin = viewOrigin_;
    Invalidate();
}

void Canvas::SetCursor()
{
    SetCursor(wing::PointF());
}

void Canvas::OnPaint(wing::PaintEventArgs& args)
{
    SetCursor();
    wing::Graphics& graphics = args.graphics;
    wing::Unit prevUnit = graphics.GetPageUnit();
    wing::TextRenderingHint prevTextRenderingHint = graphics.GetTextRenderingHint();
    graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);
    wing::SmoothingMode prevSmoothingMode = graphics.GetSmoothingMode();
    graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
    graphics.SetPageUnit(Gdiplus::UnitMillimeter);
    graphics.Clear(BackgroundColor());
    graphics.TranslateTransform(-viewOrigin.X, -viewOrigin.Y);
    diagram->Draw(graphics);
    graphics.SetSmoothingMode(prevSmoothingMode);
    graphics.SetTextRenderingHint(prevTextRenderingHint);
    graphics.SetPageUnit(prevUnit);
}

void Canvas::OnMouseDown(wing::MouseEventArgs& args)
{
    Control::OnMouseDown(args);
    wing::PointF location = ToDiagramLocation(args.location);
    DiagramElement* element = nullptr;
    int index = diagram->GetIndexOfElementAt(location);
    if (index != -1)
    {
        element = diagram->GetElementByIndex(index);
    }
    mouseDownArgs = MouseArgs(location, index, element, args);
    operationCreated = false;
    if ((args.buttons & wing::MouseButtons::lbutton) != wing::MouseButtons::none)
    {
        buttonPressed = true;
    }
    SetCursor(location);
}

void Canvas::OnMouseMove(wing::MouseEventArgs& args)
{
    Control::OnMouseMove(args);
    wing::PointF location = ToDiagramLocation(args.location);
    DiagramElement* element = nullptr;
    int index = -1;
    if ((args.buttons & (wing::MouseButtons::lbutton | wing::MouseButtons::rbutton)) != wing::MouseButtons::none)
    {
        index = diagram->GetIndexOfElementAt(location);
        if (index != -1)
        {
            element = diagram->GetElementByIndex(index);
        }
    }
    mouseMoveArgs = MouseArgs(location, index, element, args);
    Tool* currentTool = Tools::Instance().Current();
    if (currentTool->CanMove())
    {
        if (args.buttons == wing::MouseButtons::lbutton)
        {
            if (!operationCreated)
            {
                operationCreated = true;
                currentTool->CreateOperation(mouseDownArgs);
            }
            currentTool->ContinueOperation(mouseMoveArgs);
        }
    }
    else if (currentTool->IsRelationshipTool())
    {
        diagram->AdvanceOperation(mouseMoveArgs);
    }
    SetCursor(location);
}

void Canvas::OnMouseUp(wing::MouseEventArgs& args)
{
    Control::OnMouseUp(args);
    wing::PointF location = ToDiagramLocation(args.location);
    DiagramElement* element = nullptr;
    int index = diagram->GetIndexOfElementAt(location);
    if (index != -1)
    {
        element = diagram->GetElementByIndex(index);
    }
    mouseUpArgs = MouseArgs(location, index, element, args);
    Tool* currentTool = Tools::Instance().Current();
    if (currentTool->CanMove())
    {
        std::chrono::duration duration = mouseUpArgs.time - mouseDownArgs.time;
        int ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        if (ms < clickThresholdMs)
        {
            if (currentTool->IsSelectTool())
            {
                if (operationCreated)
                {
                    operationCreated = false;
                    currentTool->CancelOperation(mouseUpArgs);
                }
                currentTool->MouseClick(mouseUpArgs);
            }
        }
        else
        {
            if (operationCreated)
            {
                operationCreated = false;
                currentTool->CommitOperation(mouseUpArgs);
            }
        }
    }
    else if (currentTool->IsSimpleClickTool())
    {
        currentTool->MouseClick(mouseUpArgs);
    }
    buttonPressed = false;
    SetCursor(location);
}

void Canvas::OnMouseDoubleClick(wing::MouseEventArgs& args)
{
    Control::OnMouseDoubleClick(args);
    wing::PointF location = ToDiagramLocation(args.location);
    DiagramElement* element = nullptr;
    int index = diagram->GetIndexOfElementAt(location);
    if (index != -1)
    {
        element = diagram->GetElementByIndex(index);
    }
    MouseArgs mouseDoubleClickArgs(location, index, element, args);
    Tool* currentTool = Tools::Instance().Current();
    currentTool->MouseDoubleClick(mouseDoubleClickArgs);
}

wing::PointF Canvas::ToDiagramLocation(const wing::Point& controlLocation) const
{
    float x = wing::ScreenMetrics::Get().HorizontalPixelsToMM(controlLocation.X);
    float y = wing::ScreenMetrics::Get().VerticalPixelsToMM(controlLocation.Y);
    wing::PointF loc(x, y);
    Offset(loc, viewOrigin.X, viewOrigin.Y);
    return loc;
}

wing::Point Canvas::ToControlLocation(const wing::PointF& diagramLocation) const
{
    wing::PointF loc(diagramLocation);
    Offset(loc, -viewOrigin.X, -viewOrigin.Y);
    int x = wing::ScreenMetrics::Get().MMToHorizontalPixels(loc.X);
    int y = wing::ScreenMetrics::Get().MMToVerticalPixels(loc.Y);
    return wing::Point(x, y);
}

void Canvas::SetCursor(const wing::PointF& location)
{
    Tool* currentTool = Tools::Instance().Current();
    if (currentTool->IsSelectTool())
    {
        if (diagram->GetOperation()->IsResizeRectangleSelectionOperation())
        {
            ResizeRectangleSelectionOperation* operation = static_cast<ResizeRectangleSelectionOperation*>(diagram->GetOperation());
            wing::Cursor* operationCursor = operation->GetCursor();
            if (operationCursor)
            {
                wing::SetCursor(*operationCursor);
                return;
            }
        }
        else if (diagram->GetSelection()->IsRectangleSelection())
        {
            RectangleSelection* selection = static_cast<RectangleSelection*>(diagram->GetSelection());
            wing::Cursor* selectionCursor = selection->GetCursor(location);
            if (selectionCursor)
            {
                wing::SetCursor(*selectionCursor);
                return;
            }
        }
    }
    if (buttonPressed)
    {
        wing::SetCursor(*currentTool->GetPressedCursor());
    }
    else
    {
        wing::SetCursor(*currentTool->GetCursor());
    }
}

void Canvas::KeyDown(wing::KeyEventArgs& args)
{
    switch (args.key)
    {
        case wing::Keys::delete_:
        {
            DeleteSelection();
            break;
        }
        case wing::Keys::escape:
        {
            diagram->CancelOperation();
            break;
        }
    }
}

void Canvas::EnableCopy()
{
    if (copyMenuItem)
    {
        copyMenuItem->Enable();
    }
}

void Canvas::EnableCut()
{
    if (cutMenuItem)
    {
        cutMenuItem->Enable();
    }
}

void Canvas::EnablePaste()
{
    if (pasteMenuItem)
    {
        pasteMenuItem->Enable();
    }
}

void Canvas::DisableCopy()
{
    if (copyMenuItem)
    {
        copyMenuItem->Disable();
    }
}

void Canvas::DisableCut()
{
    if (cutMenuItem)
    {
        cutMenuItem->Disable();
    }
}

void Canvas::DisablePaste()
{
    if (pasteMenuItem)
    {
        pasteMenuItem->Disable();
    }
}

void Canvas::DeleteSelection()
{
    diagram->DeleteSelection();
}

class CanvasHolder
{
public:
    static CanvasHolder& Instance();
    void SetCanvas(Canvas* canvas_) { canvas = canvas_; }
    Canvas* GetCanvas() { return canvas; }
private:
    CanvasHolder();
    Canvas* canvas;
};

CanvasHolder& CanvasHolder::Instance()
{
    static CanvasHolder instance;
    return instance;
}

CanvasHolder::CanvasHolder() : canvas(nullptr)
{
}

void SetCanvas(Canvas* canvas)
{
    CanvasHolder::Instance().SetCanvas(canvas);
}

Canvas* GetCanvas()
{
    return CanvasHolder::Instance().GetCanvas();
}

Diagram* GetDiagram()
{
    return GetCanvas()->GetDiagram();
}

} // namespace springpp
