// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

export module springpp.note_properties_dialog;

import springpp.container_element;
import wing;
import std.core;

export namespace springpp {

class NoteElement;

class NotePropertiesDialog : public wing::Window
{
public:
    NotePropertiesDialog(NoteElement* noteElement_);
private:
    void NoteTextChanged();
    NoteElement* noteElement;
    wing::TextBox* textBox;
    wing::Button* cancelButton;
    wing::Button* okButton;
};

} // namespace springpp
