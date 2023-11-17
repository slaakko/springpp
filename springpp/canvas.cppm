// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

export module springpp.canvas;

import springpp.diagram;
import springpp.mouse_args;
import wing;
import std.core;

export namespace springpp {

struct CanvasCreateParams
{
    CanvasCreateParams& Defaults();
    CanvasCreateParams& WindowClassName(const std::string& windowClassName_);
    CanvasCreateParams& WindowClassStyle(uint32_t windowClassStyle_);
    CanvasCreateParams& WindowStyle(int windowStyle_);
    CanvasCreateParams& WindowClassBackgroundColor(int windowClassBackgroundColor_);
    CanvasCreateParams& BackgroundColor(const wing::Color& backgroundColor_);
    CanvasCreateParams& Text(const std::string& text_);
    CanvasCreateParams& Location(wing::Point location_);
    CanvasCreateParams& SetSize(wing::Size size_);
    CanvasCreateParams& SetAnchors(wing::Anchors anchors_);
    CanvasCreateParams& SetDock(wing::Dock dock_);
    wing::ControlCreateParams controlCreateParams;
};

const int clickThresholdMs = 250;

class Canvas : public wing::Control
{
public:    
    Canvas(CanvasCreateParams& createParams);
    Diagram* GetDiagram() const { return diagram.get(); }
    void SetDiagram(Diagram* diagram_);
    void SetNoteCornerBitmap(wing::Bitmap* noteCornerBitmap_);
    wing::Bitmap* NoteCornerBitmap() const { return noteCornerBitmap; }
    void SetModifiedStatusBarItem(wing::StatusBarItem* modifiedStatusBarItem_);
    wing::StatusBarItem* ModifiedStatusBarItem() const { return modifiedStatusBarItem; }
    void SetCopyMenuItem(wing::MenuItem* copyMenuItem_);
    void SetCutMenuItem(wing::MenuItem* cutMenuItem_);
    void SetPasteMenuItem(wing::MenuItem* pasteMenuItem_);
    void SetUndoMenuItem(wing::MenuItem* undoMenuItem_);
    void SetRedoMenuItem(wing::MenuItem* redoMenuItem_);
    const wing::PointF& ViewOrigin() const { return viewOrigin; }
    void SetViewOrigin(const wing::PointF& viewOrigin_);
    void SetCursor(const wing::PointF& location);
    void KeyDown(wing::KeyEventArgs& args);
    void EnableCopy();
    void EnableCut();
    void EnablePaste();
    void DisableCopy();
    void DisableCut();
    void DisablePaste();
    wing::PointF ToDiagramLocation(const wing::Point& controlLocation) const;
    wing::Point ToControlLocation(const wing::PointF& diagramLocation) const;
protected:
    void SetCursor();
    void OnPaint(wing::PaintEventArgs& args) override;
    void OnMouseDown(wing::MouseEventArgs& args) override;
    void OnMouseMove(wing::MouseEventArgs& args) override;
    void OnMouseUp(wing::MouseEventArgs& args) override;
    void OnMouseDoubleClick(wing::MouseEventArgs& args) override;
private:
    void DeleteSelection();
    std::unique_ptr<Diagram> diagram;
    wing::Bitmap* noteCornerBitmap;
    wing::StatusBarItem* modifiedStatusBarItem;
    wing::MenuItem* copyMenuItem;
    wing::MenuItem* cutMenuItem;
    wing::MenuItem* pasteMenuItem;
    wing::MenuItem* undoMenuItem;
    wing::MenuItem* redoMenuItem;
    wing::PointF viewOrigin;
    MouseArgs mouseDownArgs;
    MouseArgs mouseMoveArgs;
    MouseArgs mouseUpArgs;
    bool operationCreated;
    bool buttonPressed;
};

void SetCanvas(Canvas* canvas);
Canvas* GetCanvas();
Diagram* GetDiagram();

} // namespace springpp
