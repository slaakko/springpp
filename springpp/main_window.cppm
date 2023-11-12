// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

export module springpp.main_window;

import wing;
import std.core;

export namespace springpp {

class Canvas;

class MainWindow : public wing::Window
{
public:
    MainWindow();
    ~MainWindow();
protected:
    void OnKeyDown(wing::KeyEventArgs& args);
private:
    void NewClick();
    void OpenClick();
    void SaveClick();
    void SaveAsClick();
    void SaveImageClick();
    void ExitClick();
    void CutClick();
    void CopyClick();
    void PasteClick();
    void UndoClick();
    void RedoClick();
    void SelectAllClick();
    void AboutClick();
    void UsersGuideClick();
    void CanvasMouseEnter();
    void CanvasMouseLeave();
    void CanvasMouseMove(wing::MouseEventArgs& args);
    wing::Cursor arrowCursor;
    wing::Cursor& grabOpenCursor;
    wing::Cursor& grabClosedCursor;
    wing::Cursor& classToolCursor;
    wing::Cursor& objectToolCursor;
    wing::Cursor& noteToolCursor;
    wing::Cursor& inheritanceToolCursor;
    wing::Cursor& compositionToolCursor;
    wing::Cursor& aggregationToolCursor;
    wing::Cursor& referenceToolCursor;
    wing::Cursor& createInstanceToolCursor;
    wing::Cursor& attachNoteToolCursor;
    std::unique_ptr<wing::Bitmap> noteCornerBitmap;
    wing::MenuItem* cutMenuItem;
    wing::MenuItem* copyMenuItem;
    wing::MenuItem* pasteMenuItem;
    wing::MenuItem* undoMenuItem;
    wing::MenuItem* redoMenuItem;
    Canvas* canvas;
    wing::StatusBarTextItem* filePathStatusBarItem;
    wing::StatusBarTextItem* modifiedStatusBarItem;
    wing::StatusBarTextItem* viewXCoordStatusBarItem;
    wing::StatusBarTextItem* viewYCoordStatusBarItem;
    wing::StatusBarTextItem* xCoordStatusBarItem;
    wing::StatusBarTextItem* yCoordStatusBarItem;
    std::string diagramFilePath;
};

} // namespace springpp
