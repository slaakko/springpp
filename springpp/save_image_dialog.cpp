// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

module;
#include <Windows.h>
#undef MessageBox

module springpp.save_image_dialog;

import springpp.configuration;
import util;
import std.filesystem;

namespace springpp {

SaveImageDialog::SaveImageDialog(const std::string& diagramFileName_) : wing::Window(wing::WindowCreateParams().Defaults().Text("Save Image").
    WindowStyle(wing::DialogWindowStyle()).Location(wing::DefaultLocation()).
    WindowClassBackgroundColor(wing::DefaultControlWindowClassBackgroundColor()).BackgroundColor(wing::DefaultControlBackgroundColor()).
    SetSize(wing::Size(wing::ScreenMetrics::Get().MMToHorizontalPixels(100), wing::ScreenMetrics::Get().MMToVerticalPixels(100)))),
    diagramFileName(diagramFileName_), pngRadioButton(nullptr), jpgRadioButton(nullptr), 
    leftTextBox(nullptr), topTextBox(nullptr), rightTextBox(nullptr), bottomTextBox(nullptr), fileNameTextBox(nullptr), 
    browseButton(nullptr), okButton(nullptr), cancelButton(nullptr), ready(false)
{
    if (!diagramFileName.empty())
    {
        fileName = util::Path::ChangeExtension(diagramFileName, ".png");
    }
    format = wing::ImageFormat::png;
    wing::Size s = GetSize();
    wing::Size defaultButtonSize = wing::ScreenMetrics::Get().DefaultButtonSize();
    wing::Size defaultControlSpacing = wing::ScreenMetrics::Get().DefaultControlSpacing();
    wing::Size defaultLabelSize = wing::ScreenMetrics::Get().DefaultLabelSize();
    wing::Size defaultTextBoxSize = wing::ScreenMetrics::Get().DefaultTextBoxSize();
    Layout* layout = Configuration::Instance().GetLayout();
    margins = layout->GetDiagramPaddingElement()->GetPadding();
    int labelColumnWidth = wing::ScreenMetrics::Get().MMToHorizontalPixels(15);
    std::unique_ptr<wing::GroupBox> formatGroupBoxPtr(new wing::GroupBox(wing::GroupBoxCreateParams().Defaults().Text("Format").
        Location(wing::Point(defaultControlSpacing.Width, defaultControlSpacing.Height)).
        SetSize(wing::Size(wing::ScreenMetrics::Get().MMToHorizontalPixels(30), wing::ScreenMetrics::Get().MMToVerticalPixels(30))).
        SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    formatGroupBox = formatGroupBoxPtr.get();
    AddChild(formatGroupBoxPtr.release());
    wing::Point formatGroupBoxLoc(formatGroupBox->Location());
    std::unique_ptr<wing::RadioButton> pngRadioButtonPtr(new wing::RadioButton(wing::RadioButtonCreateParams().Defaults().Text("PNG").
        Location(wing::Point(formatGroupBoxLoc.X, formatGroupBoxLoc.Y + defaultControlSpacing.Height)).AutoSize(true).
        SetAnchors((wing::Anchors::top | wing::Anchors::left))));
    pngRadioButton = pngRadioButtonPtr.get();
    pngRadioButton->CheckedChanged().AddHandler(this, &SaveImageDialog::FormatRadioButtonStateChanged);
    formatGroupBox->AddChild(pngRadioButtonPtr.release());
    std::unique_ptr<wing::RadioButton> jpgRadioButtonPtr(new wing::RadioButton(wing::RadioButtonCreateParams().Defaults().Text("JPEG").
        Location(wing::Point(formatGroupBoxLoc.X, pngRadioButton->Location().Y + pngRadioButton->GetSize().Height)).AutoSize(true).
        SetAnchors((wing::Anchors::top | wing::Anchors::left))));
    jpgRadioButton = jpgRadioButtonPtr.get();
    jpgRadioButton->CheckedChanged().AddHandler(this, &SaveImageDialog::FormatRadioButtonStateChanged);
    formatGroupBox->AddChild(jpgRadioButtonPtr.release());
    std::unique_ptr<wing::RadioButton> gifRadioButtonPtr(new wing::RadioButton(wing::RadioButtonCreateParams().Defaults().Text("GIF").
        Location(wing::Point(formatGroupBoxLoc.X, jpgRadioButton->Location().Y + jpgRadioButton->GetSize().Height)).AutoSize(true).
        SetAnchors((wing::Anchors::top | wing::Anchors::left))));
    gifRadioButton = gifRadioButtonPtr.get();
    gifRadioButton->CheckedChanged().AddHandler(this, &SaveImageDialog::FormatRadioButtonStateChanged);
    formatGroupBox->AddChild(gifRadioButtonPtr.release());
    std::unique_ptr<wing::RadioButton> tiffRadioButtonPtr(new wing::RadioButton(wing::RadioButtonCreateParams().Defaults().Text("TIFF").
        Location(wing::Point(formatGroupBoxLoc.X, gifRadioButton->Location().Y + gifRadioButton->GetSize().Height)).AutoSize(true).
        SetAnchors((wing::Anchors::top | wing::Anchors::left))));
    tiffRadioButton = tiffRadioButtonPtr.get();
    tiffRadioButton->CheckedChanged().AddHandler(this, &SaveImageDialog::FormatRadioButtonStateChanged);
    formatGroupBox->AddChild(tiffRadioButtonPtr.release());
    std::unique_ptr<wing::RadioButton> bmpRadioButtonPtr(new wing::RadioButton(wing::RadioButtonCreateParams().Defaults().Text("BMP").
        Location(wing::Point(formatGroupBoxLoc.X, tiffRadioButton->Location().Y + tiffRadioButton->GetSize().Height)).AutoSize(true).
        SetAnchors((wing::Anchors::top | wing::Anchors::left))));
    bmpRadioButton = bmpRadioButtonPtr.get();
    bmpRadioButton->CheckedChanged().AddHandler(this, &SaveImageDialog::FormatRadioButtonStateChanged);
    formatGroupBox->AddChild(bmpRadioButtonPtr.release());
    std::unique_ptr<wing::GroupBox> marginsGroupBoxPtr(new wing::GroupBox(wing::GroupBoxCreateParams().Defaults().Text("Margins").
        Location(wing::Point(formatGroupBox->Location().X + formatGroupBox->GetSize().Width + defaultControlSpacing.Width, defaultControlSpacing.Height)).
        SetSize(wing::Size(wing::ScreenMetrics::Get().MMToHorizontalPixels(50), wing::ScreenMetrics::Get().MMToVerticalPixels(40))).
        SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    marginsGroupBox = marginsGroupBoxPtr.get();
    AddChild(marginsGroupBoxPtr.release());
    wing::Point leftLabelLoc(defaultControlSpacing.Width, marginsGroupBox->Location().Y + defaultControlSpacing.Height);
    std::unique_ptr<wing::Label> leftLabelPtr(new wing::Label(wing::LabelCreateParams().Defaults().
        Text("Left:").
        Location(wing::Point(leftLabelLoc.X, leftLabelLoc.Y + defaultControlSpacing.Height / 2)).
        AutoSize(true).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    wing::Label* leftLabel = leftLabelPtr.get();
    marginsGroupBox->AddChild(leftLabelPtr.release());
    wing::Point leftTextBoxLoc(leftLabelLoc.X + labelColumnWidth, leftLabelLoc.Y);
    wing::Size textBoxSize(wing::Size(wing::ScreenMetrics::Get().MMToHorizontalPixels(20), defaultTextBoxSize.Height + 4 + 2 * wing::defaultControlPadding +
        2 * wing::BorderWidth(wing::BorderStyle::single)));
    std::unique_ptr<wing::TextBox> leftTextBoxPtr(new wing::TextBox(wing::TextBoxCreateParams().Defaults().Text(std::to_string(margins.left))));
    leftTextBox = leftTextBoxPtr.get();
    leftTextBox->TextChanged().AddHandler(this, &SaveImageDialog::LeftChanged);
    std::unique_ptr<wing::PaddedControl> paddedLeftTextBoxPtr(new wing::PaddedControl(wing::PaddedControlCreateParams(leftTextBoxPtr.release()).Defaults().
        SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    std::unique_ptr<wing::BorderedControl> borderedLeftTextBoxPtr(new wing::BorderedControl(wing::BorderedControlCreateParams(paddedLeftTextBoxPtr.release()).Defaults().
        Location(leftTextBoxLoc).SetSize(textBoxSize).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    marginsGroupBox->AddChild(borderedLeftTextBoxPtr.release());
    std::unique_ptr<wing::Label> leftMMLabelPtr(new wing::Label(wing::LabelCreateParams().Defaults().Text("mm").
        Location(wing::Point(leftTextBoxLoc.X + textBoxSize.Width + defaultControlSpacing.Width, leftTextBoxLoc.Y + textBoxSize.Height - defaultLabelSize.Height)).
        SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    marginsGroupBox->AddChild(leftMMLabelPtr.release());

    wing::Point topTextBoxLoc(leftTextBoxLoc.X, leftTextBoxLoc.Y + textBoxSize.Height + defaultControlSpacing.Height);
    std::unique_ptr<wing::Label> topLabelPtr(new wing::Label(wing::LabelCreateParams().Defaults().
        Text("Top:").
        Location(wing::Point(defaultControlSpacing.Width, topTextBoxLoc.Y + textBoxSize.Height - defaultLabelSize.Height)).
        AutoSize(true).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    wing::Label* topLabel = topLabelPtr.get();
    marginsGroupBox->AddChild(topLabelPtr.release());
    std::unique_ptr<wing::TextBox> topTextBoxPtr(new wing::TextBox(wing::TextBoxCreateParams().Defaults().Text(std::to_string(margins.top))));
    topTextBox = topTextBoxPtr.get();
    topTextBox->TextChanged().AddHandler(this, &SaveImageDialog::TopChanged);
    std::unique_ptr<wing::PaddedControl> paddedTopTextBoxPtr(new wing::PaddedControl(wing::PaddedControlCreateParams(topTextBoxPtr.release()).Defaults().
        SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    std::unique_ptr<wing::BorderedControl> borderedTopTextBoxPtr(new wing::BorderedControl(wing::BorderedControlCreateParams(paddedTopTextBoxPtr.release()).Defaults().
        Location(topTextBoxLoc).SetSize(textBoxSize).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    marginsGroupBox->AddChild(borderedTopTextBoxPtr.release());
    std::unique_ptr<wing::Label> topMMLabelPtr(new wing::Label(wing::LabelCreateParams().Defaults().Text("mm").
        Location(wing::Point(topTextBoxLoc.X + textBoxSize.Width + defaultControlSpacing.Width, topTextBoxLoc.Y + textBoxSize.Height - defaultLabelSize.Height)).
        SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    marginsGroupBox->AddChild(topMMLabelPtr.release());

    wing::Point rightTextBoxLoc(leftTextBoxLoc.X, topTextBoxLoc.Y + textBoxSize.Height + defaultControlSpacing.Height);
    std::unique_ptr<wing::Label> rightLabelPtr(new wing::Label(wing::LabelCreateParams().Defaults().
        Text("Right:").
        Location(wing::Point(defaultControlSpacing.Width, rightTextBoxLoc.Y + textBoxSize.Height - defaultLabelSize.Height)).
        AutoSize(true).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    wing::Label* rightLabel = rightLabelPtr.get();
    marginsGroupBox->AddChild(rightLabelPtr.release());
    std::unique_ptr<wing::TextBox> rightTextBoxPtr(new wing::TextBox(wing::TextBoxCreateParams().Defaults().Text(std::to_string(margins.right))));
    rightTextBox = rightTextBoxPtr.get();
    rightTextBox->TextChanged().AddHandler(this, &SaveImageDialog::RightChanged);
    std::unique_ptr<wing::PaddedControl> paddedRightTextBoxPtr(new wing::PaddedControl(wing::PaddedControlCreateParams(rightTextBoxPtr.release()).Defaults().
        SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    std::unique_ptr<wing::BorderedControl> borderedRightTextBoxPtr(new wing::BorderedControl(wing::BorderedControlCreateParams(paddedRightTextBoxPtr.release()).Defaults().
        Location(rightTextBoxLoc).SetSize(textBoxSize).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    marginsGroupBox->AddChild(borderedRightTextBoxPtr.release());
    std::unique_ptr<wing::Label> rightMMLabelPtr(new wing::Label(wing::LabelCreateParams().Defaults().Text("mm").
        Location(wing::Point(rightTextBoxLoc.X + textBoxSize.Width + defaultControlSpacing.Width, rightTextBoxLoc.Y + textBoxSize.Height - defaultLabelSize.Height)).
        SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    marginsGroupBox->AddChild(rightMMLabelPtr.release());

    wing::Point bottomTextBoxLoc(leftTextBoxLoc.X, rightTextBoxLoc.Y + textBoxSize.Height + defaultControlSpacing.Height);
    std::unique_ptr<wing::Label> bottomLabelPtr(new wing::Label(wing::LabelCreateParams().Defaults().
        Text("Bottom:").
        Location(wing::Point(defaultControlSpacing.Width, bottomTextBoxLoc.Y + textBoxSize.Height - defaultLabelSize.Height)).
        AutoSize(true).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    wing::Label* bottomLabel = bottomLabelPtr.get();
    marginsGroupBox->AddChild(bottomLabelPtr.release());
    std::unique_ptr<wing::TextBox> bottomTextBoxPtr(new wing::TextBox(wing::TextBoxCreateParams().Defaults().Text(std::to_string(margins.bottom))));
    bottomTextBox = bottomTextBoxPtr.get();
    bottomTextBox->TextChanged().AddHandler(this, &SaveImageDialog::BottomChanged);
    std::unique_ptr<wing::PaddedControl> paddedBottomTextBoxPtr(new wing::PaddedControl(wing::PaddedControlCreateParams(bottomTextBoxPtr.release()).Defaults().
        SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    std::unique_ptr<wing::BorderedControl> borderedBottomTextBoxPtr(new wing::BorderedControl(wing::BorderedControlCreateParams(paddedBottomTextBoxPtr.release()).Defaults().
        Location(bottomTextBoxLoc).SetSize(textBoxSize).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    marginsGroupBox->AddChild(borderedBottomTextBoxPtr.release());
    std::unique_ptr<wing::Label> bottomMMLabelPtr(new wing::Label(wing::LabelCreateParams().Defaults().Text("mm").
        Location(wing::Point(bottomTextBoxLoc.X + textBoxSize.Width + defaultControlSpacing.Width, bottomTextBoxLoc.Y + textBoxSize.Height - defaultLabelSize.Height)).
        SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    marginsGroupBox->AddChild(bottomMMLabelPtr.release());

    wing::Point fileNameLabelLoc(defaultControlSpacing.Width, marginsGroupBox->Location().Y + marginsGroupBox->GetSize().Height + defaultControlSpacing.Height +
        textBoxSize.Height - defaultLabelSize.Height);
    std::unique_ptr<wing::Label> fileNameLabelPtr(new wing::Label(wing::LabelCreateParams().Defaults().
        Text("Image file name:").Location(fileNameLabelLoc).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    wing::Label* fileNameLabel = fileNameLabelPtr.get();
    AddChild(fileNameLabelPtr.release());
    wing::Point fileNameTextBoxLoc(defaultControlSpacing.Width, fileNameLabelLoc.Y + fileNameLabel->GetSize().Height + defaultControlSpacing.Height);
    std::unique_ptr<wing::TextBox> fileNameTextBoxPtr(new wing::TextBox(wing::TextBoxCreateParams().Defaults().Text(fileName)));
    fileNameTextBox = fileNameTextBoxPtr.get();
    fileNameTextBox->TextChanged().AddHandler(this, &SaveImageDialog::FileNameChanged);
    std::unique_ptr<wing::PaddedControl> paddedFileNameTextBoxPtr(new wing::PaddedControl(wing::PaddedControlCreateParams(fileNameTextBoxPtr.release()).Defaults()));
    std::unique_ptr<wing::BorderedControl> borderedFileNameTextBoxPtr(new wing::BorderedControl(wing::BorderedControlCreateParams(paddedFileNameTextBoxPtr.release()).
        Defaults().Location(fileNameTextBoxLoc).SetSize(wing::Size(wing::ScreenMetrics::Get().MMToHorizontalPixels(80), textBoxSize.Height)).
        SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    AddChild(borderedFileNameTextBoxPtr.release());

    wing::Point browseButtonLoc(fileNameTextBoxLoc.X + fileNameTextBox->GetSize().Width - defaultButtonSize.Width,
        fileNameTextBoxLoc.Y + textBoxSize.Height + defaultControlSpacing.Height);
    std::unique_ptr<wing::Button> browseButtonPtr(new wing::Button(wing::ControlCreateParams().Defaults().Text("Browse").Location(browseButtonLoc).SetSize(defaultButtonSize).
        SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    browseButton = browseButtonPtr.get();
    browseButton->Click().AddHandler(this, &SaveImageDialog::Browse);
    AddChild(browseButtonPtr.release());

    int x = s.Width - defaultButtonSize.Width - defaultControlSpacing.Width;
    int y = s.Height - defaultButtonSize.Height - defaultControlSpacing.Height;

    std::unique_ptr<wing::Button> cancelButtonPtr(new wing::Button(wing::ControlCreateParams().Defaults().Text("Cancel").Location(wing::Point(x, y)).SetSize(defaultButtonSize).
        SetAnchors(wing::Anchors::bottom | wing::Anchors::right)));
    cancelButton = cancelButtonPtr.get();
    cancelButton->SetDialogResult(wing::DialogResult::cancel);
    x = x - defaultButtonSize.Width - defaultControlSpacing.Width;

    std::unique_ptr<wing::Button> okButtonPtr(new wing::Button(wing::ControlCreateParams().Defaults().Text("OK").Location(wing::Point(x, y)).SetSize(defaultButtonSize).
        SetAnchors(wing::Anchors::bottom | wing::Anchors::right)));
    okButton = okButtonPtr.get();
    okButton->SetDialogResult(wing::DialogResult::ok);
    AddChild(okButtonPtr.release());
    AddChild(cancelButtonPtr.release());
    if (fileName.empty())
    {
        okButton->Disable();
    }
    else
    {
        fileNameTextBox->SetText(fileName);
    }
    SetDefaultButton(okButton);
    SetCancelButton(cancelButton);
    ready = true;
    fileNameTextBox->SetFocus();
}

void SaveImageDialog::FormatRadioButtonStateChanged()
{
    if (!ready) return;
    wing::RadioButton* checkedRadioButton = formatGroupBox->CheckedRadioButton();
    if (checkedRadioButton == pngRadioButton)
    {
        format = wing::ImageFormat::png;
    }
    else if (checkedRadioButton == jpgRadioButton)
    {
        format = wing::ImageFormat::jpeg;
    }
    else if (checkedRadioButton == gifRadioButton)
    {
        format = wing::ImageFormat::gif;
    }
    else if (checkedRadioButton == tiffRadioButton)
    {
        format = wing::ImageFormat::tiff;
    }
    else if (checkedRadioButton == bmpRadioButton)
    {
        format = wing::ImageFormat::bmp;
    }
    if (!fileName.empty())
    {
        switch (format)
        {
        case wing::ImageFormat::png:
        {
            fileName = util::Path::ChangeExtension(fileName, ".png");
            break;
        }
        case wing::ImageFormat::jpeg:
        {
            fileName = util::Path::ChangeExtension(fileName, ".jpg");
            break;
        }
        case wing::ImageFormat::gif:
        {
            fileName = util::Path::ChangeExtension(fileName, ".gif");
            break;
        }
        case wing::ImageFormat::tiff:
        {
            fileName = util::Path::ChangeExtension(fileName, ".tiff");
            break;
        }
        case wing::ImageFormat::bmp:
        {
            fileName = util::Path::ChangeExtension(fileName, ".bmp");
            break;
        }
        }
        if (fileNameTextBox != nullptr)
        {
            fileNameTextBox->SetText(fileName);
        }
    }
}

void SaveImageDialog::LeftChanged()
{
    bool disable = false;
    try
    {
        std::string left = leftTextBox->Text();
        if (left.empty())
        {
            disable = true;
        }
        else
        {
            margins.left = std::stof(left);
            disable = false;
        }
    }
    catch (const std::exception&)
    {
        disable = true;
    }
    if (disable)
    {
        okButton->Disable();
    }
    else
    {
        okButton->Enable();
    }
}

void SaveImageDialog::TopChanged()
{
    bool disable = false;
    try
    {
        std::string top = topTextBox->Text();
        if (top.empty())
        {
            disable = true;
        }
        else
        {
            margins.top = std::stof(top);
            disable = false;
        }
    }
    catch (const std::exception&)
    {
        disable = true;
    }
    if (disable)
    {
        okButton->Disable();
    }
    else
    {
        okButton->Enable();
    }
}

void SaveImageDialog::RightChanged()
{
    bool disable = false;
    try
    {
        std::string right = rightTextBox->Text();
        if (right.empty())
        {
            disable = true;
        }
        else
        {
            margins.right = std::stof(right);
            disable = false;
        }
    }
    catch (const std::exception&)
    {
        disable = true;
    }
    if (disable)
    {
        okButton->Disable();
    }
    else
    {
        okButton->Enable();
    }
}

void SaveImageDialog::BottomChanged()
{
    bool disable = false;
    try
    {
        std::string bottom = bottomTextBox->Text();
        if (bottom.empty())
        {
            disable = true;
        }
        else
        {
            margins.bottom = std::stof(bottom);
            disable = false;
        }
    }
    catch (const std::exception&)
    {
        disable = true;
    }
    if (disable)
    {
        okButton->Disable();
    }
    else
    {
        okButton->Enable();
    }
}

void SaveImageDialog::FileNameChanged()
{
    bool disable = false;
    fileName = fileNameTextBox->Text();
    if (fileName.empty())
    {
        disable = true;
    }
    else
    {
        disable = false;
    }
    if (disable)
    {
        okButton->Disable();
    }
    else
    {
        okButton->Enable();
    }
}

void SaveImageDialog::Browse()
{
    try
    {
        std::vector<std::pair<std::string, std::string>> descriptionFilterPairs;
        descriptionFilterPairs.push_back(std::make_pair("PNG files (*.png)", "*.png"));
        descriptionFilterPairs.push_back(std::make_pair("JPG files (*.jpg)", "*.jpg"));
        descriptionFilterPairs.push_back(std::make_pair("GIF files (*.gif)", "*.gif"));
        descriptionFilterPairs.push_back(std::make_pair("TIFF files (*.tiff)", "*.tiff"));
        descriptionFilterPairs.push_back(std::make_pair("BMP files (*.bmp)", "*.bmp"));
        std::string initialDirectory = DiagramDirectory();
        std::vector<std::string> fileNames;
        std::string defaultFilePath = fileName;
        std::string filePath;
        bool selected = wing::SaveFileName(Handle(), descriptionFilterPairs, initialDirectory, 0, ".png", filePath);
        if (selected)
        {
            if (std::filesystem::exists(filePath))
            {
                if (wing::MessageBox::Show("File '" + filePath + "' already exists. Do you want to replace it?", "File Exists", this, MB_YESNO) != wing::MessageBoxResult::yes)
                {
                    return;
                }
            }
            fileName = filePath;
            fileNameTextBox->SetText(fileName);
        }
    }
    catch (const std::exception& ex)
    {
        wing::ShowErrorMessageBox(nullptr, ex.what());
    }
}

} // namespace springpp
