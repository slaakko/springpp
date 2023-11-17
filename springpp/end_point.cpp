// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

module springpp.end_point;

import springpp.diagram_element;
import springpp.container_element;
import springpp.diagram;
import springpp.attribute_element;
import springpp.operation_element;

namespace springpp {

EndPoint::EndPoint() : element(nullptr), connector(), point(), text(), index(-1)
{
}

EndPoint::EndPoint(DiagramElement* element_, const Connector& connector_, const wing::PointF& point_) : element(element_), connector(connector_), point(point_), text(), index(-1)
{
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
    xmlElement->SetAttribute("text", text);
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
    text = xmlElement->GetAttribute("text");
}

EndPointNearer::EndPointNearer(const wing::PointF& location_) : location(location_)
{
}

bool EndPointNearer::operator()(const EndPoint* left, const EndPoint* right) const
{
    return Distance(left->Point(), location) < Distance(right->Point(), location);
}

} // namespace springpp

