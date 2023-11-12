// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

export module springpp.relationship_properties_dialog;

import wing;
import std.core;

export namespace springpp {

class RelationshipElement;

class RelationshipPropertiesDialog : public wing::Window
{
public:
    RelationshipPropertiesDialog(RelationshipElement* relationshipElement_);
private:
    void SetRelationRadioButton();
    void SetCardinalityRadioButton();
    void RelationRadioButtonStateChanged();
    void CardinalityRadioButtonStateChanged();
    void SourceTextBoxTextChanged();
    void TargetTextBoxTextChanged();
    RelationshipElement* relationshipElement;
    bool ready;
    wing::GroupBox* relationGroupBox;
    wing::RadioButton* inheritanceRadioButton;
    wing::RadioButton* compositionRadioButton;
    wing::RadioButton* aggregationRadioButton;
    wing::RadioButton* referenceRadioButton;
    wing::RadioButton* createInstanceRadioButton;
    wing::GroupBox* cardinalityGroupBox;
    wing::RadioButton* zeroRadioButton;
    wing::RadioButton* oneRadioButton;
    wing::RadioButton* manyRadioButton;
    wing::TextBox* sourceTextBox;
    wing::TextBox* targetTextBox;
    wing::Button* okButton;
    wing::Button* cancelButton;
};

} // namespace springpp
