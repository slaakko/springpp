// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

module springpp.class_properties_dialog;

import springpp.class_element;
import springpp.edit_diagram_element_list_dialog;

namespace springpp {

ClassPropertiesDialog::ClassPropertiesDialog(ClassElement* classElement_, 
    std::map<DiagramElement*, DiagramElement*>& cloneMap_, std::map<DiagramElement*, DiagramElement*>& reverseCloneMap_) :
    wing::Window(wing::WindowCreateParams().Defaults().Text("Class Properties").
    WindowStyle(wing::DialogWindowStyle()).Location(wing::DefaultLocation()).
    WindowClassBackgroundColor(wing::DefaultControlWindowClassBackgroundColor()).BackgroundColor(wing::DefaultControlBackgroundColor()).
    SetSize(wing::Size(wing::ScreenMetrics::Get().MMToHorizontalPixels(100), wing::ScreenMetrics::Get().MMToVerticalPixels(70)))),
    classElement(classElement_), cloneMap(cloneMap_), reverseCloneMap(reverseCloneMap_),
    classNameTextBox(nullptr), abstractCheckBox(nullptr), cancelButton(nullptr), okButton(nullptr), 
    editOperationsButton(nullptr), editAttributesButton(nullptr)
{
    int column1Width = wing::ScreenMetrics::Get().MMToHorizontalPixels(20);
    wing::Size s = GetSize();
    wing::Size defaultButtonSize = wing::ScreenMetrics::Get().DefaultButtonSize();
    wing::Size defaultControlSpacing = wing::ScreenMetrics::Get().DefaultControlSpacing();
    wing::Size defaultLabelSize = wing::ScreenMetrics::Get().DefaultLabelSize();
    wing::Size defaultTextBoxSize = wing::ScreenMetrics::Get().DefaultTextBoxSize();
    int column2Width = wing::ScreenMetrics::Get().MMToHorizontalPixels(30);
    int textBoxPadding = wing::defaultControlPadding;
    int borderWidth = wing::BorderWidth(wing::BorderStyle::single);
    int line1Top = defaultControlSpacing.Height + textBoxPadding + borderWidth;
    wing::Point classNameLabelLoc(defaultControlSpacing.Width, line1Top);
    std::unique_ptr<wing::Label> classNameLabel(new wing::Label(wing::LabelCreateParams().Defaults().Text("Class name:").
        Location(classNameLabelLoc).SetSize(defaultLabelSize).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    AddChild(classNameLabel.release());
    wing::Point classNameTextBoxLoc(defaultControlSpacing.Width * 2 + column1Width, line1Top);
    wing::Size classNameTextBoxSize(column2Width - 2 * (textBoxPadding + borderWidth), defaultTextBoxSize.Height + 4);
    wing::Rect classNameRect(classNameTextBoxLoc, classNameTextBoxSize);
    wing::Rect paddedClassNameRect = classNameRect;
    paddedClassNameRect.Inflate(textBoxPadding, textBoxPadding);
    std::unique_ptr<wing::TextBox> classNameTextBoxPtr(new wing::TextBox(wing::TextBoxCreateParams().Defaults().
        Text(classElement->Name()).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    classNameTextBox = classNameTextBoxPtr.get();
    classNameTextBox->SelectAll();
    classNameTextBox->TextChanged().AddHandler(this, &ClassPropertiesDialog::ClassNameChanged);
    std::unique_ptr<wing::Control> paddedClassNameTextBox(new wing::PaddedControl(wing::PaddedControlCreateParams(classNameTextBoxPtr.release()).Defaults()
        .SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    wing::Rect borderedPaddedClassNameTextBoxRect = paddedClassNameRect;
    borderedPaddedClassNameTextBoxRect.Inflate(borderWidth, borderWidth);
    std::unique_ptr<wing::Control> borderedPaddedClassNameTextBox(new wing::BorderedControl(wing::BorderedControlCreateParams(paddedClassNameTextBox.release()).Defaults()
        .Location(wing::Point(borderedPaddedClassNameTextBoxRect.X, borderedPaddedClassNameTextBoxRect.Y)).
        SetSize(wing::Size(borderedPaddedClassNameTextBoxRect.Width, borderedPaddedClassNameTextBoxRect.Height)).
        SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    wing::Control* borderedPaddedClassNameTextBoxControl = borderedPaddedClassNameTextBox.get();
    AddChild(borderedPaddedClassNameTextBox.release());

    int line2Top = borderedPaddedClassNameTextBoxControl->Location().Y + borderedPaddedClassNameTextBoxControl->GetSize().Height + defaultControlSpacing.Height;

    wing::Point keywordLabelLoc(defaultControlSpacing.Width, line2Top);
    std::unique_ptr<wing::Label> keywordLabel(new wing::Label(wing::LabelCreateParams().Defaults().Text("Keyword:").
        Location(keywordLabelLoc).SetSize(defaultLabelSize).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    AddChild(keywordLabel.release());

    wing::Point keywordTextBoxLoc(defaultControlSpacing.Width * 2 + column1Width, line2Top);
    wing::Size keywordTextBoxSize(column2Width - 2 * (textBoxPadding + borderWidth), defaultTextBoxSize.Height + 4);
    wing::Rect keywordRect(keywordTextBoxLoc, keywordTextBoxSize);
    wing::Rect paddedKeywordRect = keywordRect;
    paddedKeywordRect.Inflate(textBoxPadding, textBoxPadding);
    std::unique_ptr<wing::TextBox> keywordTextBoxPtr(new wing::TextBox(wing::TextBoxCreateParams().Defaults().
        Text(classElement->Keyword()).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    keywordTextBox = keywordTextBoxPtr.get();
    keywordTextBox->TextChanged().AddHandler(this, &ClassPropertiesDialog::KeywordChanged);
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

    int line3Top = borderedPaddedKeywordTextBoxControl->Location().Y + borderedPaddedKeywordTextBoxControl->GetSize().Height + defaultControlSpacing.Height;

    wing::Point abstractCheckBoxLoc(borderedPaddedClassNameTextBoxRect.X, line3Top);
    std::unique_ptr<wing::CheckBox> abstractCheckBoxPtr(new wing::CheckBox(wing::CheckBoxCreateParams().Defaults().Text("Abstract").Location(abstractCheckBoxLoc).
        AutoSize(true).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    abstractCheckBox = abstractCheckBoxPtr.get();
    abstractCheckBox->SetChecked(classElement->IsAbstract());
    abstractCheckBox->CheckedChanged().AddHandler(this, &ClassPropertiesDialog::AbstractChanged);
    AddChild(abstractCheckBoxPtr.release());
    wing::Size abstractCheckBoxSize = abstractCheckBox->GetSize();

    int line4Top = abstractCheckBoxLoc.Y + abstractCheckBoxSize.Height + defaultControlSpacing.Height;
    wing::Point attributesLabelLoc(defaultControlSpacing.Width, line4Top);
    std::unique_ptr<wing::Label> attributesLabel(new wing::Label(wing::LabelCreateParams().Defaults().Text("Attributes:").Location(attributesLabelLoc).SetSize(defaultLabelSize).
        SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    wing::Size attributesLabelSize = attributesLabel->GetSize();
    AddChild(attributesLabel.release());
    wing::Point editAttributesButtonLoc(defaultControlSpacing.Width + column1Width + defaultControlSpacing.Width -
        (textBoxPadding + borderWidth), line4Top - (textBoxPadding + borderWidth));
    wing::Size editAttributesButtonSize(wing::ScreenMetrics::Get().MMToHorizontalPixels(8), defaultButtonSize.Height);
    std::unique_ptr<wing::Button> editAttributesButtonPtr(new wing::Button(wing::ControlCreateParams().Defaults().Text("...").Location(editAttributesButtonLoc).
        SetSize(editAttributesButtonSize).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    editAttributesButton = editAttributesButtonPtr.get();
    editAttributesButton->Click().AddHandler(this, &ClassPropertiesDialog::EditAttributes);
    AddChild(editAttributesButtonPtr.release());

    int line5Top = attributesLabelLoc.Y + attributesLabelSize.Height + defaultControlSpacing.Height + textBoxPadding + borderWidth;
    wing::Point operationsLabelLoc(defaultControlSpacing.Width, line5Top);
    std::unique_ptr<wing::Label> operationsLabel(new wing::Label(wing::LabelCreateParams().Defaults().Text("Operations:").Location(operationsLabelLoc).SetSize(defaultLabelSize).
        SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    wing::Size operationsLabelSize = operationsLabel->GetSize();
    AddChild(operationsLabel.release());
    wing::Point editOperationsButtonLoc(defaultControlSpacing.Width + column1Width + defaultControlSpacing.Width -
        (textBoxPadding + borderWidth), line5Top - (textBoxPadding + borderWidth));
    wing::Size editOperationsButtonSize(wing::ScreenMetrics::Get().MMToHorizontalPixels(8), defaultButtonSize.Height);
    std::unique_ptr<wing::Button> editOperationsButtonPtr(new wing::Button(wing::ControlCreateParams().Defaults().Text("...").Location(editOperationsButtonLoc).
        SetSize(editOperationsButtonSize).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    editOperationsButton = editOperationsButtonPtr.get();
    editOperationsButton->Click().AddHandler(this, &ClassPropertiesDialog::EditOperations);
    AddChild(editOperationsButtonPtr.release());

    int x = s.Width - defaultButtonSize.Width - defaultControlSpacing.Width;
    int y = s.Height - defaultButtonSize.Height - defaultControlSpacing.Height;
    std::unique_ptr<wing::Button> cancelButtonPtr(new wing::Button(wing::ControlCreateParams().Defaults().Text("Cancel").Location(wing::Point(x, y)).
        SetSize(defaultButtonSize).SetDock(wing::Dock::none).SetAnchors(static_cast<wing::Anchors>(wing::Anchors::bottom | wing::Anchors::right))));
    cancelButton = cancelButtonPtr.get();
    cancelButton->SetDialogResult(wing::DialogResult::cancel);
    x = x - defaultButtonSize.Width - defaultControlSpacing.Width;

    std::unique_ptr<wing::Button> okButtonPtr(new wing::Button(wing::ControlCreateParams().Defaults().Text("OK").Location(wing::Point(x, y)).
        SetSize(defaultButtonSize).SetDock(wing::Dock::none).SetAnchors(static_cast<wing::Anchors>(wing::Anchors::bottom | wing::Anchors::right))));
    okButton = okButtonPtr.get();
    okButton->SetDialogResult(wing::DialogResult::ok);
    if (classElement->Name().empty() && classElement->Keyword().empty())
    {
        okButton->Disable();
        editOperationsButton->Disable();
        editAttributesButton->Disable();
    }
    AddChild(okButtonPtr.release());
    AddChild(cancelButtonPtr.release());
    SetDefaultButton(okButton);
    SetCancelButton(cancelButton);
    classNameTextBox->SetFocus();
}

void ClassPropertiesDialog::ClassNameChanged()
{
    if (classNameTextBox->Text().empty() && keywordTextBox->Text().empty())
    {
        okButton->Disable();
        editOperationsButton->Disable();
        editAttributesButton->Disable();
        classElement->SetName(std::string());
    }
    else
    {
        okButton->Enable();
        editOperationsButton->Enable();
        editAttributesButton->Enable();
        classElement->SetName(classNameTextBox->Text());
    }
}

void ClassPropertiesDialog::KeywordChanged()
{
    if (classNameTextBox->Text().empty() && keywordTextBox->Text().empty())
    {
        okButton->Disable();
        editOperationsButton->Disable();
        editAttributesButton->Disable();
        classElement->SetKeyword(std::string());
    }
    else
    {
        okButton->Enable();
        editOperationsButton->Enable();
        editAttributesButton->Enable();
        classElement->SetKeyword(keywordTextBox->Text());
    }
}

void ClassPropertiesDialog::AbstractChanged()
{
    bool isAbstract = abstractCheckBox->Checked();
    if (isAbstract)
    {
        classElement->SetAbstract();
    }
    else
    {
        classElement->ResetAbstract();
    }
}

void ClassPropertiesDialog::EditOperations()
{
    std::map<DiagramElement*, DiagramElement*> prevCloneMap = cloneMap;
    std::map<DiagramElement*, DiagramElement*> prevReverseCloneMap = reverseCloneMap;
    bool mayHaveAbstractOperations = classElement->IsAbstract();
    if (!ExecuteEditOperationsDialog(classElement->Operations(), cloneMap, reverseCloneMap, classElement, mayHaveAbstractOperations, *this))
    {
        cloneMap = prevCloneMap;
        reverseCloneMap = prevReverseCloneMap;
    }
}

void ClassPropertiesDialog::EditAttributes()
{
    std::map<DiagramElement*, DiagramElement*> prevCloneMap = cloneMap;
    std::map<DiagramElement*, DiagramElement*> prevReverseCloneMap = reverseCloneMap;
    if (!ExecuteEditAttributesDialog(classElement->Attributes(), cloneMap, reverseCloneMap, classElement, *this))
    {
        cloneMap = prevCloneMap;
        reverseCloneMap = prevReverseCloneMap;
    }
}

} // namespace springpp
