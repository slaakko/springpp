// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

module springpp.edit_diagram_element_list_dialog;

import util;

namespace springpp {

EditDiagramElementListDialogParams::EditDiagramElementListDialogParams() : dialogSize(), dialogCaption(), groupBoxCaption(), groupBoxSize(), listBoxSize()
{
}

EditDiagramElementListDialog::EditDiagramElementListDialog(const EditDiagramElementListDialogParams& dialogParams_) : 
    wing::Window(wing::WindowCreateParams().Defaults().Text(dialogParams_.dialogCaption).
        WindowStyle(wing::DialogWindowStyle()).Location(wing::DefaultLocation()).
        WindowClassBackgroundColor(wing::DefaultControlWindowClassBackgroundColor()).BackgroundColor(wing::DefaultControlBackgroundColor()).
        SetSize(dialogParams_.dialogSize)),
    dialogParams(dialogParams_), groupBox(nullptr), addButton(nullptr), changeButton(nullptr), deleteButton(nullptr), moveUpButton(nullptr), moveDownButton(nullptr), 
    listBox(nullptr), okButton(nullptr), cancelButton(nullptr)
{
    wing::Size s = GetSize();
    wing::Size defaultButtonSize = wing::ScreenMetrics::Get().DefaultButtonSize();
    wing::Size defaultListBoxSize = wing::ScreenMetrics::Get().DefaultListBoxSize();
    wing::Size defaultControlSpacing = wing::ScreenMetrics::Get().DefaultControlSpacing();

    std::unique_ptr<wing::GroupBox> groupBoxPtr(new wing::GroupBox(wing::GroupBoxCreateParams().Defaults().Text(dialogParams.groupBoxCaption).
        Location(wing::Point(defaultControlSpacing.Width, defaultControlSpacing.Height)).SetSize(dialogParams.groupBoxSize).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    groupBox = groupBoxPtr.get();
    wing::Point groupBoxLocation = groupBox->Location();
    wing::Size groupBoxSize = groupBox->GetSize();
    AddChild(groupBoxPtr.release());
    wing::Point addButtonLocation(groupBoxLocation.X + groupBoxSize.Width + defaultControlSpacing.Width, groupBoxLocation.Y + defaultControlSpacing.Height);
    std::unique_ptr<wing::Button> addButtonPtr(new wing::Button(wing::ControlCreateParams().Defaults().Text("Add").Location(addButtonLocation).
        SetSize(defaultButtonSize).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    addButton = addButtonPtr.get();
    addButton->Disable();
    AddChild(addButtonPtr.release());
    wing::Point changeButtonLocation(groupBoxLocation.X + groupBoxSize.Width + defaultControlSpacing.Width, addButtonLocation.Y + defaultButtonSize.Height + 
        defaultControlSpacing.Height);
    std::unique_ptr<wing::Button> changeButtonPtr(new wing::Button(wing::ControlCreateParams().Defaults().Text("Change").Location(changeButtonLocation).
        SetSize(defaultButtonSize).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    changeButton = changeButtonPtr.get();
    changeButton->Disable();
    AddChild(changeButtonPtr.release());
    wing::Point deleteButtonLocation(groupBoxLocation.X + groupBoxSize.Width + defaultControlSpacing.Width, changeButtonLocation.Y + defaultButtonSize.Height + 
        defaultControlSpacing.Height);
    std::unique_ptr<wing::Button> deleteButtonPtr(new wing::Button(wing::ControlCreateParams().Defaults().Text("Delete").Location(deleteButtonLocation).
        SetSize(defaultButtonSize).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    deleteButton = deleteButtonPtr.get();
    deleteButton->Disable();
    AddChild(deleteButtonPtr.release());
    wing::Point moveUpButtonLocation(groupBoxLocation.X + groupBoxSize.Width + defaultControlSpacing.Width, deleteButtonLocation.Y + defaultButtonSize.Height + 
        defaultControlSpacing.Height);
    std::unique_ptr<wing::Button> moveUpButtonPtr(new wing::Button(wing::ControlCreateParams().Defaults().Text(util::ToUtf8(std::u32string(1, char32_t(0xF1)))).
        Location(moveUpButtonLocation).
        SetSize(defaultButtonSize).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    wing::FontFamily fontFamily(L"Wingdings");
    wing::Font font(fontFamily, 8.25f, wing::FontStyle(wing::FontStyle::FontStyleRegular), wing::Unit::UnitPoint);
    moveUpButton = moveUpButtonPtr.get();
    moveUpButton->SetFont(font);
    moveUpButton->Disable();
    AddChild(moveUpButtonPtr.release());
    wing::Point moveDownButtonLocation(groupBoxLocation.X + groupBoxSize.Width + defaultControlSpacing.Width, moveUpButtonLocation.Y + defaultButtonSize.Height +
        defaultControlSpacing.Height);
    std::unique_ptr<wing::Button> moveDownButtonPtr(new wing::Button(wing::ControlCreateParams().Defaults().Text(util::ToUtf8(std::u32string(1, char32_t(0xF2)))).
        Location(moveDownButtonLocation).
        SetSize(defaultButtonSize).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    moveDownButton = moveDownButtonPtr.get();
    moveDownButton->SetFont(font);
    moveDownButton->Disable();
    AddChild(moveDownButtonPtr.release());

    wing::Point listBoxLocation(defaultControlSpacing.Width, defaultControlSpacing.Height + dialogParams.groupBoxSize.Height + defaultControlSpacing.Height);
    wing::Size listBoxSize = dialogParams.listBoxSize;
    wing::Rect listBoxRect(listBoxLocation, listBoxSize);
    listBoxRect.Inflate(-1, -1);

    std::unique_ptr<wing::ListBox> listBoxPtr(new wing::ListBox(wing::ListBoxCreateParams().Defaults().Location(wing::Point(listBoxRect.X, listBoxRect.Y)).
        SetSize(wing::Size(listBoxRect.Width, listBoxRect.Height)).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    listBox = listBoxPtr.get();
    std::unique_ptr<wing::Control> borderedListBoxPtr(new wing::BorderedControl(wing::BorderedControlCreateParams(listBoxPtr.release()).Defaults().
        Location(listBoxLocation).SetSize(listBoxSize).
        SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    wing::Control* borderedListBox = borderedListBoxPtr.get();
    AddChild(borderedListBoxPtr.release());
    wing::Point realListBoxLocation = listBox->Location();
    wing::Size realListBoxSize = listBox->GetSize();
    wing::Rect realListBoxRect(realListBoxLocation, realListBoxSize);
    realListBoxRect.Inflate(1, 1);
    borderedListBox->SetSize(wing::Size(realListBoxRect.Width, realListBoxRect.Height));
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
    AddChild(okButtonPtr.release());
    AddChild(cancelButtonPtr.release());
    SetDefaultButton(okButton);
    SetCancelButton(cancelButton);
}

class AbstractOperationElementListDialog : public EditDiagramElementListDialog
{
public:
    AbstractOperationElementListDialog(const EditDiagramElementListDialogParams& dialogParams_, IndexList<OperationElement>& operationList_, ContainerElement* containerElement_);
protected:
    void OnGotFocus();
private:
    void FillListBox();
    void OperationNameTextBoxGotFocus();
    void OperationNameTextBoxLostFocus();
    void OperationNameTextBoxTextChanged();
    void AddOperation();
    void ChangeOperation();
    void DeleteOperation();
    void MoveUpOperation();
    void MoveDownOperation();
    void SelectedIndexChanged();
    IndexList<OperationElement>& operationList;
    ContainerElement* containerElement;
    wing::TextBox* operationNameTextBox;
    wing::CheckBox* abstractCheckBox;
    int selectedIndex;
    OperationElement* selectedOperation;
};

AbstractOperationElementListDialog::AbstractOperationElementListDialog(const EditDiagramElementListDialogParams& dialogParams_, 
    IndexList<OperationElement>& operationList_, ContainerElement* containerElement_) : 
    EditDiagramElementListDialog(dialogParams_), 
    operationList(operationList_), containerElement(containerElement_), operationNameTextBox(nullptr), abstractCheckBox(nullptr), selectedIndex(-1), selectedOperation(nullptr)
{
    wing::Size defaultControlSpacing = wing::ScreenMetrics::Get().DefaultControlSpacing();
    wing::Size defaultTextBoxSize = wing::ScreenMetrics::Get().DefaultTextBoxSize();
    wing::Size defaultLabelSize = wing::ScreenMetrics::Get().DefaultLabelSize();
    wing::Size defaultCheckBoxSize = wing::ScreenMetrics::Get().DefaultCheckBoxSize();
    int textBoxPadding = wing::defaultControlPadding;
    int borderWidth = wing::BorderWidth(wing::BorderStyle::single);
    wing::Point groupBoxLocation = GetGroupBox()->Location();
    wing::Size groupBoxSize = GetGroupBox()->GetSize();
    wing::Point nameLabelLocation(defaultControlSpacing.Width, groupBoxLocation.Y + defaultControlSpacing.Height * 2 + 4);
    std::unique_ptr<wing::Label> nameLabelPtr(new wing::Label(wing::LabelCreateParams().Defaults().Text("Name:").
        Location(nameLabelLocation).SetSize(defaultLabelSize).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    wing::Label* nameLabel = nameLabelPtr.get();
    GetGroupBox()->AddChild(nameLabelPtr.release());
    wing::Size nameLabelSize = nameLabel->GetSize();
    wing::Point nameTextBoxLocation(defaultControlSpacing.Width + nameLabel->GetSize().Width + defaultControlSpacing.Width,
        groupBoxLocation.Y + defaultControlSpacing.Height * 2 + 4);
    wing::Size nameTextBoxSize = wing::Size(groupBoxSize.Width - nameLabel->GetSize().Width - 4 * defaultControlSpacing.Width, defaultTextBoxSize.Height + 4);
    wing::Rect nameTextBoxRec(nameTextBoxLocation, nameTextBoxSize);
    wing::Rect paddedNameTextBoxRect = nameTextBoxRec;
    paddedNameTextBoxRect.Inflate(textBoxPadding, textBoxPadding);
    std::unique_ptr<wing::TextBox> operationNameTextBoxPtr(new wing::TextBox(wing::TextBoxCreateParams().Defaults()));
    operationNameTextBox = operationNameTextBoxPtr.get();
    operationNameTextBox->GotFocus().AddHandler(this, &AbstractOperationElementListDialog::OperationNameTextBoxGotFocus);
    operationNameTextBox->LostFocus().AddHandler(this, &AbstractOperationElementListDialog::OperationNameTextBoxLostFocus);
    operationNameTextBox->TextChanged().AddHandler(this, &AbstractOperationElementListDialog::OperationNameTextBoxTextChanged);
    std::unique_ptr<wing::Control> paddedOperationNameTextBox(new wing::PaddedControl(wing::PaddedControlCreateParams(operationNameTextBoxPtr.release()).Defaults()));
    wing::Rect borderedPaddedOperationNameTextBoxRect = paddedNameTextBoxRect;
    borderedPaddedOperationNameTextBoxRect.Inflate(borderWidth, borderWidth);
    std::unique_ptr<wing::Control> borderedPaddedOperationNameTextBox(new wing::BorderedControl(wing::BorderedControlCreateParams(paddedOperationNameTextBox.release()).Defaults().
        Location(wing::Point(borderedPaddedOperationNameTextBoxRect.X, borderedPaddedOperationNameTextBoxRect.Y)).
        SetSize(wing::Size(borderedPaddedOperationNameTextBoxRect.Width, borderedPaddedOperationNameTextBoxRect.Height)).
        SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    GetGroupBox()->AddChild(borderedPaddedOperationNameTextBox.release());
    wing::Point checkBoxLocation(defaultControlSpacing.Width, groupBoxLocation.Y + nameLabelLocation.Y + nameLabelSize.Height + defaultControlSpacing.Height);
    wing::Size checkBoxSize = defaultCheckBoxSize;
    std::unique_ptr<wing::CheckBox> abstractCheckBoxPtr(new wing::CheckBox(
        wing::CheckBoxCreateParams().Defaults().Text("Abstract").Location(checkBoxLocation).SetSize(checkBoxSize).
        SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    abstractCheckBox = abstractCheckBoxPtr.get();
    GetGroupBox()->AddChild(abstractCheckBoxPtr.release());
    GetAddButton()->Click().AddHandler(this, &AbstractOperationElementListDialog::AddOperation);
    GetChangeButton()->Click().AddHandler(this, &AbstractOperationElementListDialog::ChangeOperation);
    GetDeleteButton()->Click().AddHandler(this, &AbstractOperationElementListDialog::DeleteOperation);
    GetMoveUpButton()->Click().AddHandler(this, &AbstractOperationElementListDialog::MoveUpOperation);
    GetMoveDownButton()->Click().AddHandler(this, &AbstractOperationElementListDialog::MoveDownOperation);
    GetListBox()->SelectedIndexChanged().AddHandler(this, &AbstractOperationElementListDialog::SelectedIndexChanged);
    FillListBox();
}

void AbstractOperationElementListDialog::OnGotFocus()
{
    EditDiagramElementListDialog::OnGotFocus();
    operationNameTextBox->SetFocus();
}

void AbstractOperationElementListDialog::FillListBox()
{
    for (const auto& operationElement : operationList)
    {
        std::string listBoxItemString = operationElement->Name();
        if (operationElement->IsAbstract())
        {
            listBoxItemString.append(", abstract=true");
        }
        GetListBox()->AddItem(listBoxItemString);
    }
}

void AbstractOperationElementListDialog::OperationNameTextBoxGotFocus()
{
    if (selectedIndex != -1 && GetChangeButton()->IsEnabled())
    {
        SetDefaultButton(GetChangeButton());
    }
    else
    {
        SetDefaultButton(GetAddButton());
    }
}

void AbstractOperationElementListDialog::OperationNameTextBoxLostFocus()
{
    SetDefaultButton(GetOkButton());
}

void AbstractOperationElementListDialog::OperationNameTextBoxTextChanged()
{
    if (operationNameTextBox->Text().empty())
    {
        GetAddButton()->Disable();
        GetChangeButton()->Disable();
    }
    else
    {
        GetAddButton()->Enable();
        if (selectedIndex != -1)
        {
            GetChangeButton()->Enable();
        }
    }
}

void AbstractOperationElementListDialog::AddOperation()
{
    std::string operationName = operationNameTextBox->Text();
    bool isAbstract = abstractCheckBox->Checked();
    OperationElement* operationElement = new OperationElement();
    operationElement->SetName(operationName);
    if (isAbstract)
    {
        operationElement->SetAbstract();
    }
    operationElement->SetContainerElement(containerElement);
    operationList.Add(operationElement);
    std::string listBoxItemString = operationName;
    if (isAbstract)
    {
        listBoxItemString.append(", abstract=true");
    }
    GetListBox()->AddItem(listBoxItemString);
    operationNameTextBox->Clear();
    abstractCheckBox->SetChecked(false);
    selectedIndex = -1;
    GetDeleteButton()->Disable();
    GetMoveUpButton()->Disable();
    GetMoveDownButton()->Disable();
}

void AbstractOperationElementListDialog::ChangeOperation()
{
    if (selectedIndex != -1 && selectedOperation != nullptr)
    {
        std::string operationName = operationNameTextBox->Text();
        bool isAbstract = abstractCheckBox->Checked();
        selectedOperation->SetName(operationName);
        if (isAbstract)
        {
            selectedOperation->SetAbstract();
        }
        else
        {
            selectedOperation->ResetAbstract();
        }
        std::string listBoxItemString = operationName;
        if (isAbstract)
        {
            listBoxItemString.append(", abstract=true");
        }
        GetListBox()->SetItem(selectedIndex, listBoxItemString);
        operationNameTextBox->Clear();
        operationNameTextBox->SetFocus();
        abstractCheckBox->SetChecked(false);
        selectedIndex = -1;
        SetDefaultButton(GetAddButton());
        GetDeleteButton()->Disable();
        GetMoveUpButton()->Disable();
        GetMoveDownButton()->Disable();
    }
}

void AbstractOperationElementListDialog::DeleteOperation()
{
    if (selectedIndex != -1 && selectedOperation != nullptr)
    {
        GetListBox()->DeleteItem(selectedIndex);
        operationList.Remove(selectedIndex);
        operationNameTextBox->Clear();
        abstractCheckBox->SetChecked(false);
        operationNameTextBox->SetFocus();
        selectedIndex = -1;
        SetDefaultButton(GetAddButton());
        GetDeleteButton()->Disable();
        GetMoveUpButton()->Disable();
        GetMoveDownButton()->Disable();
    }
}

void AbstractOperationElementListDialog::MoveUpOperation()
{
    if (selectedIndex > 0)
    {
        GetListBox()->DeleteItem(selectedIndex);
        operationList.MoveUp(selectedIndex);
        --selectedIndex;
        selectedOperation = operationList.Get(selectedIndex);
        std::string operationName = selectedOperation->Name();
        bool isAbstract = selectedOperation->IsAbstract();
        std::string listBoxItemString = operationName;
        if (isAbstract)
        {
            listBoxItemString.append(", abstract=true");
        }
        GetListBox()->InsertItem(selectedIndex, listBoxItemString);
        GetListBox()->SetSelectedIndex(selectedIndex);
        SelectedIndexChanged();
    }
}

void AbstractOperationElementListDialog::MoveDownOperation()
{
    if (selectedIndex < operationList.Count() - 1)
    {
        GetListBox()->DeleteItem(selectedIndex);
        operationList.MoveDown(selectedIndex);
        ++selectedIndex;
        selectedOperation = operationList.Get(selectedIndex);
        std::string operationName = selectedOperation->Name();
        bool isAbstract = selectedOperation->IsAbstract();
        std::string listBoxItemString = operationName;
        if (isAbstract)
        {
            listBoxItemString.append(", abstract=true");
        }
        if (selectedIndex < operationList.Count())
        {
            GetListBox()->InsertItem(selectedIndex, listBoxItemString);
        }
        else
        {
            GetListBox()->AddItem(listBoxItemString);
        }
        GetListBox()->SetSelectedIndex(selectedIndex);
        SelectedIndexChanged();
    }
}

void AbstractOperationElementListDialog::SelectedIndexChanged()
{
    selectedIndex = GetListBox()->GetSelectedIndex();
    if (selectedIndex != -1)
    {
        selectedOperation = operationList.Get(selectedIndex);
        operationNameTextBox->SetText(selectedOperation->Name());
        abstractCheckBox->SetChecked(selectedOperation->IsAbstract());
        GetChangeButton()->Enable();
        operationNameTextBox->SetFocus();
        GetDeleteButton()->Enable();
        if (selectedIndex > 0)
        {
            GetMoveUpButton()->Enable();
        }
        else
        {
            GetMoveUpButton()->Disable();
        }
        if (selectedIndex < operationList.Count() - 1)
        {
            GetMoveDownButton()->Enable();
        }
        else
        {
            GetMoveDownButton()->Disable();
        }
    }
    else
    {
        selectedOperation = nullptr;
        GetDeleteButton()->Disable();
    }
}

class ConcreteOperationElementListDialog : public EditDiagramElementListDialog
{
public:
    ConcreteOperationElementListDialog(const EditDiagramElementListDialogParams& dialogParams_, IndexList<OperationElement>& operationList_, ContainerElement* containerElement_);
protected:
    void OnGotFocus();
private:
    void FillListBox();
    void OperationNameTextBoxGotFocus();
    void OperationNameTextBoxLostFocus();
    void OperationNameTextBoxTextChanged();
    void AddOperation();
    void ChangeOperation();
    void DeleteOperation();
    void MoveUpOperation();
    void MoveDownOperation();
    void SelectedIndexChanged();
    IndexList<OperationElement>& operationList;
    ContainerElement* containerElement;
    wing::TextBox* operationNameTextBox;
    int selectedIndex;
    OperationElement* selectedOperation;
};

ConcreteOperationElementListDialog::ConcreteOperationElementListDialog(const EditDiagramElementListDialogParams& dialogParams_,
    IndexList<OperationElement>& operationList_, ContainerElement* containerElement_) :
    EditDiagramElementListDialog(dialogParams_),
    operationList(operationList_), containerElement(containerElement_), operationNameTextBox(nullptr), selectedIndex(-1), selectedOperation(nullptr)
{
    wing::Size defaultControlSpacing = wing::ScreenMetrics::Get().DefaultControlSpacing();
    wing::Size defaultTextBoxSize = wing::ScreenMetrics::Get().DefaultTextBoxSize();
    wing::Size defaultLabelSize = wing::ScreenMetrics::Get().DefaultLabelSize();
    int textBoxPadding = wing::defaultControlPadding;
    int borderWidth = wing::BorderWidth(wing::BorderStyle::single);
    wing::Point groupBoxLocation = GetGroupBox()->Location();
    wing::Size groupBoxSize = GetGroupBox()->GetSize();
    wing::Point nameLabelLocation(defaultControlSpacing.Width, groupBoxLocation.Y + defaultControlSpacing.Height * 2 + 4);
    std::unique_ptr<wing::Label> nameLabelPtr(new wing::Label(wing::LabelCreateParams().Defaults().Text("Name:").
        Location(nameLabelLocation).SetSize(defaultLabelSize).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    wing::Label* nameLabel = nameLabelPtr.get();
    GetGroupBox()->AddChild(nameLabelPtr.release());
    wing::Size nameLabelSize = nameLabel->GetSize();
    wing::Point nameTextBoxLocation(defaultControlSpacing.Width + nameLabel->GetSize().Width + defaultControlSpacing.Width,
        groupBoxLocation.Y + defaultControlSpacing.Height * 2 + 4);
    wing::Size nameTextBoxSize = wing::Size(groupBoxSize.Width - nameLabel->GetSize().Width - 4 * defaultControlSpacing.Width, defaultTextBoxSize.Height + 4);
    wing::Rect nameTextBoxRec(nameTextBoxLocation, nameTextBoxSize);
    wing::Rect paddedNameTextBoxRect = nameTextBoxRec;
    paddedNameTextBoxRect.Inflate(textBoxPadding, textBoxPadding);
    std::unique_ptr<wing::TextBox> operationNameTextBoxPtr(new wing::TextBox(wing::TextBoxCreateParams().Defaults()));
    operationNameTextBox = operationNameTextBoxPtr.get();
    operationNameTextBox->GotFocus().AddHandler(this, &ConcreteOperationElementListDialog::OperationNameTextBoxGotFocus);
    operationNameTextBox->LostFocus().AddHandler(this, &ConcreteOperationElementListDialog::OperationNameTextBoxLostFocus);
    operationNameTextBox->TextChanged().AddHandler(this, &ConcreteOperationElementListDialog::OperationNameTextBoxTextChanged);
    std::unique_ptr<wing::Control> paddedOperationNameTextBox(new wing::PaddedControl(wing::PaddedControlCreateParams(operationNameTextBoxPtr.release()).Defaults()));
    wing::Rect borderedPaddedOperationNameTextBoxRect = paddedNameTextBoxRect;
    borderedPaddedOperationNameTextBoxRect.Inflate(borderWidth, borderWidth);
    std::unique_ptr<wing::Control> borderedPaddedOperationNameTextBox(new wing::BorderedControl(wing::BorderedControlCreateParams(paddedOperationNameTextBox.release()).Defaults().
        Location(wing::Point(borderedPaddedOperationNameTextBoxRect.X, borderedPaddedOperationNameTextBoxRect.Y)).
        SetSize(wing::Size(borderedPaddedOperationNameTextBoxRect.Width, borderedPaddedOperationNameTextBoxRect.Height)).
        SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    GetGroupBox()->AddChild(borderedPaddedOperationNameTextBox.release());
    GetAddButton()->Click().AddHandler(this, &ConcreteOperationElementListDialog::AddOperation);
    GetChangeButton()->Click().AddHandler(this, &ConcreteOperationElementListDialog::ChangeOperation);
    GetDeleteButton()->Click().AddHandler(this, &ConcreteOperationElementListDialog::DeleteOperation);
    GetMoveUpButton()->Click().AddHandler(this, &ConcreteOperationElementListDialog::MoveUpOperation);
    GetMoveDownButton()->Click().AddHandler(this, &ConcreteOperationElementListDialog::MoveDownOperation);
    GetListBox()->SelectedIndexChanged().AddHandler(this, &ConcreteOperationElementListDialog::SelectedIndexChanged);
    FillListBox();
}

void ConcreteOperationElementListDialog::OnGotFocus()
{
    EditDiagramElementListDialog::OnGotFocus();
    operationNameTextBox->SetFocus();
}

void ConcreteOperationElementListDialog::FillListBox()
{
    for (const auto& operationElement : operationList)
    {
        std::string listBoxItemString = operationElement->Name();
        GetListBox()->AddItem(listBoxItemString);
    }
}

void ConcreteOperationElementListDialog::OperationNameTextBoxGotFocus()
{
    if (selectedIndex != -1 && GetChangeButton()->IsEnabled())
    {
        SetDefaultButton(GetChangeButton());
    }
    else
    {
        SetDefaultButton(GetAddButton());
    }
}

void ConcreteOperationElementListDialog::OperationNameTextBoxLostFocus()
{
    SetDefaultButton(GetOkButton());
}

void ConcreteOperationElementListDialog::OperationNameTextBoxTextChanged()
{
    if (operationNameTextBox->Text().empty())
    {
        GetAddButton()->Disable();
        GetChangeButton()->Disable();
    }
    else
    {
        GetAddButton()->Enable();
        if (selectedIndex != -1)
        {
            GetChangeButton()->Enable();
        }
    }
}

void ConcreteOperationElementListDialog::AddOperation()
{
    std::string operationName = operationNameTextBox->Text();
    OperationElement* operationElement = new OperationElement();
    operationElement->SetName(operationName);
    operationElement->SetContainerElement(containerElement);
    operationList.Add(operationElement);
    std::string listBoxItemString = operationName;
    GetListBox()->AddItem(listBoxItemString);
    operationNameTextBox->Clear();
    selectedIndex = -1;
    GetDeleteButton()->Disable();
    GetMoveUpButton()->Disable();
    GetMoveDownButton()->Disable();
}

void ConcreteOperationElementListDialog::ChangeOperation()
{
    if (selectedIndex != -1 && selectedOperation != nullptr)
    {
        std::string operationName = operationNameTextBox->Text();
        selectedOperation->SetName(operationName);
        std::string listBoxItemString = operationName;
        GetListBox()->SetItem(selectedIndex, listBoxItemString);
        operationNameTextBox->Clear();
        operationNameTextBox->SetFocus();
        selectedIndex = -1;
        SetDefaultButton(GetAddButton());
        GetDeleteButton()->Disable();
        GetMoveUpButton()->Disable();
        GetMoveDownButton()->Disable();
    }
}

void ConcreteOperationElementListDialog::DeleteOperation()
{
    if (selectedIndex != -1 && selectedOperation != nullptr)
    {
        GetListBox()->DeleteItem(selectedIndex);
        operationList.Remove(selectedIndex);
        operationNameTextBox->Clear();
        operationNameTextBox->SetFocus();
        selectedIndex = -1;
        SetDefaultButton(GetAddButton());
        GetDeleteButton()->Disable();
        GetMoveUpButton()->Disable();
        GetMoveDownButton()->Disable();
    }
}

void ConcreteOperationElementListDialog::MoveUpOperation()
{
    if (selectedIndex > 0)
    {
        GetListBox()->DeleteItem(selectedIndex);
        operationList.MoveUp(selectedIndex);
        --selectedIndex;
        selectedOperation = operationList.Get(selectedIndex);
        std::string operationName = selectedOperation->Name();
        std::string listBoxItemString = operationName;
        GetListBox()->InsertItem(selectedIndex, listBoxItemString);
        GetListBox()->SetSelectedIndex(selectedIndex);
        SelectedIndexChanged();
    }
}

void ConcreteOperationElementListDialog::MoveDownOperation()
{
    if (selectedIndex < operationList.Count() - 1)
    {
        GetListBox()->DeleteItem(selectedIndex);
        operationList.MoveDown(selectedIndex);
        ++selectedIndex;
        selectedOperation = operationList.Get(selectedIndex);
        std::string operationName = selectedOperation->Name();
        std::string listBoxItemString = operationName;
        if (selectedIndex < operationList.Count())
        {
            GetListBox()->InsertItem(selectedIndex, listBoxItemString);
        }
        else
        {
            GetListBox()->AddItem(listBoxItemString);
        }
        GetListBox()->SetSelectedIndex(selectedIndex);
        SelectedIndexChanged();
    }
}

void ConcreteOperationElementListDialog::SelectedIndexChanged()
{
    selectedIndex = GetListBox()->GetSelectedIndex();
    if (selectedIndex != -1)
    {
        selectedOperation = operationList.Get(selectedIndex);
        operationNameTextBox->SetText(selectedOperation->Name());
        GetChangeButton()->Enable();
        operationNameTextBox->SetFocus();
        GetDeleteButton()->Enable();
        if (selectedIndex > 0)
        {
            GetMoveUpButton()->Enable();
        }
        else
        {
            GetMoveUpButton()->Disable();
        }
        if (selectedIndex < operationList.Count() - 1)
        {
            GetMoveDownButton()->Enable();
        }
        else
        {
            GetMoveDownButton()->Disable();
        }
    }
    else
    {
        selectedOperation = nullptr;
        GetDeleteButton()->Disable();
    }
}

bool ExecuteEditOperationsDialog(IndexList<OperationElement>& operationList, ContainerElement* containerElement, bool mayHaveAbstractOperations, wing::Window& parentWindow)
{
    EditDiagramElementListDialogParams dialogParams;
    dialogParams.dialogCaption = "Operations";
    dialogParams.groupBoxCaption = "Operation";
    std::unique_ptr<EditDiagramElementListDialog> dialog;
    if (mayHaveAbstractOperations)
    {
        dialogParams.dialogSize = wing::Size(wing::ScreenMetrics::Get().MMToHorizontalPixels(140), wing::ScreenMetrics::Get().MMToVerticalPixels(90));
        dialogParams.groupBoxSize = wing::Size(wing::ScreenMetrics::Get().MMToHorizontalPixels(70), wing::ScreenMetrics::Get().MMToVerticalPixels(20));
        dialogParams.listBoxSize = wing::Size(wing::ScreenMetrics::Get().MMToHorizontalPixels(70), wing::ScreenMetrics::Get().MMToVerticalPixels(36));
        dialog.reset(new AbstractOperationElementListDialog(dialogParams, operationList, containerElement));
    }
    else
    {
        dialogParams.dialogSize = wing::Size(wing::ScreenMetrics::Get().MMToHorizontalPixels(140), wing::ScreenMetrics::Get().MMToVerticalPixels(90));
        dialogParams.groupBoxSize = wing::Size(wing::ScreenMetrics::Get().MMToHorizontalPixels(70), wing::ScreenMetrics::Get().MMToVerticalPixels(16));
        dialogParams.listBoxSize = wing::Size(wing::ScreenMetrics::Get().MMToHorizontalPixels(70), wing::ScreenMetrics::Get().MMToVerticalPixels(48));
        dialog.reset(new ConcreteOperationElementListDialog(dialogParams, operationList, containerElement));
    }
    if (dialog->ShowDialog(parentWindow) == wing::DialogResult::ok)
    {
        return true;
    }
    return false;
}

class AttributeElementListDialog : public EditDiagramElementListDialog
{
public:
    AttributeElementListDialog(const EditDiagramElementListDialogParams& dialogParams_, IndexList<AttributeElement>& attributeList_, ContainerElement* containerElement_);
protected:
    void OnGotFocus();
private:
    void FillListBox();
    void AttributeNameTextBoxGotFocus();
    void AttributeNameTextBoxLostFocus();
    void AttributeNameTextBoxTextChanged();
    void AddAttribute();
    void ChangeAttribute();
    void DeleteAttribute();
    void MoveUpAttribute();
    void MoveDownAttribute();
    void SelectedIndexChanged();
    IndexList<AttributeElement>& attributeList;
    ContainerElement* containerElement;
    wing::TextBox* attributeNameTextBox;
    int selectedIndex;
    AttributeElement* selectedAttribute;
};

AttributeElementListDialog::AttributeElementListDialog(const EditDiagramElementListDialogParams& dialogParams_,
    IndexList<AttributeElement>& attributeList_, ContainerElement* containerElement_) :
    EditDiagramElementListDialog(dialogParams_),
    attributeList(attributeList_), containerElement(containerElement_), attributeNameTextBox(nullptr), selectedIndex(-1), selectedAttribute(nullptr)
{
    wing::Size defaultControlSpacing = wing::ScreenMetrics::Get().DefaultControlSpacing();
    wing::Size defaultTextBoxSize = wing::ScreenMetrics::Get().DefaultTextBoxSize();
    wing::Size defaultLabelSize = wing::ScreenMetrics::Get().DefaultLabelSize();
    int textBoxPadding = wing::defaultControlPadding;
    int borderWidth = wing::BorderWidth(wing::BorderStyle::single);
    wing::Point groupBoxLocation = GetGroupBox()->Location();
    wing::Size groupBoxSize = GetGroupBox()->GetSize();
    wing::Point nameLabelLocation(defaultControlSpacing.Width, groupBoxLocation.Y + defaultControlSpacing.Height * 2 + 4);
    std::unique_ptr<wing::Label> nameLabelPtr(new wing::Label(wing::LabelCreateParams().Defaults().Text("Name:").
        Location(nameLabelLocation).SetSize(defaultLabelSize).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    wing::Label* nameLabel = nameLabelPtr.get();
    GetGroupBox()->AddChild(nameLabelPtr.release());
    wing::Size nameLabelSize = nameLabel->GetSize();
    wing::Point nameTextBoxLocation(defaultControlSpacing.Width + nameLabel->GetSize().Width + defaultControlSpacing.Width,
        groupBoxLocation.Y + defaultControlSpacing.Height * 2 + 4);
    wing::Size nameTextBoxSize = wing::Size(groupBoxSize.Width - nameLabel->GetSize().Width - 4 * defaultControlSpacing.Width, defaultTextBoxSize.Height + 4);
    wing::Rect nameTextBoxRec(nameTextBoxLocation, nameTextBoxSize);
    wing::Rect paddedNameTextBoxRect = nameTextBoxRec;
    paddedNameTextBoxRect.Inflate(textBoxPadding, textBoxPadding);
    std::unique_ptr<wing::TextBox> attributeNameTextBoxPtr(new wing::TextBox(wing::TextBoxCreateParams().Defaults()));
    attributeNameTextBox = attributeNameTextBoxPtr.get();
    attributeNameTextBox->GotFocus().AddHandler(this, &AttributeElementListDialog::AttributeNameTextBoxGotFocus);
    attributeNameTextBox->LostFocus().AddHandler(this, &AttributeElementListDialog::AttributeNameTextBoxLostFocus);
    attributeNameTextBox->TextChanged().AddHandler(this, &AttributeElementListDialog::AttributeNameTextBoxTextChanged);
    std::unique_ptr<wing::Control> paddedAttributeNameTextBox(new wing::PaddedControl(wing::PaddedControlCreateParams(attributeNameTextBoxPtr.release()).Defaults()));
    wing::Rect borderedPaddedAttributeNameTextBoxRect = paddedNameTextBoxRect;
    borderedPaddedAttributeNameTextBoxRect.Inflate(borderWidth, borderWidth);
    std::unique_ptr<wing::Control> borderedPaddedAttributeNameTextBox(new wing::BorderedControl(wing::BorderedControlCreateParams(paddedAttributeNameTextBox.release()).Defaults().
        Location(wing::Point(borderedPaddedAttributeNameTextBoxRect.X, borderedPaddedAttributeNameTextBoxRect.Y)).
        SetSize(wing::Size(borderedPaddedAttributeNameTextBoxRect.Width, borderedPaddedAttributeNameTextBoxRect.Height)).
        SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    GetGroupBox()->AddChild(borderedPaddedAttributeNameTextBox.release());
    GetAddButton()->Click().AddHandler(this, &AttributeElementListDialog::AddAttribute);
    GetChangeButton()->Click().AddHandler(this, &AttributeElementListDialog::ChangeAttribute);
    GetDeleteButton()->Click().AddHandler(this, &AttributeElementListDialog::DeleteAttribute);
    GetMoveUpButton()->Click().AddHandler(this, &AttributeElementListDialog::MoveUpAttribute);
    GetMoveDownButton()->Click().AddHandler(this, &AttributeElementListDialog::MoveDownAttribute);
    GetListBox()->SelectedIndexChanged().AddHandler(this, &AttributeElementListDialog::SelectedIndexChanged);
    FillListBox();
}

void AttributeElementListDialog::OnGotFocus()
{
    EditDiagramElementListDialog::OnGotFocus();
    attributeNameTextBox->SetFocus();
}

void AttributeElementListDialog::FillListBox()
{
    for (const auto& attributeElement : attributeList)
    {
        std::string listBoxItemString = attributeElement->Name();
        GetListBox()->AddItem(listBoxItemString);
    }
}

void AttributeElementListDialog::AttributeNameTextBoxGotFocus()
{
    if (selectedIndex != -1 && GetChangeButton()->IsEnabled())
    {
        SetDefaultButton(GetChangeButton());
    }
    else
    {
        SetDefaultButton(GetAddButton());
    }
}

void AttributeElementListDialog::AttributeNameTextBoxLostFocus()
{
    SetDefaultButton(GetOkButton());
}

void AttributeElementListDialog::AttributeNameTextBoxTextChanged()
{
    if (attributeNameTextBox->Text().empty())
    {
        GetAddButton()->Disable();
        GetChangeButton()->Disable();
    }
    else
    {
        GetAddButton()->Enable();
        if (selectedIndex != -1)
        {
            GetChangeButton()->Enable();
        }
    }
}

void AttributeElementListDialog::AddAttribute()
{
    std::string attributeName = attributeNameTextBox->Text();
    AttributeElement* attributeElement = new AttributeElement();
    attributeElement->SetName(attributeName);
    attributeElement->SetContainerElement(containerElement);
    attributeList.Add(attributeElement);
    std::string listBoxItemString = attributeName;
    GetListBox()->AddItem(listBoxItemString);
    attributeNameTextBox->Clear();
    selectedIndex = -1;
    GetDeleteButton()->Disable();
    GetMoveUpButton()->Disable();
    GetMoveDownButton()->Disable();
}

void AttributeElementListDialog::ChangeAttribute()
{
    if (selectedIndex != -1 && selectedAttribute != nullptr)
    {
        std::string attributeName = attributeNameTextBox->Text();
        selectedAttribute->SetName(attributeName);
        std::string listBoxItemString = attributeName;
        GetListBox()->SetItem(selectedIndex, listBoxItemString);
        attributeNameTextBox->Clear();
        attributeNameTextBox->SetFocus();
        selectedIndex = -1;
        SetDefaultButton(GetAddButton());
        GetDeleteButton()->Disable();
        GetMoveUpButton()->Disable();
        GetMoveDownButton()->Disable();
    }
}

void AttributeElementListDialog::DeleteAttribute()
{
    if (selectedIndex != -1 && selectedAttribute != nullptr)
    {
        GetListBox()->DeleteItem(selectedIndex);
        attributeList.Remove(selectedIndex);
        attributeNameTextBox->Clear();
        attributeNameTextBox->SetFocus();
        selectedIndex = -1;
        SetDefaultButton(GetAddButton());
        GetDeleteButton()->Disable();
        GetMoveUpButton()->Disable();
        GetMoveDownButton()->Disable();
    }
}

void AttributeElementListDialog::MoveUpAttribute()
{
    if (selectedIndex > 0)
    {
        GetListBox()->DeleteItem(selectedIndex);
        attributeList.MoveUp(selectedIndex);
        --selectedIndex;
        selectedAttribute = attributeList.Get(selectedIndex);
        std::string attributeName = selectedAttribute->Name();
        std::string listBoxItemString = attributeName;
        GetListBox()->InsertItem(selectedIndex, listBoxItemString);
        GetListBox()->SetSelectedIndex(selectedIndex);
        SelectedIndexChanged();
    }
}

void AttributeElementListDialog::MoveDownAttribute()
{
    if (selectedIndex < attributeList.Count() - 1)
    {
        GetListBox()->DeleteItem(selectedIndex);
        attributeList.MoveDown(selectedIndex);
        ++selectedIndex;
        selectedAttribute = attributeList.Get(selectedIndex);
        std::string attributeName = selectedAttribute->Name();
        std::string listBoxItemString = attributeName;
        if (selectedIndex < attributeList.Count())
        {
            GetListBox()->InsertItem(selectedIndex, listBoxItemString);
        }
        else
        {
            GetListBox()->AddItem(listBoxItemString);
        }
        GetListBox()->SetSelectedIndex(selectedIndex);
        SelectedIndexChanged();
    }
}

void AttributeElementListDialog::SelectedIndexChanged()
{
    selectedIndex = GetListBox()->GetSelectedIndex();
    if (selectedIndex != -1)
    {
        selectedAttribute = attributeList.Get(selectedIndex);
        attributeNameTextBox->SetText(selectedAttribute->Name());
        GetChangeButton()->Enable();
        attributeNameTextBox->SetFocus();
        GetDeleteButton()->Enable();
        if (selectedIndex > 0)
        {
            GetMoveUpButton()->Enable();
        }
        else
        {
            GetMoveUpButton()->Disable();
        }
        if (selectedIndex < attributeList.Count() - 1)
        {
            GetMoveDownButton()->Enable();
        }
        else
        {
            GetMoveDownButton()->Disable();
        }
    }
    else
    {
        selectedAttribute = nullptr;
        GetDeleteButton()->Disable();
    }
}

bool ExecuteEditAttributesDialog(IndexList<AttributeElement>& attributeList, ContainerElement* containerElement, wing::Window& parentWindow)
{
    EditDiagramElementListDialogParams dialogParams;
    dialogParams.dialogSize = wing::Size(wing::ScreenMetrics::Get().MMToHorizontalPixels(140), wing::ScreenMetrics::Get().MMToVerticalPixels(90));
    dialogParams.dialogCaption = "Attributes";
    dialogParams.groupBoxCaption = "Attribute";
    dialogParams.groupBoxSize = wing::Size(wing::ScreenMetrics::Get().MMToHorizontalPixels(70), wing::ScreenMetrics::Get().MMToVerticalPixels(16));
    dialogParams.listBoxSize = wing::Size(wing::ScreenMetrics::Get().MMToHorizontalPixels(70), wing::ScreenMetrics::Get().MMToVerticalPixels(48));
    std::unique_ptr<EditDiagramElementListDialog> dialog(new AttributeElementListDialog(dialogParams, attributeList, containerElement));
    if (dialog->ShowDialog(parentWindow) == wing::DialogResult::ok)
    {
        return true;
    }
    return false;
}

} // namespace springpp
