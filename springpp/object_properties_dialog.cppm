// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

export module springpp.object_properties_dialog;

import wing;
import std.core;

export namespace springpp {

class ObjectElement;
class DiagramElement;

class ObjectPropertiesDialog : public wing::Window
{
public:
    ObjectPropertiesDialog(ObjectElement* objectElement_, std::map<DiagramElement*, DiagramElement*>& cloneMap_, std::map<DiagramElement*, DiagramElement*>& reverseCloneMap_);
protected:
    void OnGotFocus();
private:
    ObjectElement* objectElement;
    wing::TextBox* objectNameTextBox;
    wing::Button* cancelButton;
    wing::Button* okButton;
    wing::Button* editAttributesButton;
    std::map<DiagramElement*, DiagramElement*>& cloneMap;
    std::map<DiagramElement*, DiagramElement*>& reverseCloneMap;
    void ObjectNameChanged();
    void EditAttributes();
};

} // namespace springpp
