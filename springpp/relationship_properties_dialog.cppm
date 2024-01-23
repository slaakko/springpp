// =================================
// Copyright (c) 2024 Seppo Laakko
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
    void EditPrimarySourceText();
    void EditSecondarySourceText();
    void EditSourceConnector();
    void EditPrimaryTargetText();
    void EditSecondaryTargetText();
    void EditTargetConnector();
    RelationshipElement* relationshipElement;
    bool ready;
    wing::GroupBox* relationGroupBox;
    wing::RadioButton* inheritanceRadioButton;
    wing::RadioButton* compositionRadioButton;
    wing::RadioButton* aggregationRadioButton;
    wing::RadioButton* referenceRadioButton;
    wing::RadioButton* createInstanceRadioButton;
    wing::RadioButton* attachNoteRadioButton;
    wing::GroupBox* cardinalityGroupBox;
    wing::RadioButton* zeroRadioButton;
    wing::RadioButton* oneRadioButton;
    wing::RadioButton* manyRadioButton;
    wing::GroupBox* sourceGroupBox;
    wing::GroupBox* targetGroupBox;
    wing::Button* primarySourceTextButton;
    wing::Button* secondarySourceTextButton;
    wing::Button* sourceConnectorButton;
    wing::Button* primaryTargetTextButton;
    wing::Button* secondaryTargetTextButton;
    wing::Button* targetConnectorButton;
    wing::Label* sourceConnectorTextLabel;
    wing::Label* targetConnectorTextLabel;
    wing::Button* okButton;
    wing::Button* cancelButton;
};

} // namespace springpp
