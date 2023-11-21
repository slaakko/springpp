// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

module;
#include <Windows.h>
#undef min
#undef max

module springpp.text_element_properties_dialog;

import springpp.text_element;
import util;

namespace springpp {

TextElementPropertiesDialog::TextElementPropertiesDialog(const std::string& dialogName, TextElement* textElement_) :
    wing::Window(wing::WindowCreateParams().Defaults().Text(dialogName).
        WindowStyle(wing::DialogWindowStyle()).Location(wing::DefaultLocation()).
        WindowClassBackgroundColor(wing::DefaultControlWindowClassBackgroundColor()).BackgroundColor(wing::DefaultControlBackgroundColor()).
        SetSize(wing::Size(wing::ScreenMetrics::Get().MMToHorizontalPixels(100), wing::ScreenMetrics::Get().MMToVerticalPixels(80)))),
    textElement(textElement_), linesTextBox(nullptr), keywordTextBox(nullptr), cancelButton(nullptr), okButton(nullptr)
{
    wing::Size s = GetSize();
    wing::Size defaultButtonSize = wing::ScreenMetrics::Get().DefaultButtonSize();
    wing::Size defaultControlSpacing = wing::ScreenMetrics::Get().DefaultControlSpacing();
    wing::Size defaultLabelSize = wing::ScreenMetrics::Get().DefaultLabelSize();
    wing::Size defaultTextBoxSize = wing::ScreenMetrics::Get().DefaultTextBoxSize();
    int borderWidth = wing::BorderWidth(wing::BorderStyle::single);
    int textBoxPadding = wing::defaultControlPadding;
    std::string linesText;
    bool first = true;
    lines = textElement->Lines();
    for (const std::string& line : lines)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            linesText.append("\r\n");
        }
        linesText.append(line);
    }

    wing::Point linesLabelLoc(defaultControlSpacing.Width, defaultControlSpacing.Height);
    std::unique_ptr<wing::Label> linesLabelPtr(new wing::Label(wing::LabelCreateParams().Defaults().Text("Text:").
        Location(linesLabelLoc).SetSize(defaultLabelSize).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    wing::Label* linesLabel = linesLabelPtr.get();
    AddChild(linesLabelPtr.release());

    wing::Point textBoxLoc(defaultControlSpacing.Width, defaultControlSpacing.Height + linesLabel->Location().Y + linesLabel->GetSize().Height);
    wing::Size textBoxSize(wing::ScreenMetrics::Get().MMToHorizontalPixels(80), wing::ScreenMetrics::Get().MMToVerticalPixels(30));
    std::unique_ptr<wing::TextBox> textBoxPtr(new wing::TextBox(wing::TextBoxCreateParams().Defaults().
        WindowStyle(wing::DefaultChildWindowStyle() | ES_LEFT | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE).
        WindowClassBackgroundColor(COLOR_WINDOW).
        BackgroundColor(wing::Color::White).Text(linesText).Location(textBoxLoc).SetSize(textBoxSize).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    linesTextBox = textBoxPtr.get();
    linesTextBox->TextChanged().AddHandler(this, &TextElementPropertiesDialog::LinesTextChanged);
    linesTextBox->SelectAll();
    std::unique_ptr<wing::PaddedControl> paddedTextBox(new wing::PaddedControl(wing::PaddedControlCreateParams(textBoxPtr.release()).Defaults()));
    std::unique_ptr<wing::BorderedControl> borderedPaddedTextBox(
        new wing::BorderedControl(wing::BorderedControlCreateParams(paddedTextBox.release()).SetBorderStyle(wing::BorderStyle::threeD).
            Location(textBoxLoc).SetSize(textBoxSize).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    AddChild(borderedPaddedTextBox.release());

    wing::Point keywordLabelLoc(defaultControlSpacing.Width, 2 * defaultControlSpacing.Height + textBoxLoc.Y + textBoxSize.Height);
    std::unique_ptr<wing::Label> keywordLabelPtr(new wing::Label(wing::LabelCreateParams().Defaults().Text("Keyword:").
        Location(keywordLabelLoc).SetSize(defaultLabelSize).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    wing::Label* keywordLabel = keywordLabelPtr.get();
    AddChild(keywordLabelPtr.release());

    keyword = textElement->Keyword();
    wing::Point keywordTextBoxLoc(keywordLabelLoc.X + keywordLabel->GetSize().Width + defaultControlSpacing.Width, 
        textBoxLoc.Y + textBoxSize.Height + 2 * defaultControlSpacing.Height);
    std::unique_ptr<wing::TextBox> keywordTextBoxPtr(new wing::TextBox(wing::TextBoxCreateParams().Defaults().Text(keyword).
        SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    keywordTextBox = keywordTextBoxPtr.get();
    keywordTextBox->TextChanged().AddHandler(this, &TextElementPropertiesDialog::KeywordChanged);
    wing::Size keywordTextBoxSize = wing::Size(defaultTextBoxSize.Width, defaultTextBoxSize.Height);
    wing::Rect keywordRect(keywordTextBoxLoc, keywordTextBoxSize);
    wing::Rect paddedKeywordRect = keywordRect;
    paddedKeywordRect.Inflate(textBoxPadding, textBoxPadding);
    std::unique_ptr<wing::Control> paddedKeywordTextBox(new wing::PaddedControl(wing::PaddedControlCreateParams(keywordTextBoxPtr.release()).Defaults()
        .SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    wing::Rect borderedPaddedKeywordTextBoxRect = paddedKeywordRect;
    borderedPaddedKeywordTextBoxRect.Inflate(borderWidth, borderWidth);
    std::unique_ptr<wing::Control> borderedPaddedKeywordTextBox(new wing::BorderedControl(wing::BorderedControlCreateParams(paddedKeywordTextBox.release()).Defaults()
        .Location(wing::Point(borderedPaddedKeywordTextBoxRect.X, borderedPaddedKeywordTextBoxRect.Y)).
        SetSize(wing::Size(borderedPaddedKeywordTextBoxRect.Width, borderedPaddedKeywordTextBoxRect.Height)).
        SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    wing::Control* borderedPaddedKeywordTextBoxControl = borderedPaddedKeywordTextBox.get();
    AddChild(borderedPaddedKeywordTextBox.release());

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
    linesTextBox->SetFocus();
}

void TextElementPropertiesDialog::LinesTextChanged()
{
    std::u32string utext = util::ToUtf32(linesTextBox->Text());
    std::vector<std::u32string> ulines = wing::SplitTextIntoLines(utext);
    lines.clear();
    for (const auto& uline : ulines)
    {
        lines.push_back(util::ToUtf8(uline));
    }
}

void TextElementPropertiesDialog::KeywordChanged()
{
    keyword = keywordTextBox->Text();
}

std::vector<std::string> TextElementPropertiesDialog::Lines() 
{
    return std::move(lines);
}

std::string TextElementPropertiesDialog::Keyword() 
{
    return std::move(keyword);
}

} // namespace springpp
