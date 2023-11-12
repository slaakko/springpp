// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

export module springpp.class_properties_dialog;

import wing;
import std.core;

export namespace springpp {

class ClassElement;

class ClassPropertiesDialog : public wing::Window
{
public:    
    ClassPropertiesDialog(ClassElement* classElement_);
private:
    ClassElement* classElement;
    wing::TextBox* classNameTextBox;
    wing::CheckBox* abstractCheckBox;
    wing::Button* cancelButton;
    wing::Button* okButton;
    wing::Button* editOperationsButton;
    wing::Button* editFieldsButton;
    void ClassNameChanged();
    void AbstractChanged();
    void EditOperations();
    void EditFields();
};

} // namespace springpp
