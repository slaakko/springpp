// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

module springpp.object_properties_dialog;

import springpp.edit_diagram_element_list_dialog;
import springpp.object_element;

namespace springpp {

ObjectPropertiesDialog::ObjectPropertiesDialog(ObjectElement* objectElement_,
    std::map<DiagramElement*, DiagramElement*>& cloneMap_, std::map<DiagramElement*, DiagramElement*>& reverseCloneMap_) : 
    wing::Window(wing::WindowCreateParams().Defaults().Text("Object Properties").
    WindowStyle(wing::DialogWindowStyle()).Location(wing::DefaultLocation()).
    WindowClassBackgroundColor(wing::DefaultControlWindowClassBackgroundColor()).BackgroundColor(wing::DefaultControlBackgroundColor()).
    SetSize(wing::Size(wing::ScreenMetrics::Get().MMToHorizontalPixels(100), wing::ScreenMetrics::Get().MMToVerticalPixels(70)))),
    objectElement(objectElement_), cloneMap(cloneMap_), reverseCloneMap(reverseCloneMap_),
    objectNameTextBox(nullptr), cancelButton(nullptr), okButton(nullptr), editAttributesButton(nullptr)
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
    wing::Point objectNameLabelLoc(defaultControlSpacing.Width, line1Top);
    std::unique_ptr<wing::Label> objectNameLabel(new wing::Label(wing::LabelCreateParams().Defaults().Text("Object name:").
        Location(objectNameLabelLoc).SetSize(defaultLabelSize).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    int line2Top = objectNameLabel->Location().Y + objectNameLabel->GetSize().Height + defaultControlSpacing.Height;
    AddChild(objectNameLabel.release());
    wing::Point objectNameTextBoxLoc(defaultControlSpacing.Width * 2 + column1Width, line1Top);
    wing::Size objectNameTextBoxSize(column2Width - 2 * (textBoxPadding + borderWidth), defaultTextBoxSize.Height);
    wing::Rect objectNameRect(objectNameTextBoxLoc, objectNameTextBoxSize);
    wing::Rect paddedObjectNameRect = objectNameRect;
    paddedObjectNameRect.Inflate(textBoxPadding, textBoxPadding);
    std::unique_ptr<wing::TextBox> objectNameTextBoxPtr(new wing::TextBox(wing::TextBoxCreateParams().Defaults().
        Text(objectElement->Name()).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    objectNameTextBox = objectNameTextBoxPtr.get();
    objectNameTextBox->SelectAll();
    objectNameTextBox->TextChanged().AddHandler(this, &ObjectPropertiesDialog::ObjectNameChanged);
    std::unique_ptr<wing::Control> paddedObjectNameTextBox(new wing::PaddedControl(wing::PaddedControlCreateParams(objectNameTextBoxPtr.release()).Defaults().
        SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    wing::Rect borderedPaddedObjectNameTextBoxRect = paddedObjectNameRect;
    borderedPaddedObjectNameTextBoxRect.Inflate(borderWidth, borderWidth);
    std::unique_ptr<wing::Control> borderedPaddedObjectNameTextBox(new wing::BorderedControl(wing::BorderedControlCreateParams(paddedObjectNameTextBox.release()).Defaults().
        Location(wing::Point(borderedPaddedObjectNameTextBoxRect.X, borderedPaddedObjectNameTextBoxRect.Y)).
        SetSize(wing::Size(borderedPaddedObjectNameTextBoxRect.Width, borderedPaddedObjectNameTextBoxRect.Height)).
        SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    AddChild(borderedPaddedObjectNameTextBox.release());
    wing::Point attributesLabelLoc(defaultControlSpacing.Width, line2Top);
    std::unique_ptr<wing::Label> attributesLabel(new wing::Label(wing::LabelCreateParams().Defaults().Text("Attributes:").Location(attributesLabelLoc).SetSize(defaultLabelSize).
        SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    wing::Size attributesLabelSize = attributesLabel->GetSize();
    AddChild(attributesLabel.release());
    wing::Point editAttributesButtonLoc(defaultControlSpacing.Width + column1Width + defaultControlSpacing.Width - (textBoxPadding + borderWidth), line2Top - (textBoxPadding + borderWidth));
    wing::Size editAttributesButtonSize(wing::ScreenMetrics::Get().MMToHorizontalPixels(8), defaultButtonSize.Height);
    std::unique_ptr<wing::Button> editAttributesButtonPtr(new wing::Button(wing::ControlCreateParams().Defaults().Text("...").Location(editAttributesButtonLoc).
        SetSize(editAttributesButtonSize).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    editAttributesButton = editAttributesButtonPtr.get();
    editAttributesButton->Click().AddHandler(this, &ObjectPropertiesDialog::EditAttributes);
    AddChild(editAttributesButtonPtr.release());
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
    if (objectElement->Name().empty())
    {
        okButton->Disable();
        editAttributesButton->Disable();
    }
    AddChild(okButtonPtr.release());
    AddChild(cancelButtonPtr.release());
    SetDefaultButton(okButton);
    SetCancelButton(cancelButton);
    objectNameTextBox->SetFocus();
}

void ObjectPropertiesDialog::OnGotFocus()
{
    wing::Window::OnGotFocus();
    objectNameTextBox->SetFocus();
}

void ObjectPropertiesDialog::ObjectNameChanged()
{
    if (objectNameTextBox->Text().empty())
    {
        okButton->Disable();
        editAttributesButton->Disable();
    }
    else
    {
        okButton->Enable();
        editAttributesButton->Enable();
        objectElement->SetName(objectNameTextBox->Text());
    }
}

void ObjectPropertiesDialog::EditAttributes()
{
    std::map<DiagramElement*, DiagramElement*> prevCloneMap = cloneMap;
    std::map<DiagramElement*, DiagramElement*> prevReverseCloneMap = reverseCloneMap;
    if (!ExecuteEditAttributesDialog(objectElement->Attributes(), cloneMap, reverseCloneMap, objectElement, *this))
    {
        cloneMap = prevCloneMap;
        reverseCloneMap = prevReverseCloneMap;
    }
}

} // namespace springpp
