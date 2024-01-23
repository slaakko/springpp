// =================================
// Copyright (c) 2024 Seppo Laakko
// Distributed under the MIT license
// =================================

export module springpp.class_properties_dialog;

import wing;
import std.core;

export namespace springpp {

class ClassElement;
class DiagramElement;

class ClassPropertiesDialog : public wing::Window
{
public:    
    ClassPropertiesDialog(ClassElement* classElement_, std::map<DiagramElement*, DiagramElement*>& cloneMap_, std::map<DiagramElement*, DiagramElement*>& reverseCloneMap_);
private:
    ClassElement* classElement;
    wing::TextBox* classNameTextBox;
    wing::TextBox* keywordTextBox;
    wing::CheckBox* abstractCheckBox;
    wing::Button* cancelButton;
    wing::Button* okButton;
    wing::Button* editOperationsButton;
    wing::Button* editAttributesButton;
    std::map<DiagramElement*, DiagramElement*>& cloneMap;
    std::map<DiagramElement*, DiagramElement*>& reverseCloneMap;
    void ClassNameChanged();
    void KeywordChanged();
    void AbstractChanged();
    void EditOperations();
    void EditAttributes();
};

} // namespace springpp
