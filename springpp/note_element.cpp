// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

module springpp.note_element;

import springpp.action;
import soul.xml.xpath;

namespace springpp {

NoteElement::NoteElement() : ContainerElement(DiagramElementKind::noteElement)
{
}

soul::xml::Element* NoteElement::ToXml() const
{
    soul::xml::Element* xmlElement = soul::xml::MakeElement("noteElement");
    soul::xml::Element* boundingRectElement = soul::xml::MakeElement("boundingRect");
    wing::RectF boundingRect = BoundingRect();
    boundingRectElement->SetAttribute("x", std::to_string(boundingRect.X));
    boundingRectElement->SetAttribute("y", std::to_string(boundingRect.Y));
    boundingRectElement->SetAttribute("width", std::to_string(boundingRect.Width));
    boundingRectElement->SetAttribute("height", std::to_string(boundingRect.Height));
    xmlElement->AppendChild(boundingRectElement);
    for (const auto& line : lines)
    {
        soul::xml::Element* lineElement = soul::xml::MakeElement("line");
        lineElement->SetAttribute("text", line);
        xmlElement->AppendChild(lineElement);
    }
    return xmlElement;
}

void NoteElement::Parse(soul::xml::Element* xmlElement)
{
    std::unique_ptr<soul::xml::xpath::NodeSet> nodeSet = soul::xml::xpath::EvaluateToNodeSet("boundingRect", xmlElement);
    if (nodeSet->Count() == 1)
    {
        soul::xml::Node* node = nodeSet->GetNode(0);
        if (node->IsElementNode())
        {
            wing::RectF boundingRect;
            soul::xml::Element* boundingRectElement = static_cast<soul::xml::Element*>(node);
            std::string xStr = boundingRectElement->GetAttribute("x");
            if (!xStr.empty())
            {
                boundingRect.X = std::stof(xStr);
            }
            else
            {
                throw std::runtime_error("XML element 'noteElement' has no 'x' attribute");
            }
            std::string yStr = boundingRectElement->GetAttribute("y");
            if (!yStr.empty())
            {
                boundingRect.Y = std::stof(yStr);
            }
            else
            {
                throw std::runtime_error("XML element 'noteElement' has no 'y' attribute");
            }
            std::string widthStr = boundingRectElement->GetAttribute("width");
            if (!widthStr.empty())
            {
                boundingRect.Width = std::stof(widthStr);
            }
            else
            {
                throw std::runtime_error("XML element 'noteElement' has no 'width' attribute");
            }
            std::string heightStr = boundingRectElement->GetAttribute("height");
            if (!heightStr.empty())
            {
                boundingRect.Height = std::stof(heightStr);
            }
            else
            {
                throw std::runtime_error("XML element 'noteElement' has no 'height' attribute");
            }
            SetBoundingRect(boundingRect);
        }
        else
        {
            throw std::runtime_error("XML element node expected in '" + xmlElement->Name() + "'");
        }
    }
    else
    {
        throw std::runtime_error("XML element 'boundingRect' not unique in '" + xmlElement->Name() + "'");
    }
    std::unique_ptr<soul::xml::xpath::NodeSet> lineNodeSet = soul::xml::xpath::EvaluateToNodeSet("line", xmlElement);
    int n = lineNodeSet->Count();
    for (int i = 0; i < n; ++i)
    {
        soul::xml::Node* node = nodeSet->GetNode(i);
        if (node->IsElementNode())
        {
            soul::xml::Element* lineElement = static_cast<soul::xml::Element*>(node);
            std::string textAttribute = lineElement->GetAttribute("text");
            lines.push_back(textAttribute);
        }
    }
}

void NoteElement::Measure(wing::Graphics& graphics)
{
    // todo
}

void NoteElement::Draw(wing::Graphics& graphics)
{
    // todo
}

DiagramElement* NoteElement::Clone() const
{
    NoteElement* clone = new NoteElement();
    clone->SetBoundingRect(BoundingRect());
    // todo
    return clone;
}

void NoteElement::AddActions(Diagram* diagram, int elementIndex, wing::ContextMenu* contextMenu) const
{
    ContainerElement::AddActions(diagram, elementIndex, contextMenu);
    wing::MenuItem* propertiesMenuItem = new wing::MenuItem("Properties...");
    contextMenu->AddMenuItem(propertiesMenuItem);
    contextMenu->AddAction(new NoteElementPropertiesAction(diagram, elementIndex, propertiesMenuItem));
}

std::vector<RelationshipElement*> NoteElement::GetAllRelationships() const
{
    return Relationships();
}

} // namespace springpp
