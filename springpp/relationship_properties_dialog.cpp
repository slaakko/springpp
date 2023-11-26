// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

module springpp.relationship_properties_dialog;

import springpp.relationship_element;
import springpp.connector_dialog;
import springpp.text_element;
import springpp.text_element_properties_dialog;

namespace springpp {

RelationshipPropertiesDialog::RelationshipPropertiesDialog(RelationshipElement* relationshipElement_) : 
    wing::Window(wing::WindowCreateParams().Defaults().Text("Relationship Properties").
        WindowStyle(wing::DialogWindowStyle()).Location(wing::DefaultLocation()).
        WindowClassBackgroundColor(wing::DefaultControlWindowClassBackgroundColor()).BackgroundColor(wing::DefaultControlBackgroundColor()).
        SetSize(wing::Size(wing::ScreenMetrics::Get().MMToHorizontalPixels(120), wing::ScreenMetrics::Get().MMToVerticalPixels(100)))),
    relationshipElement(relationshipElement_), ready(false), relationGroupBox(nullptr), 
    inheritanceRadioButton(nullptr), compositionRadioButton(nullptr), aggregationRadioButton(nullptr), referenceRadioButton(nullptr), createInstanceRadioButton(nullptr),
    attachNoteRadioButton(nullptr),
    cardinalityGroupBox(nullptr), zeroRadioButton(nullptr), oneRadioButton(nullptr), manyRadioButton(nullptr), okButton(nullptr), cancelButton(nullptr), 
    sourceGroupBox(nullptr), primarySourceTextButton(nullptr), secondarySourceTextButton(nullptr), sourceConnectorButton(nullptr),
    targetGroupBox(nullptr), primaryTargetTextButton(nullptr), secondaryTargetTextButton(nullptr), targetConnectorButton(nullptr),
    sourceConnectorTextLabel(nullptr), targetConnectorTextLabel(nullptr)
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
    wing::Point createInstanceRadioButtonLoc = createInstanceRadioButton->Location();
    wing::Size createInstanceRadioButtonSize = createInstanceRadioButton->GetSize();

    std::unique_ptr<wing::RadioButton> attachNoteRadioButtonPtr(new wing::RadioButton(wing::RadioButtonCreateParams().Defaults().Text("Attach Note").
        Location(wing::Point(createInstanceRadioButtonLoc.X, createInstanceRadioButtonLoc.Y + createInstanceRadioButtonSize.Height)).
        SetAnchors((wing::Anchors::top | wing::Anchors::left))));
    attachNoteRadioButton = attachNoteRadioButtonPtr.get();
    attachNoteRadioButton->CheckedChanged().AddHandler(this, &RelationshipPropertiesDialog::RelationRadioButtonStateChanged);
    relationGroupBox->AddChild(attachNoteRadioButtonPtr.release());

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

