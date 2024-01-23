// =================================
// Copyright (c) 2024 Seppo Laakko
// Distributed under the MIT license
// =================================

export module springpp.text_element_properties_dialog;

import wing;
import std.core;

export namespace springpp {

class TextElement;

class TextElementPropertiesDialog : public wing::Window
{
public:
    TextElementPropertiesDialog(const std::string& dialogName, TextElement* textElement_);
    std::vector<std::string> Lines();
    std::string Keyword();
private:
    void LinesTextChanged();
    void KeywordChanged();
    TextElement* textElement;
    wing::TextBox* linesTextBox;
    wing::TextBox* keywordTextBox;
    wing::Button* cancelButton;
    wing::Button* okButton;
    std::vector<std::string> lines;
    std::string keyword;
};

} // namespace springpp
