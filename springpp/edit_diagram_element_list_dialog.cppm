// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

export module springpp.edit_diagram_element_list_dialog;

import springpp.index_list;
import springpp.operation_element;
import springpp.field_element;
import wing;
import std.core;

export namespace springpp {

struct EditDiagramElementListDialogParams
{
    EditDiagramElementListDialogParams();
    wing::Size dialogSize;
    std::string dialogCaption;
    std::string groupBoxCaption;
    wing::Size groupBoxSize;
    wing::Size listBoxSize;
};

class EditDiagramElementListDialog : public wing::Window
{
public:
    EditDiagramElementListDialog(const EditDiagramElementListDialogParams& dialogParams_);
    wing::GroupBox* GetGroupBox() const { return groupBox; }
    wing::Button* GetAddButton() const { return addButton; }
    wing::Button* GetChangeButton() const { return changeButton; }
    wing::Button* GetDeleteButton() const { return deleteButton; }
    wing::Button* GetMoveUpButton() const { return moveUpButton; }
    wing::Button* GetMoveDownButton() const { return moveDownButton; }
    wing::ListBox* GetListBox() const { return listBox; }
    wing::Button* GetOkButton() const { return okButton; }
    wing::Button* GetCancelButton() const { return cancelButton; }
private:
    EditDiagramElementListDialogParams dialogParams;
    wing::GroupBox* groupBox;
    wing::Button* addButton;
    wing::Button* changeButton;
    wing::Button* deleteButton;
    wing::Button* moveUpButton;
    wing::Button* moveDownButton;
    wing::ListBox* listBox;
    wing::Button* okButton;
    wing::Button* cancelButton;
};

bool ExecuteEditOperationsDialog(IndexList<OperationElement>& operationList, ContainerElement* containerElement, bool mayHaveAbstractOperations, wing::Window& parentWindow);

bool ExecuteEditFieldsDialog(IndexList<FieldElement>& fieldList, ContainerElement* containerElement, wing::Window& parentWindow);

} // namespace springpp
