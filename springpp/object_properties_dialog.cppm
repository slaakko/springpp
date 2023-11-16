// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

export module springpp.object_properties_dialog;

import wing;
import std.core;

export namespace springpp {

class ObjectElement;

class ObjectPropertiesDialog : public wing::Window
{
public:
    ObjectPropertiesDialog(ObjectElement* objectElement_);
protected:
    void OnGotFocus();
private:
    ObjectElement* objectElement;
    wing::TextBox* objectNameTextBox;
    wing::Button* cancelButton;
    wing::Button* okButton;
    wing::Button* editAttributesButton;
    void ObjectNameChanged();
    void EditAttributes();
};

} // namespace springpp