    wing::Size sourceGroupBoxSize(wing::ScreenMetrics::Get().MMToHorizontalPixels(40), wing::ScreenMetrics::Get().MMToVerticalPixels(30));
    std::unique_ptr<wing::GroupBox> sourceGroupBoxPtr(new wing::GroupBox(wing::GroupBoxCreateParams().Defaults().Text("Source").
        Location(wing::Point(defaultControlSpacing.Width, defaultControlSpacing.Height + cardinalityGroupBox->Location().Y + cardinalityGroupBox->GetSize().Height)).
        SetSize(sourceGroupBoxSize).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    sourceGroupBox = sourceGroupBoxPtr.get();
    AddChild(sourceGroupBoxPtr.release());
    wing::Point primarySourceTextLabelLoc(defaultControlSpacing.Width, 3 * defaultControlSpacing.Height);
    std::unique_ptr<wing::Label> primarySourceTextLabelPtr(new wing::Label(wing::LabelCreateParams().Defaults().Text("Primary text:").Location(primarySourceTextLabelLoc).
        SetSize(defaultLabelSize).AutoSize(true).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    wing::Label* primarySourceTextLabel = primarySourceTextLabelPtr.get();
    sourceGroupBox->AddChild(primarySourceTextLabelPtr.release());
    wing::Size primarySourceTextButtonSize(wing::ScreenMetrics::Get().MMToHorizontalPixels(8), defaultButtonSize.Height);
    wing::Point primarySourceTextButtonLoc(sourceGroupBoxSize.Width - primarySourceTextButtonSize.Width - defaultControlSpacing.Width, 3 * defaultControlSpacing.Height - 4);
    std::unique_ptr<wing::Button> primarySourceTextButtonPtr(new wing::Button(wing::ControlCreateParams().Defaults().Text("...").Location(primarySourceTextButtonLoc).
        SetSize(primarySourceTextButtonSize).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    primarySourceTextButton = primarySourceTextButtonPtr.get();
    primarySourceTextButton->Click().AddHandler(this, &RelationshipPropertiesDialog::EditPrimarySourceText);
    sourceGroupBox->AddChild(primarySourceTextButtonPtr.release());
    wing::Point secondarySourceTextLabelLoc(defaultControlSpacing.Width, 3 * defaultControlSpacing.Height + defaultButtonSize.Height);
    std::unique_ptr<wing::Label> secondarySourceTextLabelPtr(new wing::Label(wing::LabelCreateParams().Defaults().Text("Secondary text:").Location(secondarySourceTextLabelLoc).
        SetSize(defaultLabelSize).AutoSize(true).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    wing::Label* secondarySourceTextLabel = secondarySourceTextLabelPtr.get();
    sourceGroupBox->AddChild(secondarySourceTextLabelPtr.release());
    wing::Size secondarySourceTextButtonSize(wing::ScreenMetrics::Get().MMToHorizontalPixels(8), defaultButtonSize.Height);
    wing::Point secondarySourceTextButtonLoc(sourceGroupBoxSize.Width - secondarySourceTextButtonSize.Width - defaultControlSpacing.Width, 
        3 * defaultControlSpacing.Height - 4 + defaultButtonSize.Height);
    std::unique_ptr<wing::Button> secondarySourceTextButtonPtr(new wing::Button(wing::ControlCreateParams().Defaults().Text("...").Location(secondarySourceTextButtonLoc).
        SetSize(secondarySourceTextButtonSize).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    secondarySourceTextButton = secondarySourceTextButtonPtr.get();
    secondarySourceTextButton->Click().AddHandler(this, &RelationshipPropertiesDialog::EditSecondarySourceText);
    sourceGroupBox->AddChild(secondarySourceTextButtonPtr.release());
    wing::Point sourceConnectorLabelLoc(defaultControlSpacing.Width, 3 * defaultControlSpacing.Height + 2 * defaultButtonSize.Height);
    std::unique_ptr<wing::Label> sourceConnectorLabelPtr(new wing::Label(wing::LabelCreateParams().Defaults().Text("Connector:").Location(sourceConnectorLabelLoc).
        SetSize(defaultLabelSize).AutoSize(true).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    wing::Label* sourceConnectorLabel = sourceConnectorLabelPtr.get();
    sourceGroupBox->AddChild(sourceConnectorLabelPtr.release());
    wing::Point sourceConnectorTextLabelLoc(defaultControlSpacing.Width, 3 * defaultControlSpacing.Height + 3 * defaultButtonSize.Height);
    std::unique_ptr<wing::Label> sourceConnectorTextLabelPtr(new wing::Label(wing::LabelCreateParams().Defaults().Text(relationshipElement->Source().GetConnector().ToString()).
        Location(sourceConnectorTextLabelLoc).SetSize(defaultLabelSize).AutoSize(true).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    sourceConnectorTextLabel = sourceConnectorTextLabelPtr.get();
    sourceGroupBox->AddChild(sourceConnectorTextLabelPtr.release());
    wing::Size sourceConnectorButtonSize(wing::ScreenMetrics::Get().MMToHorizontalPixels(8), defaultButtonSize.Height);
    wing::Point sourceConnectorButtonLoc(sourceGroupBoxSize.Width - secondarySourceTextButtonSize.Width - defaultControlSpacing.Width,
        3 * defaultControlSpacing.Height - 4 + 2 * defaultButtonSize.Height);
    std::unique_ptr<wing::Button> sourceConnectorButtonPtr(new wing::Button(wing::ControlCreateParams().Defaults().Text("...").Location(sourceConnectorButtonLoc).
        SetSize(sourceConnectorButtonSize).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    sourceConnectorButton = sourceConnectorButtonPtr.get();
    sourceConnectorButton->Click().AddHandler(this, &RelationshipPropertiesDialog::EditSourceConnector);
    sourceGroupBox->AddChild(sourceConnectorButtonPtr.release());

    wing::Size targetGroupBoxSize(wing::ScreenMetrics::Get().MMToHorizontalPixels(40), wing::ScreenMetrics::Get().MMToVerticalPixels(30));
    std::unique_ptr<wing::GroupBox> targetGroupBoxPtr(new wing::GroupBox(wing::GroupBoxCreateParams().Defaults().Text("Target").
        Location(wing::Point(defaultControlSpacing.Width + relationGroupBoxLoc.X + relationGroupBoxSize.Width,
            defaultControlSpacing.Height + cardinalityGroupBox->Location().Y + cardinalityGroupBox->GetSize().Height)).
        SetSize(targetGroupBoxSize).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    targetGroupBox = targetGroupBoxPtr.get();
    AddChild(targetGroupBoxPtr.release());
    wing::Point primaryTargetTextLabelLoc(defaultControlSpacing.Width, 3 * defaultControlSpacing.Height);
    std::unique_ptr<wing::Label> primaryTargetTextLabelPtr(new wing::Label(wing::LabelCreateParams().Defaults().Text("Primary text:").Location(primaryTargetTextLabelLoc).
        SetSize(defaultLabelSize).AutoSize(true).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    wing::Label* primaryTargetTextLabel = primaryTargetTextLabelPtr.get();
    targetGroupBox->AddChild(primaryTargetTextLabelPtr.release());
    wing::Size primaryTargetTextButtonSize(wing::ScreenMetrics::Get().MMToHorizontalPixels(8), defaultButtonSize.Height);
    wing::Point primaryTargetTextButtonLoc(targetGroupBoxSize.Width - primaryTargetTextButtonSize.Width - defaultControlSpacing.Width, 3 * defaultControlSpacing.Height - 4);
    std::unique_ptr<wing::Button> primaryTargetTextButtonPtr(new wing::Button(wing::ControlCreateParams().Defaults().Text("...").Location(primaryTargetTextButtonLoc).
        SetSize(primaryTargetTextButtonSize).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    primaryTargetTextButton = primaryTargetTextButtonPtr.get();
    primaryTargetTextButton->Click().AddHandler(this, &RelationshipPropertiesDialog::EditPrimaryTargetText);
    targetGroupBox->AddChild(primaryTargetTextButtonPtr.release());
    wing::Point secondaryTargetTextLabelLoc(defaultControlSpacing.Width, 3 * defaultControlSpacing.Height + defaultButtonSize.Height);
    std::unique_ptr<wing::Label> secondaryTargetTextLabelPtr(new wing::Label(wing::LabelCreateParams().Defaults().Text("Secondary text:").Location(secondaryTargetTextLabelLoc).
        SetSize(defaultLabelSize).AutoSize(true).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    wing::Label* secondaryTargetTextLabel = secondaryTargetTextLabelPtr.get();
    targetGroupBox->AddChild(secondaryTargetTextLabelPtr.release());
    wing::Size secondaryTargetTextButtonSize(wing::ScreenMetrics::Get().MMToHorizontalPixels(8), defaultButtonSize.Height);
    wing::Point secondaryTargetTextButtonLoc(targetGroupBoxSize.Width - secondaryTargetTextButtonSize.Width - defaultControlSpacing.Width,
        3 * defaultControlSpacing.Height - 4 + defaultButtonSize.Height);
    std::unique_ptr<wing::Button> secondaryTargetTextButtonPtr(new wing::Button(wing::ControlCreateParams().Defaults().Text("...").Location(secondaryTargetTextButtonLoc).
        SetSize(secondaryTargetTextButtonSize).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    secondaryTargetTextButton = secondaryTargetTextButtonPtr.get();
    secondaryTargetTextButton->Click().AddHandler(this, &RelationshipPropertiesDialog::EditSecondaryTargetText);
    targetGroupBox->AddChild(secondaryTargetTextButtonPtr.release());
    wing::Point targetConnectorLabelLoc(defaultControlSpacing.Width, 3 * defaultControlSpacing.Height + 2 * defaultButtonSize.Height);
    std::unique_ptr<wing::Label> targetConnectorLabelPtr(new wing::Label(wing::LabelCreateParams().Defaults().Text("Connector:").Location(targetConnectorLabelLoc).
        SetSize(defaultLabelSize).AutoSize(true).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    wing::Label* targetConnectorLabel = targetConnectorLabelPtr.get();
    targetGroupBox->AddChild(targetConnectorLabelPtr.release());
    wing::Point targetConnectorTextLabelLoc(defaultControlSpacing.Width, 3 * defaultControlSpacing.Height + 3 * defaultButtonSize.Height);
    std::unique_ptr<wing::Label> targetConnectorTextLabelPtr(new wing::Label(wing::LabelCreateParams().Defaults().Text(relationshipElement->Target().GetConnector().ToString()).
        Location(targetConnectorTextLabelLoc).SetSize(defaultLabelSize).AutoSize(true).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    targetConnectorTextLabel = targetConnectorTextLabelPtr.get();
    targetGroupBox->AddChild(targetConnectorTextLabelPtr.release());

    wing::Size targetConnectorButtonSize(wing::ScreenMetrics::Get().MMToHorizontalPixels(8), defaultButtonSize.Height);
    wing::Point targetConnectorButtonLoc(targetGroupBoxSize.Width - secondaryTargetTextButtonSize.Width - defaultControlSpacing.Width,
        3 * defaultControlSpacing.Height - 4 + 2 * defaultButtonSize.Height);
    std::unique_ptr<wing::Button> targetConnectorButtonPtr(new wing::Button(wing::ControlCreateParams().Defaults().Text("...").Location(targetConnectorButtonLoc).
        SetSize(targetConnectorButtonSize).SetAnchors(wing::Anchors::top | wing::Anchors::left)));
    targetConnectorButton = targetConnectorButtonPtr.get();
    targetConnectorButton->Click().AddHandler(this, &RelationshipPropertiesDialog::EditTargetConnector);
    targetGroupBox->AddChild(targetConnectorButtonPtr.release());

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
}

void RelationshipPropertiesDialog::EditPrimarySourceText()
{
    TextElementPropertiesDialog dialog("Primary Source Text", relationshipElement->Source().PrimaryTextElement());
    if (dialog.ShowDialog(*this) == wing::DialogResult::ok)
    {
        relationshipElement->Source().PrimaryTextElement()->SetLines(dialog.Lines());
        relationshipElement->Source().PrimaryTextElement()->SetKeyword(dialog.Keyword());
    }
}

void RelationshipPropertiesDialog::EditSecondarySourceText()
{
    TextElementPropertiesDialog dialog("Secondary Source Text", relationshipElement->Source().SecondaryTextElement());
    if (dialog.ShowDialog(*this) == wing::DialogResult::ok)
    {
        relationshipElement->Source().SecondaryTextElement()->SetLines(dialog.Lines());
        relationshipElement->Source().SecondaryTextElement()->SetKeyword(dialog.Keyword());
    }
}

void RelationshipPropertiesDialog::EditSourceConnector()
{
    ConnectorDialog connectorDialog(relationshipElement->Source().GetConnector());
    if (connectorDialog.ShowDialog(*this) == wing::DialogResult::ok)
    {
        relationshipElement->Source().SetConnector(connectorDialog.GetConnector());
        sourceConnectorTextLabel->SetText(relationshipElement->Source().GetConnector().ToString());
    }
}

void RelationshipPropertiesDialog::EditPrimaryTargetText()
{
    TextElementPropertiesDialog dialog("Primary Target Text", relationshipElement->Target().PrimaryTextElement());
    if (dialog.ShowDialog(*this) == wing::DialogResult::ok)
    {
        relationshipElement->Target().PrimaryTextElement()->SetLines(dialog.Lines());
        relationshipElement->Target().PrimaryTextElement()->SetKeyword(dialog.Keyword());
    }
}

void RelationshipPropertiesDialog::EditSecondaryTargetText()
{
    TextElementPropertiesDialog dialog("Secondary Target Text", relationshipElement->Target().SecondaryTextElement());
    if (dialog.ShowDialog(*this) == wing::DialogResult::ok)
    {
        relationshipElement->Target().SecondaryTextElement()->SetLines(dialog.Lines());
        relationshipElement->Target().SecondaryTextElement()->SetKeyword(dialog.Keyword());
    }
}

void RelationshipPropertiesDialog::EditTargetConnector()
{
    ConnectorDialog connectorDialog(relationshipElement->Target().GetConnector());
    if (connectorDialog.ShowDialog(*this) == wing::DialogResult::ok)
    {
        relationshipElement->Target().SetConnector(connectorDialog.GetConnector());
        targetConnectorTextLabel->SetText(relationshipElement->Target().GetConnector().ToString());
    }
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
        case RelationshipKind::attachNote:
        {
            relationGroupBox->SetCheckedRadioButton(attachNoteRadioButton);
            zeroRadioButton->Disable();
            oneRadioButton->Disable();
            manyRadioButton->Disable();
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
        else if (checkedRadioButton == attachNoteRadioButton)
        {
            relationshipElement->SetRKind(RelationshipKind::attachNote);
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
        else if (checkedRadioButton == attachNoteRadioButton)
        {
            zeroRadioButton->Disable();
            oneRadioButton->Disable();
            manyRadioButton->Disable();
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

} // namespace springpp
