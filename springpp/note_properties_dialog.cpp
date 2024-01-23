// =================================
// Copyright (c) 2024 Seppo Laakko
// Distributed under the MIT license
// =================================

module;
#include <Windows.h>

module springpp.note_properties_dialog;

import springpp.note_element;
import util;

namespace springpp {

NotePropertiesDialog::NotePropertiesDialog(NoteElement* noteElement_) : wing::Window(wing::WindowCreateParams().Defaults().Text("Note Properties").
    WindowStyle(wing::DialogWindowStyle()).Location(wing::DefaultLocation()).
    WindowClassBackgroundColor(wing::DefaultControlWindowClassBackgroundColor()).BackgroundColor(wing::DefaultControlBackgroundColor()).
    SetSize(wing::Size(wing::ScreenMetrics::Get().MMToHorizontalPixels(100), wing::ScreenMetrics::Get().MMToVerticalPixels(70)))),
    noteElement(noteElement_), textBox(nullptr), cancelButton(nullptr), okButton(nullptr)
{
    wing::Size s = GetSize();
    wing::Size defaultButtonSize = wing::ScreenMetrics::Get().DefaultButtonSize();
    wing::Size defaultControlSpacing = wing::ScreenMetrics::Get().DefaultControlSpacing();
    wing::Size defaultLabelSize = wing::ScreenMetrics::Get().DefaultLabelSize();
    wing::Size defaultTextBoxSize = wing::ScreenMetrics::Get().DefaultTextBoxSize();
    wing::Point textBoxLoc(defaultControlSpacing.Width, defaultControlSpacing.Height);
    wing::Size textBoxSize(wing::ScreenMetrics::Get().MMToHorizontalPixels(80), wing::ScreenMetrics::Get().MMToVerticalPixels(30));
    std::string noteText;
    bool first = true;
    for (const std::string& line : noteElement->Lines())
    {
        if (first)
        {
            first = false;
        }
        else
        {
            noteText.append("\r\n");
        }
        noteText.append(line);
    }
    std::unique_ptr<wing::TextBox> textBoxPtr(new wing::TextBox(wing::TextBoxCreateParams().Defaults().
        WindowStyle(wing::DefaultChildWindowStyle() | ES_LEFT | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE).
        WindowClassBackgroundColor(COLOR_WINDOW).
        BackgroundColor(wing::Color::White).Text(noteText).Location(textBoxLoc).SetSize(textBoxSize).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    textBox = textBoxPtr.get();
    textBox->TextChanged().AddHandler(this, &NotePropertiesDialog::NoteTextChanged);
    textBox->SelectAll();
    std::unique_ptr<wing::PaddedControl> paddedTextBox(new wing::PaddedControl(wing::PaddedControlCreateParams(textBoxPtr.release()).Defaults()));
    std::unique_ptr<wing::BorderedControl> borderedPaddedTextBox(
        new wing::BorderedControl(wing::BorderedControlCreateParams(paddedTextBox.release()).SetBorderStyle(wing::BorderStyle::threeD).
            Location(textBoxLoc).SetSize(textBoxSize).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    AddChild(borderedPaddedTextBox.release());
    int x = s.Width - defaultButtonSize.Width - defaultControlSpacing.Width;
    int y = s.Height - defaultButtonSize.Height - defaultControlSpacing.Height;
    std::unique_ptr<wing::Button> cancelButtonPtr(new wing::Button(wing::ControlCreateParams().Defaults().Text("Cancel").Location(wing::Point(x, y)).SetSize(defaultButtonSize).
        SetAnchors(wing::Anchors::bottom | wing::Anchors::right)));
    cancelButton = cancelButtonPtr.get();
    cancelButton->SetDialogResult(wing::DialogResult::cancel);
    x = x - defaultButtonSize.Width - defaultControlSpacing.Width;
    std::unique_ptr<wing::Button > okButtonPtr(new wing::Button(wing::ControlCreateParams().Defaults().Text("OK").Location(wing::Point(x, y)).
        SetSize(defaultButtonSize).SetDock(wing::Dock::none).SetAnchors(static_cast<wing::Anchors>(wing::Anchors::bottom | wing::Anchors::right))));
    okButton = okButtonPtr.get();
    okButton->SetDialogResult(wing::DialogResult::ok);
    AddChild(okButtonPtr.release());
    AddChild(cancelButtonPtr.release());
    SetDefaultButton(okButton);
    SetCancelButton(cancelButton);
    textBox->SetFocus();
}

void NotePropertiesDialog::NoteTextChanged()
{
    std::u32string utext = util::ToUtf32(textBox->Text());
    std::vector<std::u32string> ulines = wing::SplitTextIntoLines(utext);
    std::vector<std::string> lines;
    for (const auto& uline : ulines)
    {
        lines.push_back(util::ToUtf8(uline));
    }
    noteElement->SetLines(std::move(lines));
}

} // namespace springpp
