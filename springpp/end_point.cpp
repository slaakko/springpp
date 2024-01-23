// =================================
// Copyright (c) 2024 Seppo Laakko
// Distributed under the MIT license
// =================================

module springpp.end_point;

import springpp.diagram_element;
import springpp.container_element;
import springpp.diagram;
import springpp.attribute_element;
import springpp.operation_element;
import springpp.text_element;
import soul.xml.xpath;

namespace springpp {

EndPoint::EndPoint() : element(nullptr), connector(), point(), primaryTextElement(new TextElement("primaryText")), secondaryTextElement(new TextElement("secondaryText")), index(-1)
{
}

EndPoint::EndPoint(DiagramElement* element_, const Connector& connector_, const wing::PointF& point_) : 
    element(element_), connector(connector_), point(point_), primaryTextElement(new TextElement("primaryText")), secondaryTextElement(new TextElement("secondaryText")), index(-1)
{
}

EndPoint::EndPoint(const EndPoint& that) : 
    element(that.element), connector(that.connector), point(that.point), primaryTextElement(nullptr), secondaryTextElement(nullptr), index(that.index)
{
    if (that.primaryTextElement)
    {
        primaryTextElement = static_cast<TextElement*>(that.primaryTextElement->Clone());
    }
    if (that.secondaryTextElement)
    {
        secondaryTextElement = static_cast<TextElement*>(that.secondaryTextElement->Clone());
    }
}

EndPoint& EndPoint::operator=(const EndPoint& that) 
{
    element = that.element;
    connector = that.connector;
    point = that.point;
    primaryTextElement = nullptr;
    if (that.primaryTextElement)
    {
        primaryTextElement = static_cast<TextElement*>(that.primaryTextElement->Clone());
    }
    secondaryTextElement = nullptr;
    if (that.secondaryTextElement)
    {
        secondaryTextElement = static_cast<TextElement*>(that.secondaryTextElement->Clone());
    }
    return *this;
}

EndPoint::EndPoint(EndPoint&& that) : 
    element(that.element), 
    connector(std::move(that.connector)), 
    point(std::move(that.point)), 
    primaryTextElement(nullptr),
    secondaryTextElement(nullptr)
{
    if (that.primaryTextElement)
    {
        primaryTextElement = static_cast<TextElement*>(that.primaryTextElement->Clone());
    }
    secondaryTextElement = nullptr;
    if (that.secondaryTextElement)
    {
        secondaryTextElement = static_cast<TextElement*>(that.secondaryTextElement->Clone());
    }
}

EndPoint& EndPoint::operator=(EndPoint&& that)
{
    std::swap(element, that.element);
    std::swap(connector, that.connector);
    std::swap(point, that.point);
    std::swap(primaryTextElement, that.primaryTextElement);
    std::swap(secondaryTextElement, that.secondaryTextElement);
    std::swap(index, that.index);
    return *this;
}

EndPoint::~EndPoint()
{
    if (primaryTextElement)
    {
        delete primaryTextElement;
    }
    if (secondaryTextElement)
    {
        delete secondaryTextElement;
    }
}

void EndPoint::SetIndex(const std::map<ContainerElement*, int>& containerElementIndexMap)
{
    if (element)
    {
        ContainerElement* containerElement = element->GetContainerElement();
        auto it = containerElementIndexMap.find(containerElement);
        if (it != containerElementIndexMap.end())
        {
            index = it->second;
        }
        else
        {
            throw std::runtime_error("EndPoint::SetIndex: index for container element '" + containerElement->Name() + "' not found");
        }
    }
    else
    {
        index = -1;
    }
}

void EndPoint::MapIndex(const std::map<int, int>& indexMap)
{
    if (index != -1)
    {
        auto it = indexMap.find(index);
        if (it != indexMap.end())
        {
            index = it->second;
        }
        else
        {
            throw std::runtime_error("EndPoint::MapIndex: index " + std::to_string(index) + " not found from index map");
        }
    }
}

void EndPoint::Resolve(Diagram* diagram)
{
    if (index != -1)
    {
        element = diagram->GetElementByIndex(index);
        if (connector.PrimaryPoint() == ConnectorPoint::attribute)
        {
            if (element->IsContainerElement())
            {
                ContainerElement* containerElement = static_cast<ContainerElement*>(element);
                element = containerElement->GetAttribute(static_cast<int>(connector.SecondaryPoint()));
            }
            else
            {
                throw std::runtime_error("EndPoint::Resolve: container element expected");
            }
        }
        else if (connector.PrimaryPoint() == ConnectorPoint::operation)
        {
            if (element->IsContainerElement())
            {
                ContainerElement* containerElement = static_cast<ContainerElement*>(element);
                element = containerElement->GetOperation(static_cast<int>(connector.SecondaryPoint()));
            }
            else
            {
                throw std::runtime_error("EndPoint::Resolve: container element expected");
            }
        }
    }
    else
    {
        element = nullptr;
    }
}

void EndPoint::SetConnector(const Connector& connector_)
{
    connector = connector_;
}

void EndPoint::SetPoint(const wing::PointF& point_)
{
    point = point_;
}

soul::xml::Element* EndPoint::ToXml(const std::string& elementName) const
{
    soul::xml::Element* xmlElement = soul::xml::MakeElement(elementName);
    if (index != -1)
    {
        xmlElement->SetAttribute("index", std::to_string(index));
    }
    xmlElement->SetAttribute("connector", connector.ToString());
    xmlElement->SetAttribute("x", std::to_string(point.X));
    xmlElement->SetAttribute("y", std::to_string(point.Y));
    xmlElement->AppendChild(primaryTextElement->ToXml());
    xmlElement->AppendChild(secondaryTextElement->ToXml());
    return xmlElement;
}

void EndPoint::Parse(soul::xml::Element* xmlElement)
{
    std::string indexStr = xmlElement->GetAttribute("index");
    if (indexStr.empty())
    {
        index = -1;
    }
    else
    {
        index = std::stoi(indexStr);
    }
    std::string connectorStr = xmlElement->GetAttribute("connector");
    connector = ParseConnector(connectorStr);
    std::string xStr = xmlElement->GetAttribute("x");
    std::string yStr = xmlElement->GetAttribute("y");
    point.X = std::stof(xStr);
    point.Y = std::stof(yStr);
    std::unique_ptr<soul::xml::xpath::NodeSet> primaryNodeSet = soul::xml::xpath::EvaluateToNodeSet(primaryTextElement->XmlElementName(), xmlElement);
    if (primaryNodeSet->Count() == 1)
    {
        soul::xml::Node* node = primaryNodeSet->GetNode(0);
        if (node->IsElementNode())
        {
            soul::xml::Element* primaryXmlElement = static_cast<soul::xml::Element*>(node);
            primaryTextElement->Parse(primaryXmlElement);
        }
        else
        {
            throw std::runtime_error("XML element node expected in '" + xmlElement->Name() + "'");
        }
    }
    else
    {
        throw std::runtime_error("XML element '" + primaryTextElement->XmlElementName() + "' not unique in '" + xmlElement->Name() + "'");
    }
    std::unique_ptr<soul::xml::xpath::NodeSet> secondaryNodeSet = soul::xml::xpath::EvaluateToNodeSet(secondaryTextElement->XmlElementName(), xmlElement);
    if (secondaryNodeSet->Count() == 1)
    {
        soul::xml::Node* node = secondaryNodeSet->GetNode(0);
        if (node->IsElementNode())
        {
            soul::xml::Element* secondaryXmlElement = static_cast<soul::xml::Element*>(node);
            secondaryTextElement->Parse(secondaryXmlElement);
        }
        else
        {
            throw std::runtime_error("XML element node expected in '" + xmlElement->Name() + "'");
        }
    }
    else
    {
        throw std::runtime_error("XML element '" + secondaryTextElement->XmlElementName() + "' not unique in '" + xmlElement->Name() + "'");
    }
}

void EndPoint::MeasureTexts(wing::Graphics& graphics)
{
    if (primaryTextElement && !primaryTextElement->IsEmpty())
    {
        primaryTextElement->Measure(graphics);
    }
    if (secondaryTextElement && !secondaryTextElement->IsEmpty())
    {
        secondaryTextElement->Measure(graphics);
    }
}

EndPointNearer::EndPointNearer(const wing::PointF& location_) : location(location_)
{
}

bool EndPointNearer::operator()(const EndPoint* left, const EndPoint* right) const
{
    return Distance(left->Point(), location) < Distance(right->Point(), location);
}

} // namespace springpp

