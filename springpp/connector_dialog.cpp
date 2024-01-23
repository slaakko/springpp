// =================================
// Copyright (c) 2024 Seppo Laakko
// Distributed under the MIT license
// =================================

module springpp.connector_dialog;

namespace springpp {

ConnectorDialog::ConnectorDialog(Connector connector_) : wing::Window(wing::WindowCreateParams().Defaults().Text("Connector").
    WindowStyle(wing::DialogWindowStyle()).Location(wing::DefaultLocation()).
    WindowClassBackgroundColor(wing::DefaultControlWindowClassBackgroundColor()).BackgroundColor(wing::DefaultControlBackgroundColor()).
    SetSize(wing::Size(wing::ScreenMetrics::Get().MMToHorizontalPixels(60), wing::ScreenMetrics::Get().MMToVerticalPixels(60)))),
    connector(connector_), cancelButton(nullptr), okButton(nullptr), 
    topLeftRadioButton(nullptr), topCenterRadioButton(nullptr), topRightRadioButton(nullptr),
    rightTopRadioButton(nullptr), rightCenterRadioButton(nullptr), rightBottomRadioButton(nullptr),
    bottomLeftRadioButton(nullptr), bottomCenterRadioButton(nullptr), bottomRightRadioButton(nullptr),
    leftTopRadioButton(nullptr), leftCenterRadioButton(nullptr), leftBottomRadioButton(nullptr), ready(false)
{
    wing::Size s = GetSize();
    wing::Size defaultButtonSize = wing::ScreenMetrics::Get().DefaultButtonSize();
    wing::Size defaultControlSpacing = wing::ScreenMetrics::Get().DefaultControlSpacing();
    wing::Size radioButtonSize(wing::ScreenMetrics::Get().MMToHorizontalPixels(4.0f), wing::ScreenMetrics::Get().MMToVerticalPixels(4.0f));

    topLeftRadioButton = new wing::RadioButton(wing::RadioButtonCreateParams().Defaults().Location(
        wing::Point(radioButtonSize.Width + defaultControlSpacing.Width, defaultControlSpacing.Height)).
        SetSize(radioButtonSize).SetAnchors(wing::Anchors::left | wing::Anchors::top));
    topLeftRadioButton->CheckedChanged().AddHandler(this, &ConnectorDialog::RadioButtonCheckedChanged);
    AddChild(topLeftRadioButton);
    topCenterRadioButton = new wing::RadioButton(wing::RadioButtonCreateParams().Defaults().Location(
        wing::Point(radioButtonSize.Width * 2 + defaultControlSpacing.Width, defaultControlSpacing.Height)).
        SetSize(radioButtonSize).SetAnchors(wing::Anchors::left | wing::Anchors::top));
    topCenterRadioButton->CheckedChanged().AddHandler(this, &ConnectorDialog::RadioButtonCheckedChanged);
    AddChild(topCenterRadioButton);
    topRightRadioButton = new wing::RadioButton(wing::RadioButtonCreateParams().Defaults().Location(
        wing::Point(radioButtonSize.Width * 3 + defaultControlSpacing.Width, defaultControlSpacing.Height)).
        SetSize(radioButtonSize).SetAnchors(wing::Anchors::left | wing::Anchors::top));
    topRightRadioButton->CheckedChanged().AddHandler(this, &ConnectorDialog::RadioButtonCheckedChanged);
    AddChild(topRightRadioButton);
    leftTopRadioButton = new wing::RadioButton(wing::RadioButtonCreateParams().Defaults().Location(
        wing::Point(defaultControlSpacing.Width, defaultControlSpacing.Height + radioButtonSize.Height)).
        SetSize(radioButtonSize).SetAnchors(wing::Anchors::left | wing::Anchors::top));
    leftTopRadioButton->CheckedChanged().AddHandler(this, &ConnectorDialog::RadioButtonCheckedChanged);
    AddChild(leftTopRadioButton);
    rightTopRadioButton = new wing::RadioButton(wing::RadioButtonCreateParams().Defaults().Location(
        wing::Point(defaultControlSpacing.Width + 4 * radioButtonSize.Width, defaultControlSpacing.Height + radioButtonSize.Height)).
        SetSize(radioButtonSize).SetAnchors(wing::Anchors::left | wing::Anchors::top));
    rightTopRadioButton->CheckedChanged().AddHandler(this, &ConnectorDialog::RadioButtonCheckedChanged);
    AddChild(rightTopRadioButton);
    leftCenterRadioButton = new wing::RadioButton(wing::RadioButtonCreateParams().Defaults().Location(
        wing::Point(defaultControlSpacing.Width, defaultControlSpacing.Height + 2 * radioButtonSize.Height)).
        SetSize(radioButtonSize).SetAnchors(wing::Anchors::left | wing::Anchors::top));
    leftCenterRadioButton->CheckedChanged().AddHandler(this, &ConnectorDialog::RadioButtonCheckedChanged);
    AddChild(leftCenterRadioButton);
    rightCenterRadioButton = new wing::RadioButton(wing::RadioButtonCreateParams().Defaults().Location(
        wing::Point(defaultControlSpacing.Width + 4 * radioButtonSize.Width, defaultControlSpacing.Height + 2 * radioButtonSize.Height)).
        SetSize(radioButtonSize).SetAnchors(wing::Anchors::left | wing::Anchors::top));
    rightCenterRadioButton->CheckedChanged().AddHandler(this, &ConnectorDialog::RadioButtonCheckedChanged);
    AddChild(rightCenterRadioButton);
    leftBottomRadioButton = new wing::RadioButton(wing::RadioButtonCreateParams().Defaults().Location(
        wing::Point(defaultControlSpacing.Width, defaultControlSpacing.Height + 3 * radioButtonSize.Height)).
        SetSize(radioButtonSize).SetAnchors(wing::Anchors::left | wing::Anchors::top));
    leftBottomRadioButton->CheckedChanged().AddHandler(this, &ConnectorDialog::RadioButtonCheckedChanged);
    AddChild(leftBottomRadioButton);
    rightBottomRadioButton = new wing::RadioButton(wing::RadioButtonCreateParams().Defaults().Location(
        wing::Point(defaultControlSpacing.Width + 4 * radioButtonSize.Width, defaultControlSpacing.Height + 3 * radioButtonSize.Height)).
        SetSize(radioButtonSize).SetAnchors(wing::Anchors::left | wing::Anchors::top));
    rightBottomRadioButton->CheckedChanged().AddHandler(this, &ConnectorDialog::RadioButtonCheckedChanged);
    AddChild(rightBottomRadioButton);
    bottomLeftRadioButton = new wing::RadioButton(wing::RadioButtonCreateParams().Defaults().Location(
        wing::Point(radioButtonSize.Width + defaultControlSpacing.Width, defaultControlSpacing.Height + 4 * radioButtonSize.Height)).
        SetSize(radioButtonSize).SetAnchors(wing::Anchors::left | wing::Anchors::top));
    bottomLeftRadioButton->CheckedChanged().AddHandler(this, &ConnectorDialog::RadioButtonCheckedChanged);
    AddChild(bottomLeftRadioButton);
    bottomCenterRadioButton = new wing::RadioButton(wing::RadioButtonCreateParams().Defaults().Location(
        wing::Point(radioButtonSize.Width * 2 + defaultControlSpacing.Width, defaultControlSpacing.Height + 4 * radioButtonSize.Height)).
        SetSize(radioButtonSize).SetAnchors(wing::Anchors::left | wing::Anchors::top));
    bottomCenterRadioButton->CheckedChanged().AddHandler(this, &ConnectorDialog::RadioButtonCheckedChanged);
    AddChild(bottomCenterRadioButton);
    bottomRightRadioButton = new wing::RadioButton(wing::RadioButtonCreateParams().Defaults().Location(
        wing::Point(radioButtonSize.Width * 3 + defaultControlSpacing.Width, defaultControlSpacing.Height + 4 * radioButtonSize.Height)).
        SetSize(radioButtonSize).SetAnchors(wing::Anchors::left | wing::Anchors::top));
    bottomRightRadioButton->CheckedChanged().AddHandler(this, &ConnectorDialog::RadioButtonCheckedChanged);
    AddChild(bottomRightRadioButton);

    Connector topLeftConnector(ConnectorPoint::top, ConnectorPoint::left);
    radioButtonMap[topLeftRadioButton] = topLeftConnector;
    connectorMap[topLeftConnector] = topLeftRadioButton;
    Connector topCenterConnector(ConnectorPoint::top, ConnectorPoint::center);
    radioButtonMap[topCenterRadioButton] = topCenterConnector;
    connectorMap[topCenterConnector] = topCenterRadioButton;
    Connector topRightConnector(ConnectorPoint::top, ConnectorPoint::right);
    radioButtonMap[topRightRadioButton] = topRightConnector;
    connectorMap[topRightConnector] = topRightRadioButton;
    Connector rightTopConnector(ConnectorPoint::right, ConnectorPoint::top);
    radioButtonMap[rightTopRadioButton] = rightTopConnector;
    connectorMap[rightTopConnector] = rightTopRadioButton;
    Connector rightCenterConnector(ConnectorPoint::right, ConnectorPoint::center);
    radioButtonMap[rightCenterRadioButton] = rightCenterConnector;
    connectorMap[rightCenterConnector] = rightCenterRadioButton;
    Connector rightBottomConnector(ConnectorPoint::right, ConnectorPoint::bottom);
    radioButtonMap[rightBottomRadioButton] = rightBottomConnector;
    connectorMap[rightBottomConnector] = rightBottomRadioButton;
    Connector bottomLeftConnector(ConnectorPoint::bottom, ConnectorPoint::left);
    radioButtonMap[bottomLeftRadioButton] = bottomLeftConnector;
    connectorMap[bottomLeftConnector] = bottomLeftRadioButton;
    Connector bottomCenterConnector(ConnectorPoint::bottom, ConnectorPoint::center);
    radioButtonMap[bottomCenterRadioButton] = bottomCenterConnector;
    connectorMap[bottomCenterConnector] = bottomCenterRadioButton;
    Connector bottomRightConnector(ConnectorPoint::bottom, ConnectorPoint::right);
    radioButtonMap[bottomRightRadioButton] = bottomRightConnector;
    connectorMap[bottomRightConnector] = bottomRightRadioButton;
    Connector leftTopConnector(ConnectorPoint::left, ConnectorPoint::top);
    radioButtonMap[leftTopRadioButton] = leftTopConnector;
    connectorMap[leftTopConnector] = leftTopRadioButton;
    Connector leftCenterConnector(ConnectorPoint::left, ConnectorPoint::center);
    radioButtonMap[leftCenterRadioButton] = leftCenterConnector;
    connectorMap[leftCenterConnector] = leftCenterRadioButton;
    Connector leftBottomConnector(ConnectorPoint::left, ConnectorPoint::bottom);
    radioButtonMap[leftBottomRadioButton] = leftBottomConnector;
    connectorMap[leftBottomConnector] = leftBottomRadioButton;

    auto it = connectorMap.find(connector);
    if (it != connectorMap.end())
    {
        it->second->SetChecked(true);
    }

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
    ready = true;
}

void ConnectorDialog::RadioButtonCheckedChanged()
{
    if (!ready) return;
    ready = false;
    wing::RadioButton* prev = connectorMap[connector];
    if (prev)
    {
        prev->SetChecked(false);
    }
    for (const auto& rb : radioButtonMap)
    {
        if (rb.first->Checked())
        {
            if (connector != rb.second)
            {
                connector = rb.second;
            }
        }
    }
    ready = true;
}

} // namespace springpp
