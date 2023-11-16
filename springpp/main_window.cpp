// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

module;
#include <Windows.h>
#undef MessageBox

module springpp.main_window;

import springpp.tool;
import springpp.canvas;
import springpp.diagram;
import springpp.layout;
import springpp.save_image_dialog;
import springpp.about_dialog;
import util;
import std.filesystem;

namespace springpp {

MainWindow::MainWindow() : 
    wing::Window(wing::WindowCreateParams().Defaults().Text("Spring++ Software Diagram Designer")),
    arrowCursor(wing::LoadStandardCursor(wing::StandardCursorId::arrow)),
    grabOpenCursor(wing::Application::GetResourceManager().GetCursor("grab.open.spring.cursor")),
    grabClosedCursor(wing::Application::GetResourceManager().GetCursor("grab.closed.spring.cursor")),
    classToolCursor(wing::Application::GetResourceManager().GetCursor("class.tool.spring.cursor")),
    objectToolCursor(wing::Application::GetResourceManager().GetCursor("object.tool.spring.cursor")),
    noteToolCursor(wing::Application::GetResourceManager().GetCursor("note.tool.spring.cursor")),
    inheritanceToolCursor(wing::Application::GetResourceManager().GetCursor("inheritance.tool.spring.cursor")),
    compositionToolCursor(wing::Application::GetResourceManager().GetCursor("composition.tool.spring.cursor")),
    aggregationToolCursor(wing::Application::GetResourceManager().GetCursor("aggregation.tool.spring.cursor")),
    referenceToolCursor(wing::Application::GetResourceManager().GetCursor("reference.tool.spring.cursor")),
    createInstanceToolCursor(wing::Application::GetResourceManager().GetCursor("create.instance.tool.spring.cursor")),
    attachNoteToolCursor(wing::Application::GetResourceManager().GetCursor("attach.note.tool.spring.cursor")),
    noteCornerBitmap(wing::Bitmap::FromResource(wing::Instance(), L"note.corner.spring.bitmap")),
    cutMenuItem(nullptr),
    copyMenuItem(nullptr),
    pasteMenuItem(nullptr),
    undoMenuItem(nullptr),
    redoMenuItem(nullptr),
    filePathStatusBarItem(nullptr),
    modifiedStatusBarItem(nullptr),
    viewXCoordStatusBarItem(nullptr),
    viewYCoordStatusBarItem(nullptr),
    xCoordStatusBarItem(nullptr),
    yCoordStatusBarItem(nullptr),
    springppDiagramElementsFormat(nullptr), 
    clipboardListener(nullptr)
{
    try
    {
        std::unique_ptr<wing::MenuBar> menuBar(new wing::MenuBar());
        wing::MenuItem* fileMenuItem = new wing::MenuItem("&File");
        wing::MenuItem* newMenuItem = new wing::MenuItem("&New");
        newMenuItem->SetShortcut(wing::Keys::controlModifier | wing::Keys::n);
        newMenuItem->Click().AddHandler(this, &MainWindow::NewClick);
        fileMenuItem->AddMenuItem(newMenuItem);
        wing::MenuItem* openMenuItem = new wing::MenuItem("&Open...");
        openMenuItem->SetShortcut(wing::Keys::controlModifier | wing::Keys::o);
        openMenuItem->Click().AddHandler(this, &MainWindow::OpenClick);
        fileMenuItem->AddMenuItem(openMenuItem);
        wing::MenuItem* saveMenuItem = new wing::MenuItem("&Save");
        saveMenuItem->SetShortcut(wing::Keys::controlModifier | wing::Keys::s);
        saveMenuItem->Click().AddHandler(this, &MainWindow::SaveClick);
        fileMenuItem->AddMenuItem(saveMenuItem);
        wing::MenuItem* saveAsMenuItem = new wing::MenuItem("Save &As...");
        saveAsMenuItem->Click().AddHandler(this, &MainWindow::SaveAsClick);
        fileMenuItem->AddMenuItem(saveAsMenuItem);
        wing::MenuItem* saveImageMenuItem = new wing::MenuItem("Save &Image...");
        saveImageMenuItem->Click().AddHandler(this, &MainWindow::SaveImageClick);
        fileMenuItem->AddMenuItem(saveImageMenuItem);
        wing::MenuItem* exitMenuItem = new wing::MenuItem("E&xit");
        exitMenuItem->SetShortcut(wing::Keys::altModifier | wing::Keys::f4);
        exitMenuItem->Click().AddHandler(this, &MainWindow::ExitClick);
        fileMenuItem->AddMenuItem(exitMenuItem);
        menuBar->AddMenuItem(fileMenuItem);
        wing::MenuItem* editMenuItem = new wing::MenuItem("&Edit");
        cutMenuItem = new wing::MenuItem("Cu&t");
        cutMenuItem->Disable();
        cutMenuItem->SetShortcut(wing::Keys::controlModifier | wing::Keys::x);
        cutMenuItem->Click().AddHandler(this, &MainWindow::CutClick);
        editMenuItem->AddMenuItem(cutMenuItem);
        copyMenuItem = new wing::MenuItem("&Copy");
        copyMenuItem->Disable();
        copyMenuItem->SetShortcut(wing::Keys::controlModifier | wing::Keys::c);
        copyMenuItem->Click().AddHandler(this, &MainWindow::CopyClick);
        editMenuItem->AddMenuItem(copyMenuItem);
        pasteMenuItem = new wing::MenuItem("&Paste");
        pasteMenuItem->Disable();
        pasteMenuItem->SetShortcut(wing::Keys::controlModifier | wing::Keys::v);
        pasteMenuItem->Click().AddHandler(this, &MainWindow::PasteClick);
        editMenuItem->AddMenuItem(pasteMenuItem);
        undoMenuItem = new wing::MenuItem("&Undo");
        undoMenuItem->Disable();
        undoMenuItem->SetShortcut(wing::Keys::controlModifier | wing::Keys::z);
        undoMenuItem->Click().AddHandler(this, &MainWindow::UndoClick);
        editMenuItem->AddMenuItem(undoMenuItem);
        redoMenuItem = new wing::MenuItem("&Redo");
        redoMenuItem->Disable();
        redoMenuItem->SetShortcut(wing::Keys::controlModifier | wing::Keys::y);
        redoMenuItem->Click().AddHandler(this, &MainWindow::RedoClick);
        editMenuItem->AddMenuItem(redoMenuItem);
        wing::MenuItem* selectAllMenuItem = new wing::MenuItem("Select &All");
        selectAllMenuItem->SetShortcut(wing::Keys::controlModifier | wing::Keys::a);
        selectAllMenuItem->Click().AddHandler(this, &MainWindow::SelectAllClick);
        editMenuItem->AddMenuItem(selectAllMenuItem);
        menuBar->AddMenuItem(editMenuItem);
        wing::MenuItem* helpMenuItem = new wing::MenuItem("&Help");
        wing::MenuItem* usersGuideMenuItem = new wing::MenuItem("&User's Guide");
        usersGuideMenuItem->Click().AddHandler(this, &MainWindow::UsersGuideClick);
        helpMenuItem->AddMenuItem(usersGuideMenuItem);
        wing::MenuItem* aboutMenuItem = new wing::MenuItem("&About...");
        aboutMenuItem->Click().AddHandler(this, &MainWindow::AboutClick);
        helpMenuItem->AddMenuItem(aboutMenuItem);
        menuBar->AddMenuItem(helpMenuItem);
        AddChild(menuBar.release());
        std::unique_ptr<wing::ToolBar> toolBar(new wing::ToolBar(wing::ToolBarCreateParams().Defaults().Style(wing::ToolBarStyle::radioButtonGroup)));
        wing::ToolButton* selectToolButton = new wing::ToolButton(
            wing::ToolButtonCreateParams().Defaults().ToolBitMapName("arrow.wing.bitmap").Style(wing::ToolButtonStyle::manual));
        selectToolButton->SetToolTip("Select");
        Tools::Instance().GetSelectTool()->SetToolButton(selectToolButton);
        Tools::Instance().GetSelectTool()->SetCursor(&arrowCursor);
        toolBar->AddToolButton(selectToolButton);
        wing::ToolButton* grabToolButton = new wing::ToolButton(
            wing::ToolButtonCreateParams().Defaults().ToolBitMapName("grab.tool.spring.bitmap").Style(wing::ToolButtonStyle::manual));
        grabToolButton->SetToolTip("Grab");
        Tools::Instance().GetGrabTool()->SetToolButton(grabToolButton);
        Tools::Instance().GetGrabTool()->SetCursor(&grabOpenCursor);
        Tools::Instance().GetGrabTool()->SetPressedCursor(&grabClosedCursor);
        toolBar->AddToolButton(grabToolButton);
        wing::ToolButton* classToolButton = new wing::ToolButton(
            wing::ToolButtonCreateParams().Defaults().ToolBitMapName("class.tool.spring.bitmap").Style(wing::ToolButtonStyle::manual));
        classToolButton->SetToolTip("Add new class");
        Tools::Instance().GetClassTool()->SetToolButton(classToolButton);
        Tools::Instance().GetClassTool()->SetCursor(&classToolCursor);
        toolBar->AddToolButton(classToolButton);
        wing::ToolButton* objectToolButton = new wing::ToolButton(
            wing::ToolButtonCreateParams().Defaults().ToolBitMapName("object.tool.spring.bitmap").Style(wing::ToolButtonStyle::manual));
        objectToolButton->SetToolTip("Add new object");
        Tools::Instance().GetObjectTool()->SetToolButton(objectToolButton);
        Tools::Instance().GetObjectTool()->SetCursor(&objectToolCursor);
        toolBar->AddToolButton(objectToolButton);
        wing::ToolButton* noteToolButton = new wing::ToolButton(
            wing::ToolButtonCreateParams().Defaults().ToolBitMapName("note.tool.spring.bitmap").Style(wing::ToolButtonStyle::manual));
        noteToolButton->SetToolTip("Add new note");
        Tools::Instance().GetNoteTool()->SetToolButton(noteToolButton);
        Tools::Instance().GetNoteTool()->SetCursor(&noteToolCursor);
        toolBar->AddToolButton(noteToolButton);
        wing::ToolButton* inheritanceToolButton = new wing::ToolButton(
            wing::ToolButtonCreateParams().Defaults().ToolBitMapName("inheritance.tool.spring.bitmap").Style(wing::ToolButtonStyle::manual));
        inheritanceToolButton->SetToolTip("Add inheritance relationship");
        Tools::Instance().GetInheritanceTool()->SetToolButton(inheritanceToolButton);
        Tools::Instance().GetInheritanceTool()->SetCursor(&inheritanceToolCursor);
        toolBar->AddToolButton(inheritanceToolButton);
        wing::ToolButton* compositionToolButton = new wing::ToolButton(
            wing::ToolButtonCreateParams().Defaults().ToolBitMapName("composition.tool.spring.bitmap").Style(wing::ToolButtonStyle::manual));
        compositionToolButton->SetToolTip("Add composition relationship");
        Tools::Instance().GetCompositionTool()->SetToolButton(compositionToolButton);
        Tools::Instance().GetCompositionTool()->SetCursor(&compositionToolCursor);
        toolBar->AddToolButton(compositionToolButton);
        wing::ToolButton* aggregationToolButton = new wing::ToolButton(
            wing::ToolButtonCreateParams().Defaults().ToolBitMapName("aggregation.tool.spring.bitmap").Style(wing::ToolButtonStyle::manual));
        aggregationToolButton->SetToolTip("Add aggregation relationship");
        Tools::Instance().GetAggregationTool()->SetToolButton(aggregationToolButton);
        Tools::Instance().GetAggregationTool()->SetCursor(&aggregationToolCursor);
        toolBar->AddToolButton(aggregationToolButton);
        wing::ToolButton* referenceToolButton = new wing::ToolButton(
            wing::ToolButtonCreateParams().Defaults().ToolBitMapName("reference.tool.spring.bitmap").Style(wing::ToolButtonStyle::manual));
        referenceToolButton->SetToolTip("Add reference relationship");
        Tools::Instance().GetReferenceTool()->SetToolButton(referenceToolButton);
        Tools::Instance().GetReferenceTool()->SetCursor(&referenceToolCursor);
        toolBar->AddToolButton(referenceToolButton);
        wing::ToolButton* createInstanceToolButton = new wing::ToolButton(
            wing::ToolButtonCreateParams().Defaults().ToolBitMapName("create.instance.tool.spring.bitmap").Style(wing::ToolButtonStyle::manual));
        createInstanceToolButton->SetToolTip("Add create instance relationship");
        Tools::Instance().GetCreateInstanceTool()->SetToolButton(createInstanceToolButton);
        Tools::Instance().GetCreateInstanceTool()->SetCursor(&createInstanceToolCursor);
        toolBar->AddToolButton(createInstanceToolButton);
        wing::ToolButton* attachNoteToolButton = new wing::ToolButton(
            wing::ToolButtonCreateParams().Defaults().ToolBitMapName("attach.note.tool.spring.bitmap").Style(wing::ToolButtonStyle::manual));
        attachNoteToolButton->SetToolTip("Attach note");
        Tools::Instance().GetAttachNoteTool()->SetToolButton(attachNoteToolButton);
        Tools::Instance().GetAttachNoteTool()->SetCursor(&attachNoteToolCursor);
        toolBar->AddToolButton(attachNoteToolButton);
        wing::Size toolBarSize = toolBar->GetSize();
        toolBarSize.Width += 3;
        toolBarSize.Height += 3;
        std::unique_ptr<wing::PaddedControl> paddedToolBar(new wing::PaddedControl(wing::PaddedControlCreateParams(toolBar.release()).Defaults()));
        wing::Size paddedToolBarSize = wing::PaddedSize(toolBarSize, wing::DefaultPadding());
        std::unique_ptr<wing::BorderedControl> borderedToolBar(new wing::BorderedControl(wing::BorderedControlCreateParams(paddedToolBar.release()).SetBorderStyle(wing::BorderStyle::single).
            NormalSingleBorderColor(wing::GetColor("tool.bar.frame")).FocusedSingleBorderColor(wing::GetColor("tool.bar.frame")).SetSize(paddedToolBarSize).SetDock(wing::Dock::top)));
        borderedToolBar->SetFrameItemName("tool.bar.frame");
        AddChild(borderedToolBar.release());
        canvas = new Canvas(CanvasCreateParams().Defaults());
        SetCanvas(canvas);
        canvas->SetNoteCornerBitmap(noteCornerBitmap.get());
        canvas->SetCopyMenuItem(copyMenuItem);
        canvas->SetCutMenuItem(cutMenuItem);
        canvas->SetPasteMenuItem(pasteMenuItem);
        canvas->SetUndoMenuItem(undoMenuItem);
        canvas->SetRedoMenuItem(redoMenuItem);
        canvas->GetDiagram()->SetMenuItems(undoMenuItem, redoMenuItem);
        canvas->MouseEnter().AddHandler(this, &MainWindow::CanvasMouseEnter);
        canvas->MouseLeave().AddHandler(this, &MainWindow::CanvasMouseLeave);
        canvas->MouseMove().AddHandler(this, &MainWindow::CanvasMouseMove);
        AddChild(canvas);
        std::unique_ptr<wing::StatusBar> statusBar(new wing::StatusBar(wing::StatusBarCreateParams().Defaults()));
        filePathStatusBarItem = new wing::StatusBarTextItem(wing::StatusBarTextItemCreateParams().Defaults().BorderStyle(wing::StatusBarItemBorderStyle::sunken));
        statusBar->AddItem(filePathStatusBarItem);
        modifiedStatusBarItem = new wing::StatusBarTextItem(wing::StatusBarTextItemCreateParams().Defaults().BorderStyle(wing::StatusBarItemBorderStyle::sunken));
        canvas->SetModifiedStatusBarItem(modifiedStatusBarItem);
        modifiedStatusBarItem->SetText(" ");
        statusBar->AddItem(modifiedStatusBarItem);
        statusBar->AddItem(new wing::StatusBarSpringItem());
        wing::StatusBarTextItem* viewXItem = new wing::StatusBarTextItem(wing::StatusBarTextItemCreateParams().Defaults().BorderStyle(
            wing::StatusBarItemBorderStyle::sunken).Text("view X:"));
        statusBar->AddItem(viewXItem);
        viewXCoordStatusBarItem = new wing::StatusBarTextItem(wing::StatusBarTextItemCreateParams().Defaults().BorderStyle(wing::StatusBarItemBorderStyle::sunken).MaxTextLength(6));
        statusBar->AddItem(viewXCoordStatusBarItem);
        wing::StatusBarTextItem* viewYItem = new wing::StatusBarTextItem(wing::StatusBarTextItemCreateParams().Defaults().BorderStyle(
            wing::StatusBarItemBorderStyle::sunken).Text("view Y:"));
        statusBar->AddItem(viewYItem);
        viewYCoordStatusBarItem = new wing::StatusBarTextItem(wing::StatusBarTextItemCreateParams().Defaults().BorderStyle(wing::StatusBarItemBorderStyle::sunken).MaxTextLength(6));
        statusBar->AddItem(viewYCoordStatusBarItem);
        wing::StatusBarTextItem* xItem = new wing::StatusBarTextItem(wing::StatusBarTextItemCreateParams().Defaults().BorderStyle(
            wing::StatusBarItemBorderStyle::sunken).Text("X:"));
        statusBar->AddItem(xItem);
        xCoordStatusBarItem = new wing::StatusBarTextItem(wing::StatusBarTextItemCreateParams().Defaults().BorderStyle(wing::StatusBarItemBorderStyle::sunken).MaxTextLength(6));
        statusBar->AddItem(xCoordStatusBarItem);
        wing::StatusBarTextItem* yItem = new wing::StatusBarTextItem(wing::StatusBarTextItemCreateParams().Defaults().BorderStyle(
            wing::StatusBarItemBorderStyle::sunken).Text("Y:"));
        statusBar->AddItem(yItem);
        yCoordStatusBarItem = new wing::StatusBarTextItem(wing::StatusBarTextItemCreateParams().Defaults().BorderStyle(wing::StatusBarItemBorderStyle::sunken).MaxTextLength(6));
        statusBar->AddItem(yCoordStatusBarItem);
        AddChild(statusBar.release());
        springppDiagramElementsFormat.reset(new wing::ClipboardFormat("springpp.diagram.elements"));
        AddClipboardListener();
    }
    catch (const std::exception& ex)
    {
        wing::ShowErrorMessageBox(nullptr, ex.what());
    }
}

MainWindow::~MainWindow()
{
}

void MainWindow::OnWindowClosing(wing::CancelArgs& args)
{
    Diagram* diagram = canvas->GetDiagram();
    if (diagram->IsModified())
    {
        wing::MessageBoxResult result = wing::MessageBox::Show("Current diagram is modified. Do you want to save changes?", "Diagram modified", this, MB_YESNOCANCEL);
        if (result == wing::MessageBoxResult::yes)
        {
            SaveClick();
        }
        else if (result == wing::MessageBoxResult::cancel)
        {
            args.cancel = true;
            return;
        }
    }
    RemoveClipboardListener();
}

void MainWindow::OnKeyDown(wing::KeyEventArgs& args)
{
    try
    {
        wing::Window::OnKeyDown(args);
        switch (args.key)
        {
            case wing::Keys::controlModifier | wing::Keys::insert:
            {
                if (copyMenuItem->IsEnabled())
                {
                    CopyClick();
                }
                args.handled = true;
                break;
            }
            case wing::Keys::shiftModifier | wing::Keys::insert:
            {
                if (pasteMenuItem->IsEnabled())
                {
                    PasteClick();
                }
                args.handled = true;
                break;
            }
            case wing::Keys::shiftModifier | wing::Keys::delete_:
            {
                if (cutMenuItem->IsEnabled())
                {
                    CutClick();
                }
                args.handled = true;
                break;
            }
        }
        if (!args.handled)
        {
            canvas->KeyDown(args);
        }
    }
    catch (const std::exception& ex)
    {
        wing::ShowErrorMessageBox(nullptr, ex.what());
    }
}

void MainWindow::OnClipboardUpdate()
{
    try
    {
        Window::OnClipboardUpdate();
        wing::Clipboard clipboard(Handle());
        if (clipboard.IsClipboardFormatAvailable(*springppDiagramElementsFormat))
        {
            clipboardData = clipboard.GetStringData(*springppDiagramElementsFormat);
            pasteMenuItem->Enable();
        }
    }
    catch (const std::exception& ex)
    {
        wing::ShowErrorMessageBox(nullptr, ex.what());
    }
}

void MainWindow::NewClick()
{
    try
    {
        Diagram* diagram = canvas->GetDiagram();
        if (diagram->IsModified())
        {
            wing::MessageBoxResult result = wing::MessageBox::Show("Current diagram is modified. Do you want to save changes?", "Diagram modified", this, MB_YESNOCANCEL);
            if (result == wing::MessageBoxResult::yes)
            {
                SaveClick();
            }
            else if (result == wing::MessageBoxResult::cancel)
            {
                return;
            }
        }
        std::unique_ptr<Diagram> newDiagram(new Diagram());
        diagramFilePath.clear();
        filePathStatusBarItem->SetText(std::string());
        modifiedStatusBarItem->SetText(" ");
        canvas->SetDiagram(newDiagram.release());
        canvas->GetDiagram()->ResetModified();
    }
    catch (const std::exception& ex)
    {
        wing::ShowErrorMessageBox(nullptr, ex.what());
    }
}

void MainWindow::OpenClick()
{
    try
    {
        Diagram* diagram = canvas->GetDiagram();
        if (diagram->IsModified())
        {
            wing::MessageBoxResult result = wing::MessageBox::Show("Current diagram is modified. Do you want to save changes?", "Diagram modified", this, MB_YESNOCANCEL);
            if (result == wing::MessageBoxResult::yes)
            {
                SaveClick();
            }
            else if (result == wing::MessageBoxResult::cancel)
            {
                return;
            }
        }
        std::vector<std::pair<std::string, std::string>> descriptionFilterPairs;
        descriptionFilterPairs.push_back(std::make_pair("Spring++ diagram files (*.springpp.xml)", "*.springpp.xml"));
        std::string initialDirectory = DiagramDirectory();
        std::string filePath;
        std::string currentDirectory;
        std::vector<std::string> fileNames;
        std::string defaultFilePath;
        bool selected = wing::OpenFileName(Handle(), descriptionFilterPairs, initialDirectory, defaultFilePath, ".springpp.xml",
            OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST, filePath, currentDirectory, fileNames);
        if (selected)
        {
            if (!fileNames.empty())
            {
                filePath = util::Path::Combine(currentDirectory, fileNames[0]);
            }
            if (!filePath.empty() && std::filesystem::exists(filePath))
            {
                std::unique_ptr<Diagram> diagram(new Diagram());
                diagram->Load(filePath);
                diagramFilePath = filePath;
                filePathStatusBarItem->SetText(diagramFilePath);
                modifiedStatusBarItem->SetText(" ");
                canvas->SetDiagram(diagram.release());
                canvas->GetDiagram()->ResetModified();
            }
            else
            {
                throw std::runtime_error("file path is empty or does not exist");
            }
        }
    }
    catch (const std::exception& ex)
    {
        wing::ShowErrorMessageBox(nullptr, ex.what());
    }
}

void MainWindow::SaveClick()
{
    try
    {
        Diagram* diagram = canvas->GetDiagram();
        if (diagramFilePath.empty())
        {
            SaveAsClick();
        }
        else
        {
            diagram->Save(diagramFilePath);
            diagram->ResetModified();
        }
    }
    catch (const std::exception& ex)
    {
        wing::ShowErrorMessageBox(nullptr, ex.what());
    }
}

void MainWindow::SaveAsClick()
{
    try
    {
        std::vector<std::pair<std::string, std::string>> descriptionFilterPairs;
        descriptionFilterPairs.push_back(std::make_pair("Spring++ diagram files (*.springpp.xml)", "*.springpp.xml"));
        std::string initialDirectory = DiagramDirectory();
        std::string filePath;
        bool succeeded = wing::SaveFileName(Handle(), descriptionFilterPairs, initialDirectory, 0, ".springpp.xml", filePath);
        if (succeeded)
        {
            if (std::filesystem::exists(filePath))
            {
                if (wing::MessageBox::Show("File '" + filePath + "' already exists. Do you want to replace it?", "File Exists", this, MB_YESNO) != wing::MessageBoxResult::yes)
                {
                    return;
                }
            }
            Diagram* diagram = canvas->GetDiagram();
            diagram->Save(filePath);
            diagramFilePath = filePath;
            filePathStatusBarItem->SetText(diagramFilePath);
            diagram->ResetModified();
        }
    }
    catch (const std::exception& ex)
    {
        wing::ShowErrorMessageBox(nullptr, ex.what());
    }
}

void MainWindow::SaveImageClick()
{
    try
    {
        Canvas* canvas = GetCanvas();
        Diagram* diagram = canvas->GetDiagram();
        SaveImageDialog dialog(diagramFilePath);
        if (dialog.ShowDialog(*this) == wing::DialogResult::ok)
        {
            diagram->SaveImage(dialog.FileName(), dialog.Margins(), dialog.Format());
        }
    }
    catch (const std::exception& ex)
    {
        wing::ShowErrorMessageBox(nullptr, ex.what());
    }
}

void MainWindow::ExitClick()
{
    try
    {
        Close();
    }
    catch (const std::exception& ex)
    {
        wing::ShowErrorMessageBox(nullptr, ex.what());
    }
}

void MainWindow::CopyClick()
{
    try
    {
        Selection* selection = canvas->GetDiagram()->GetSelection();
        std::u32string copy = selection->Copy();
        if (!copy.empty())
        {
            wing::Clipboard clipboard(Handle());
            clipboard.SetStringData(*springppDiagramElementsFormat, copy);
        }
    }
    catch (const std::exception& ex)
    {
        wing::ShowErrorMessageBox(nullptr, ex.what());
    }
}

void MainWindow::CutClick()
{
    try
    {
        Selection* selection = canvas->GetDiagram()->GetSelection();
        std::u32string copy = selection->Copy();
        if (!copy.empty())
        {
            wing::Clipboard clipboard(Handle());
            clipboard.SetStringData(*springppDiagramElementsFormat, copy);
            selection->Delete();
        }
    }
    catch (const std::exception& ex)
    {
        wing::ShowErrorMessageBox(nullptr, ex.what());
    }
}

void MainWindow::PasteClick()
{
    try
    {
        if (!clipboardData.empty())
        {
            canvas->GetDiagram()->Paste(clipboardData);
        }
    }
    catch (const std::exception& ex)
    {
        wing::ShowErrorMessageBox(nullptr, ex.what());
    }
}

void MainWindow::UndoClick()
{
    try
    {
        canvas->GetDiagram()->Undo();
    }
    catch (const std::exception& ex)
    {
        wing::ShowErrorMessageBox(nullptr, ex.what());
    }
}

void MainWindow::RedoClick()
{
    try
    {
        canvas->GetDiagram()->Redo();
    }
    catch (const std::exception& ex)
    {
        wing::ShowErrorMessageBox(nullptr, ex.what());
    }
}

void MainWindow::SelectAllClick()
{
    try
    {
        Diagram* diagram = canvas->GetDiagram();
        wing::RectF diagramRect = diagram->CalculateBoundingRect();
        std::unique_ptr<RectangleSelection> selection(new RectangleSelection(diagram, wing::PointF()));
        selection->SetRect(diagramRect);
        diagram->SetSelection(selection.release());
        diagram->SetChanged();
        diagram->Invalidate();
    }
    catch (const std::exception& ex)
    {
        wing::ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::AboutClick()
{
    try
    {
        AboutDialog dialog;
        dialog.ShowDialog(*this);
    }
    catch (const std::exception& ex)
    {
        wing::ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::UsersGuideClick()
{
    try
    {
        // todo
    }
    catch (const std::exception& ex)
    {
        wing::ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::CanvasMouseEnter()
{
    xCoordStatusBarItem->SetText(std::string(6, ' '));
    yCoordStatusBarItem->SetText(std::string(6, ' '));
}

void MainWindow::CanvasMouseLeave()
{
    xCoordStatusBarItem->SetText(std::string(6, ' '));
    yCoordStatusBarItem->SetText(std::string(6, ' '));
}

void MainWindow::CanvasMouseMove(wing::MouseEventArgs& args)
{
    std::string viewXCoordText = util::Format(std::format("{:6.2f}", canvas->ViewOrigin().X), 6, util::FormatJustify::right);
    viewXCoordStatusBarItem->SetText(viewXCoordText);
    std::string viewYCoordText = util::Format(std::format("{:6.2f}", canvas->ViewOrigin().Y), 6, util::FormatJustify::right);
    viewYCoordStatusBarItem->SetText(viewYCoordText);
    std::string xCoordText = util::Format(std::format("{:6.2f}", canvas->ViewOrigin().X + wing::PixelsToMM(args.location.X, wing::ScreenMetrics::Get().DPIX())), 6,
        util::FormatJustify::right);
    xCoordStatusBarItem->SetText(xCoordText);
    std::string yCoordText = util::Format(std::format("{:6.2f}", canvas->ViewOrigin().Y + wing::PixelsToMM(args.location.Y, wing::ScreenMetrics::Get().DPIY())), 6,
        util::FormatJustify::right);
    yCoordStatusBarItem->SetText(yCoordText);
}

void MainWindow::AddClipboardListener()
{
    try
    {
        clipboardListener.reset(new wing::ClipboardListener(Handle()));
    }
    catch (const std::exception& ex)
    {
        wing::ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::RemoveClipboardListener()
{
    try
    {
        clipboardListener.reset();
    }
    catch (const std::exception& ex)
    {
        wing::ShowErrorMessageBox(Handle(), ex.what());
    }
}

} // namespace springpp
