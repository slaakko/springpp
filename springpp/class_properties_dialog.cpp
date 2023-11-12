// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

module springpp.class_properties_dialog;

import springpp.class_element;
import springpp.edit_diagram_element_list_dialog;

namespace springpp {

ClassPropertiesDialog::ClassPropertiesDialog(ClassElement* classElement_) : wing::Window(wing::WindowCreateParams().Defaults().Text("Class Properties").
    WindowStyle(wing::DialogWindowStyle()).Location(wing::DefaultLocation()).
    WindowClassBackgroundColor(wing::DefaultControlWindowClassBackgroundColor()).BackgroundColor(wing::DefaultControlBackgroundColor()).
    SetSize(wing::Size(wing::ScreenMetrics::Get().MMToHorizontalPixels(100), wing::ScreenMetrics::Get().MMToVerticalPixels(70)))),
    classElement(classElement_), classNameTextBox(nullptr), abstractCheckBox(nullptr), cancelButton(nullptr), okButton(nullptr), 
    editOperationsButton(nullptr), editFieldsButton(nullptr)
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
    int line2Top = classNameLabel->Location().Y + classNameLabel->GetSize().Height + defaultControlSpacing.Height;
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
    AddChild(borderedPaddedClassNameTextBox.release());

    wing::Point abstractCheckBoxLoc(borderedPaddedClassNameTextBoxRect.X, line2Top);
    std::unique_ptr<wing::CheckBox> abstractCheckBoxPtr(new wing::CheckBox(wing::CheckBoxCreateParams().Defaults().Text("Abstract").Location(abstractCheckBoxLoc).
        AutoSize(true).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    abstractCheckBox = abstractCheckBoxPtr.get();
    abstractCheckBox->SetChecked(classElement->IsAbstract());
    abstractCheckBox->CheckedChanged().AddHandler(this, &ClassPropertiesDialog::AbstractChanged);
    AddChild(abstractCheckBoxPtr.release());
    wing::Size abstractCheckBoxSize = abstractCheckBox->GetSize();
    int line3Top = abstractCheckBoxLoc.Y + abstractCheckBoxSize.Height + defaultControlSpacing.Height;
    wing::Point operationsLabelLoc(defaultControlSpacing.Width, line3Top);

    std::unique_ptr<wing::Label> operationsLabel(new wing::Label(wing::LabelCreateParams().Defaults().Text("Operations:").Location(operationsLabelLoc).SetSize(defaultLabelSize).
        SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    wing::Size operationsLabelSize = operationsLabel->GetSize();
    AddChild(operationsLabel.release());
    wing::Point editOperationsButtonLoc(defaultControlSpacing.Width + column1Width + defaultControlSpacing.Width -
        (textBoxPadding + borderWidth), line3Top - (textBoxPadding + borderWidth));
    wing::Size editOperationsButtonSize(wing::ScreenMetrics::Get().MMToHorizontalPixels(8), defaultButtonSize.Height);
    std::unique_ptr<wing::Button> editOperationsButtonPtr(new wing::Button(wing::ControlCreateParams().Defaults().Text("...").Location(editOperationsButtonLoc).
        SetSize(editOperationsButtonSize).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    editOperationsButton = editOperationsButtonPtr.get();
    editOperationsButton->Click().AddHandler(this, &ClassPropertiesDialog::EditOperations);
    AddChild(editOperationsButtonPtr.release());

    int line4Top = operationsLabelLoc.Y + operationsLabelSize.Height + defaultControlSpacing.Height + textBoxPadding + borderWidth;
    wing::Point fieldsLabelLoc(defaultControlSpacing.Width, line4Top);
    std::unique_ptr<wing::Label> fieldsLabel(new wing::Label(wing::LabelCreateParams().Defaults().Text("Fields:").Location(fieldsLabelLoc).SetSize(defaultLabelSize).
        SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    wing::Size fieldsLabelSize = fieldsLabel->GetSize();
    AddChild(fieldsLabel.release());
    wing::Point editFieldsButtonLoc(defaultControlSpacing.Width + column1Width + defaultControlSpacing.Width -
        (textBoxPadding + borderWidth), line4Top - (textBoxPadding + borderWidth));
    wing::Size editFieldsButtonSize(wing::ScreenMetrics::Get().MMToHorizontalPixels(8), defaultButtonSize.Height);
    std::unique_ptr<wing::Button> editFieldsButtonPtr(new wing::Button(wing::ControlCreateParams().Defaults().Text("...").Location(editFieldsButtonLoc).
        SetSize(editFieldsButtonSize).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    editFieldsButton = editFieldsButtonPtr.get();
    editFieldsButton->Click().AddHandler(this, &ClassPropertiesDialog::EditFields);
    AddChild(editFieldsButtonPtr.release());

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
    if (classElement->Name().empty())
    {
        okButton->Disable();
        editOperationsButton->Disable();
        editFieldsButton->Disable();
    }
    AddChild(okButtonPtr.release());
    AddChild(cancelButtonPtr.release());
    SetDefaultButton(okButton);
    SetCancelButton(cancelButton);
    classNameTextBox->SetFocus();
}

void ClassPropertiesDialog::ClassNameChanged()
{
    if (classNameTextBox->Text().empty())
    {
        okButton->Disable();
        editOperationsButton->Disable();
        editFieldsButton->Disable();
    }
    else
    {
        okButton->Enable();
        editOperationsButton->Enable();
        editFieldsButton->Enable();
        classElement->SetName(classNameTextBox->Text());
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
    bool mayHaveAbstractOperations = classElement->IsAbstract();
    IndexList<OperationElement> clonedOperationElements = classElement->Operations().Clone();
    if (ExecuteEditOperationsDialog(clonedOperationElements, classElement, mayHaveAbstractOperations, *this))
    {
        classElement->SetOperations(std::move(clonedOperationElements));
    }
}

void ClassPropertiesDialog::EditFields()
{
    IndexList<FieldElement> clonedFieldElements = classElement->Fields().Clone();
    if (ExecuteEditFieldsDialog(clonedFieldElements, classElement, *this))
    {
        classElement->SetFields(std::move(clonedFieldElements));
    }
}

} // namespace springpp
