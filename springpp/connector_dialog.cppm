// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

export module springpp.connector_dialog;

import springpp.connector;
import wing;
import std.core;

export namespace springpp {

class ConnectorDialog : public wing::Window
{
public:
    ConnectorDialog(Connector connector_);
    Connector GetConnector() const { return connector; }
private:
    void RadioButtonCheckedChanged();
    bool ready;
    Connector connector;
    wing::Button* cancelButton;
    wing::Button* okButton;
    wing::RadioButton* topLeftRadioButton;
    wing::RadioButton* topCenterRadioButton;
    wing::RadioButton* topRightRadioButton;
    wing::RadioButton* rightTopRadioButton;
    wing::RadioButton* rightCenterRadioButton;
    wing::RadioButton* rightBottomRadioButton;
    wing::RadioButton* bottomLeftRadioButton;
    wing::RadioButton* bottomCenterRadioButton;
    wing::RadioButton* bottomRightRadioButton;
    wing::RadioButton* leftTopRadioButton;
    wing::RadioButton* leftCenterRadioButton;
    wing::RadioButton* leftBottomRadioButton;
    std::map<wing::RadioButton*, Connector> radioButtonMap;
    std::map<Connector, wing::RadioButton*> connectorMap;
};

} // namespace springpp
