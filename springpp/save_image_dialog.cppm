// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

export module springpp.save_image_dialog;

import springpp.layout;
import wing;
import std.core;

export namespace springpp {

class SaveImageDialog : public wing::Window
{
public:
    SaveImageDialog(const std::string& diagramFileName_);
    const std::string& FileName() const { return fileName; }
    const Padding& Margins() const { return margins; }
    wing::ImageFormat Format() const { return format; }
private:
    void FormatRadioButtonStateChanged();
    void LeftChanged();
    void TopChanged();
    void RightChanged();
    void BottomChanged();
    void FileNameChanged();
    void Browse();
    std::string diagramFileName;
    std::string fileName;
    wing::ImageFormat format;
    Padding margins;
    wing::GroupBox* formatGroupBox;
    wing::RadioButton* pngRadioButton;
    wing::RadioButton* jpgRadioButton;
    wing::RadioButton* gifRadioButton;
    wing::RadioButton* tiffRadioButton;
    wing::RadioButton* bmpRadioButton;
    wing::GroupBox* marginsGroupBox;
    wing::TextBox* leftTextBox;
    wing::TextBox* topTextBox;
    wing::TextBox* rightTextBox;
    wing::TextBox* bottomTextBox;
    wing::TextBox* fileNameTextBox;
    wing::Button* browseButton;
    wing::Button* okButton;
    wing::Button* cancelButton;
    bool ready;
};

} // namespace springpp
