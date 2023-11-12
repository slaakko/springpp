// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

module springpp.relationship_properties_dialog;

import springpp.relationship_element;

namespace springpp {

RelationshipPropertiesDialog::RelationshipPropertiesDialog(RelationshipElement* relationshipElement_) : 
    wing::Window(wing::WindowCreateParams().Defaults().Text("Relationship Properties").
        WindowStyle(wing::DialogWindowStyle()).Location(wing::DefaultLocation()).
        WindowClassBackgroundColor(wing::DefaultControlWindowClassBackgroundColor()).BackgroundColor(wing::DefaultControlBackgroundColor()).
        SetSize(wing::Size(wing::ScreenMetrics::Get().MMToHorizontalPixels(120), wing::ScreenMetrics::Get().MMToVerticalPixels(100)))),
    relationshipElement(relationshipElement_), ready(false), relationGroupBox(nullptr), sourceTextBox(nullptr), targetTextBox(nullptr), 
    inheritanceRadioButton(nullptr), compositionRadioButton(nullptr), aggregationRadioButton(nullptr), referenceRadioButton(nullptr), createInstanceRadioButton(nullptr),
    cardinalityGroupBox(nullptr), zeroRadioButton(nullptr), oneRadioButton(nullptr), manyRadioButton(nullptr), okButton(nullptr), cancelButton(nullptr)
{
    int column1Width = wing::ScreenMetrics::Get().MMToHorizontalPixels(20);
    int column2Width = wing::ScreenMetrics::Get().MMToHorizontalPixels(30);
    wing::Size s = GetSize();
    wing::Size defaultButtonSize = wing::ScreenMetrics::Get().DefaultButtonSize();
    wing::Size defaultControlSpacing = wing::ScreenMetrics::Get().DefaultControlSpacing();
    wing::Size defaultLabelSize = wing::ScreenMetrics::Get().DefaultLabelSize();
    wing::Size defaultTextBoxSize = wing::ScreenMetrics::Get().DefaultTextBoxSize();
    wing::Size relationGroupBoxSize = wing::Size(wing::ScreenMetrics::Get().MMToHorizontalPixels(40), wing::ScreenMetrics::Get().MMToVerticalPixels(30));

    std::unique_ptr<wing::GroupBox> relationGroupBoxPtr(new wing::GroupBox(wing::GroupBoxCreateParams().Defaults().Text("Relation").
        Location(wing::Point(defaultControlSpacing.Width, defaultControlSpacing.Height)).
        SetSize(relationGroupBoxSize).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    relationGroupBox = relationGroupBoxPtr.get();
    AddChild(relationGroupBoxPtr.release());
    wing::Point relationGroupBoxLoc = relationGroupBox->Location();

    std::unique_ptr<wing::RadioButton> inheritanceRadioButtonPtr(new wing::RadioButton(wing::RadioButtonCreateParams().Defaults().Text("Inheritance").
        Location(wing::Point(relationGroupBoxLoc.X, relationGroupBoxLoc.Y + defaultControlSpacing.Height)).AutoSize(true).
        SetAnchors((wing::Anchors::top | wing::Anchors::left))));
    inheritanceRadioButton = inheritanceRadioButtonPtr.get();
    inheritanceRadioButton->CheckedChanged().AddHandler(this, &RelationshipPropertiesDialog::RelationRadioButtonStateChanged);
    relationGroupBox->AddChild(inheritanceRadioButtonPtr.release());
    wing::Point inheritanceRadioButtonLoc = inheritanceRadioButton->Location();
    wing::Size inheritanceRadioButtonSize = inheritanceRadioButton->GetSize();

    std::unique_ptr<wing::RadioButton> compositionRadioButtonPtr(new wing::RadioButton(wing::RadioButtonCreateParams().Defaults().Text("Composition").
        Location(wing::Point(inheritanceRadioButtonLoc.X, inheritanceRadioButtonLoc.Y + inheritanceRadioButtonSize.Height)).AutoSize(true).
        SetAnchors((wing::Anchors::top | wing::Anchors::left))));
    compositionRadioButton = compositionRadioButtonPtr.get();
    compositionRadioButton->CheckedChanged().AddHandler(this, &RelationshipPropertiesDialog::RelationRadioButtonStateChanged);
    relationGroupBox->AddChild(compositionRadioButtonPtr.release());
    wing::Point compositionRadioButtonLoc = compositionRadioButton->Location();
    wing::Size compositionRadioButtonSize = compositionRadioButton->GetSize();

    std::unique_ptr<wing::RadioButton> aggregationRadioButtonPtr(new wing::RadioButton(wing::RadioButtonCreateParams().Defaults().Text("Aggregation").
        Location(wing::Point(compositionRadioButtonLoc.X, compositionRadioButtonLoc.Y + compositionRadioButtonSize.Height)).AutoSize(true).
        SetAnchors((wing::Anchors::top | wing::Anchors::left))));
    aggregationRadioButton = aggregationRadioButtonPtr.get();
    aggregationRadioButton->CheckedChanged().AddHandler(this, &RelationshipPropertiesDialog::RelationRadioButtonStateChanged);
    relationGroupBox->AddChild(aggregationRadioButtonPtr.release());
    wing::Point aggregationRadioButtonLoc = aggregationRadioButton->Location();
    wing::Size aggregationRadioButtonSize = aggregationRadioButton->GetSize();

    std::unique_ptr<wing::RadioButton> referenceRadioButtonPtr(new wing::RadioButton(wing::RadioButtonCreateParams().Defaults().Text("Reference").
        Location(wing::Point(aggregationRadioButtonLoc.X, aggregationRadioButtonLoc.Y + aggregationRadioButtonSize.Height)).
        SetAnchors((wing::Anchors::top | wing::Anchors::left))));
    referenceRadioButton = referenceRadioButtonPtr.get();
    referenceRadioButton->CheckedChanged().AddHandler(this, &RelationshipPropertiesDialog::RelationRadioButtonStateChanged);
    relationGroupBox->AddChild(referenceRadioButtonPtr.release());
    wing::Point referenceRadioButtonLoc = referenceRadioButton->Location();
    wing::Size referenceRadioButtonSize = referenceRadioButton->GetSize();

    std::unique_ptr<wing::RadioButton> createInstanceRadioButtonPtr(new wing::RadioButton(wing::RadioButtonCreateParams().Defaults().Text("Create Instance").
        Location(wing::Point(referenceRadioButtonLoc.X, referenceRadioButtonLoc.Y + referenceRadioButtonSize.Height)).
        SetAnchors((wing::Anchors::top | wing::Anchors::left))));
    createInstanceRadioButton = createInstanceRadioButtonPtr.get();
    createInstanceRadioButton->CheckedChanged().AddHandler(this, &RelationshipPropertiesDialog::RelationRadioButtonStateChanged);
    relationGroupBox->AddChild(createInstanceRadioButtonPtr.release());

    wing::Size cardinalityGroupBoxSize(wing::ScreenMetrics::Get().MMToHorizontalPixels(40), wing::ScreenMetrics::Get().MMToVerticalPixels(30));
    std::unique_ptr<wing::GroupBox> cardinalityGroupBoxPtr(new wing::GroupBox(wing::GroupBoxCreateParams().Defaults().Text("Cardinality").
        Location(wing::Point(defaultControlSpacing.Width + relationGroupBoxLoc.X + relationGroupBoxSize.Width, defaultControlSpacing.Height)).
        SetSize(cardinalityGroupBoxSize).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    cardinalityGroupBox = cardinalityGroupBoxPtr.get();
    AddChild(cardinalityGroupBoxPtr.release());

    wing::Point cardinalityGroupBoxLoc = cardinalityGroupBox->Location();
    std::unique_ptr<wing::RadioButton> zeroRadioButtonPtr(new wing::RadioButton(wing::RadioButtonCreateParams().Defaults().Text("Zero").
        Location(wing::Point(defaultControlSpacing.Width, cardinalityGroupBoxLoc.Y + defaultControlSpacing.Height)).AutoSize(true).
        SetAnchors((wing::Anchors::top | wing::Anchors::left))));
    zeroRadioButton = zeroRadioButtonPtr.get();
    zeroRadioButton->CheckedChanged().AddHandler(this, &RelationshipPropertiesDialog::CardinalityRadioButtonStateChanged);
    cardinalityGroupBox->AddChild(zeroRadioButtonPtr.release());
    wing::Point zeroRadioButtonLoc = zeroRadioButton->Location();
    wing::Size zeroRadioButtonSize = zeroRadioButton->GetSize();
    std::unique_ptr<wing::RadioButton> oneRadioButtonPtr(new wing::RadioButton(wing::RadioButtonCreateParams().Defaults().Text("One").
        Location(wing::Point(defaultControlSpacing.Width, zeroRadioButtonLoc.Y + zeroRadioButtonSize.Height)).AutoSize(true).
        SetAnchors((wing::Anchors::top | wing::Anchors::left))));
    oneRadioButton = oneRadioButtonPtr.get();
    oneRadioButton->CheckedChanged().AddHandler(this, &RelationshipPropertiesDialog::CardinalityRadioButtonStateChanged);
    cardinalityGroupBox->AddChild(oneRadioButtonPtr.release());
    wing::Point oneRadioButtonLoc = oneRadioButton->Location();
    wing::Size oneRadioButtonSize = oneRadioButton->GetSize();
    std::unique_ptr<wing::RadioButton> manyRadioButtonPtr(new wing::RadioButton(wing::RadioButtonCreateParams().Defaults().Text("Many").
        Location(wing::Point(defaultControlSpacing.Width, oneRadioButtonLoc.Y + oneRadioButtonSize.Height)).AutoSize(true).
        SetAnchors((wing::Anchors::top | wing::Anchors::left))));
    manyRadioButton = manyRadioButtonPtr.get();
    manyRadioButton->CheckedChanged().AddHandler(this, &RelationshipPropertiesDialog::CardinalityRadioButtonStateChanged);
    cardinalityGroupBox->AddChild(manyRadioButtonPtr.release());

    wing::Point sourceTextLabelLoc(defaultControlSpacing.Width, relationGroupBoxLoc.Y + relationGroupBoxSize.Height + defaultControlSpacing.Height);
    std::unique_ptr<wing::Label> sourceTextLabelPtr(new wing::Label(wing::LabelCreateParams().Defaults().Text("Source text:").Location(sourceTextLabelLoc).SetSize(defaultLabelSize).
        AutoSize(true).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    wing::Label* sourceTextLabel = sourceTextLabelPtr.get();
    AddChild(sourceTextLabelPtr.release());
    wing::Size sourceTextLabelSize = sourceTextLabel->GetSize();

    std::unique_ptr<wing::TextBox> sourceTextBoxPtr(new wing::TextBox(wing::TextBoxCreateParams().Defaults().Text(relationshipElement->Source().Text())));
    sourceTextBox = sourceTextBoxPtr.get();
    sourceTextBox->TextChanged().AddHandler(this, &RelationshipPropertiesDialog::SourceTextBoxTextChanged);
    std::unique_ptr<wing::PaddedControl> paddedSourceTextBox(new wing::PaddedControl(wing::PaddedControlCreateParams(sourceTextBoxPtr.release()).Defaults().
        SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    std::unique_ptr<wing::BorderedControl> borderedPaddedSourceTextBox(new wing::BorderedControl(wing::BorderedControlCreateParams(paddedSourceTextBox.release()).Defaults().
        Location(wing::Point(column1Width, sourceTextLabelLoc.Y)).SetSize(wing::Size(column2Width, defaultTextBoxSize.Height + 4 + 1 + 2 * wing::defaultControlPadding)).
            SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    AddChild(borderedPaddedSourceTextBox.release());

    wing::Point targetTextLabelLoc(defaultControlSpacing.Width, sourceTextLabelLoc.Y + sourceTextLabelSize.Height + defaultControlSpacing.Height * 2 + 4);
    std::unique_ptr<wing::Label> targetTextLabelPtr(new wing::Label(wing::LabelCreateParams().Defaults().Text("Target text:").Location(targetTextLabelLoc).SetSize(defaultLabelSize).
        AutoSize(true).SetAnchors(wing::Anchors::left | wing::Anchors::top)));
    wing::Label* targetTextLabel = targetTextLabelPtr.get();
    AddChild(targetTextLabelPtr.release());

    std::unique_ptr<wing::TextBox> targetTextBoxPtr(new wing::TextBox(wing::TextBoxCreateParams().Defaults().Text(relationshipElement->Target().Text())));
    targetTextBox = targetTextBoxPtr.get();
    targetTextBox->TextChanged().AddHandler(this, &RelationshipPropertiesDialog::TargetTextBoxTextChanged);
    std::unique_ptr<wing::PaddedControl> paddedTargetTextBox(new wing::PaddedControl(wing::PaddedControlCreateParams(targetTextBoxPtr.release()).Defaults().
        SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    std::unique_ptr<wing::BorderedControl> borderedPaddedTargetTextBox(new wing::BorderedControl(wing::BorderedControlCreateParams(paddedTargetTextBox.release()).Defaults().
        Location(wing::Point(column1Width, targetTextLabelLoc.Y)).SetSize(wing::Size(column2Width, defaultTextBoxSize.Height + 4 + 1 + 2 * wing::defaultControlPadding)).
        SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    AddChild(borderedPaddedTargetTextBox.release());

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
    SetRelationRadioButton();
    SetCardinalityRadioButton();
    ready = true;
    RelationRadioButtonStateChanged();
    sourceTextBox->SetFocus();
}

void RelationshipPropertiesDialog::SetRelationRadioButton()
{
    switch (relationshipElement->RKind())
    {
        case RelationshipKind::inheritance:
        {
            relationGroupBox->SetCheckedRadioButton(inheritanceRadioButton);
            zeroRadioButton->Disable();
            break;
        }
        case RelationshipKind::composition:
        {
            relationGroupBox->SetCheckedRadioButton(compositionRadioButton);
            zeroRadioButton->Disable();
            break;
        }
        case RelationshipKind::aggregation:
        {
            relationGroupBox->SetCheckedRadioButton(aggregationRadioButton);
            zeroRadioButton->Disable();
            break;
        }
        case RelationshipKind::reference:
        {
            relationGroupBox->SetCheckedRadioButton(referenceRadioButton);
            break;
        }
        case RelationshipKind::createInstance:
        {
            relationGroupBox->SetCheckedRadioButton(createInstanceRadioButton);
            zeroRadioButton->Disable();
            break;
        }
    }
}

void RelationshipPropertiesDialog::SetCardinalityRadioButton()
{
    switch (relationshipElement->GetCardinality())
    {
        case Cardinality::zero:
        {
            cardinalityGroupBox->SetCheckedRadioButton(zeroRadioButton);
            break;
        }
        case Cardinality::one:
        {
            cardinalityGroupBox->SetCheckedRadioButton(oneRadioButton);
            break;
        }
        case Cardinality::many:
        {
            cardinalityGroupBox->SetCheckedRadioButton(manyRadioButton);
            break;
        }
    }
}

void RelationshipPropertiesDialog::RelationRadioButtonStateChanged()
{
    if (ready)
    {
        wing::RadioButton* checkedRadioButton = relationGroupBox->CheckedRadioButton();
        if (checkedRadioButton == inheritanceRadioButton)
        {
            relationshipElement->SetRKind(RelationshipKind::inheritance);
        }
        else if (checkedRadioButton == compositionRadioButton)
        {
            relationshipElement->SetRKind(RelationshipKind::composition);
        }
        else if (checkedRadioButton == aggregationRadioButton)
        {
            relationshipElement->SetRKind(RelationshipKind::aggregation);
        }
        else if (checkedRadioButton == referenceRadioButton)
        {
            relationshipElement->SetRKind(RelationshipKind::reference);
        }
        else if (checkedRadioButton == createInstanceRadioButton)
        {
            relationshipElement->SetRKind(RelationshipKind::createInstance);
        }
        if (checkedRadioButton == inheritanceRadioButton)
        {
            if (manyRadioButton->Checked())
            {
                oneRadioButton->SetChecked(true);
            }
            else if (zeroRadioButton->Checked())
            {
                oneRadioButton->SetChecked(true);
            }
            manyRadioButton->Disable();
            zeroRadioButton->Disable();
        }
        else if (checkedRadioButton == compositionRadioButton)
        {
            if (zeroRadioButton->Checked())
            {
                oneRadioButton->SetChecked(true);
            }
            zeroRadioButton->Disable();
            manyRadioButton->Enable();
        }
        else if (checkedRadioButton == aggregationRadioButton)
        {
            if (zeroRadioButton->Checked())
            {
                oneRadioButton->SetChecked(true);
            }
            zeroRadioButton->Disable();
            manyRadioButton->Enable();
        }
        else if (checkedRadioButton == referenceRadioButton)
        {
            zeroRadioButton->Enable();
            manyRadioButton->Enable();
        }
        else if (checkedRadioButton == createInstanceRadioButton)
        {
            if (zeroRadioButton->Checked())
            {
                oneRadioButton->SetChecked(true);
            }
            zeroRadioButton->Disable();
            manyRadioButton->Enable();
        }
    }
}

void RelationshipPropertiesDialog::CardinalityRadioButtonStateChanged()
{
    if (ready)
    {
        wing::RadioButton* checkedRadioButton = cardinalityGroupBox->CheckedRadioButton();
        if (checkedRadioButton == zeroRadioButton)
        {
            relationshipElement->SetCardinality(Cardinality::zero);
        }
        else if (checkedRadioButton == oneRadioButton)
        {
            relationshipElement->SetCardinality(Cardinality::one);
        }
        else if (checkedRadioButton == manyRadioButton)
        {
            relationshipElement->SetCardinality(Cardinality::many);
        }
    }
}

void RelationshipPropertiesDialog::SourceTextBoxTextChanged()
{
    relationshipElement->Source().SetText(sourceTextBox->Text());
}

void RelationshipPropertiesDialog::TargetTextBoxTextChanged()
{
    relationshipElement->Target().SetText(targetTextBox->Text());
}

} // namespace springpp
