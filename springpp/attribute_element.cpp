// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

module springpp.attribute_element;

import soul.xml.xpath;
import springpp.relationship_element;
import springpp.configuration;
import springpp.layout;
import springpp.container_element;

namespace springpp {

AttributeElement::AttributeElement() : DiagramElement(DiagramElementKind::attributeElement), containerElement(nullptr), relationship(nullptr)
{
}

DiagramElement* AttributeElement::Clone() const
{
    AttributeElement* clone = new AttributeElement();
    clone->SetName(Name());
    clone->SetBounds(Bounds());
    clone->SetContainerElement(containerElement);
    return clone;
}

void AttributeElement::SetContainerElement(ContainerElement* containerElement_)
{
    containerElement = containerElement_;
}

void AttributeElement::AddRelationship(RelationshipElement* relationship_)
{
    relationship = relationship_;
}

void AttributeElement::RemoveRelationship(RelationshipElement* relationship_)
{
    if (relationship == relationship_)
    {
        relationship = nullptr;
    }
}

void AttributeElement::SetRelationshipPoint()
{
    if (relationship != nullptr)
    {
        relationship->Source().SetElement(this);
        relationship->Source().SetPoint(GetRelationshipPoint());
        for (auto& sourceEndPoint : relationship->SourceEndPoints())
        {
            sourceEndPoint.SetElement(this);
            sourceEndPoint.SetPoint(GetRelationshipPoint());
        }
    }
}

wing::PointF AttributeElement::GetRelationshipPoint() const
{
    Layout* layout = Configuration::Instance().GetLayout();
    RelationshipLayoutElement* relationshipLayoutElement = layout->GetRelationshipLayoutElement();
    PaddingElement* paddingElement = relationshipLayoutElement->GetPaddingElement();
    float relationshipSymbolRadius = relationshipLayoutElement->RelationshipSymbolRadius();
    wing::PointF loc = Location();
    wing::SizeF size = Size();
    float symbolLeft = 0.0f;
    if (containerElement != nullptr)
    {
        size.Width = containerElement->GetMaxChildElementWidth();
        size.Width -= GetRelationshipSymbolFieldWidth(relationshipSymbolRadius, paddingElement->GetPadding().Horizontal());
        symbolLeft = paddingElement->GetPadding().left + relationshipSymbolRadius;
    }
    wing::PointF point(loc.X + size.Width + symbolLeft, loc.Y + size.Height / 2);
    return point;
}

EndPoint AttributeElement::GetEndPoint(const Connector& connector) const
{
    EndPoint endPoint(const_cast<AttributeElement*>(this), connector, GetRelationshipPoint());
    return endPoint;
}

void AttributeElement::Measure(wing::Graphics& graphics)
{
    Layout* layout = Configuration::Instance().GetLayout();
    AttributeLayoutElement* attributeLayout = layout->GetAttributeLayoutElement();
    wing::Font* font = attributeLayout->GetFontElement()->GetFont();
    wing::RectF r = wing::MeasureString(graphics, Name(), *font, wing::PointF(0, 0), layout->GetStringFormat());
    wing::SizeF sz;
    r.GetSize(&sz);
    SetSize(sz);
}

void AttributeElement::Draw(wing::Graphics& graphics)
{
    Layout* layout = Configuration::Instance().GetLayout();
    AttributeLayoutElement* attributeLayout = layout->GetAttributeLayoutElement();
    wing::Font* font = attributeLayout->GetFontElement()->GetFont();
    wing::Brush* brush = attributeLayout->GetTextColorElement()->GetBrush();
    wing::DrawString(graphics, Name(), *font, Location(), *brush);
}

soul::xml::Element* AttributeElement::ToXml() const
{
    soul::xml::Element* xmlElement = soul::xml::MakeElement("attribute");
    soul::xml::Element* boundsElement = soul::xml::MakeElement("bounds");
    wing::RectF bounds = Bounds();
    boundsElement->SetAttribute("x", std::to_string(bounds.X));
    boundsElement->SetAttribute("y", std::to_string(bounds.Y));
    boundsElement->SetAttribute("width", std::to_string(bounds.Width));
    boundsElement->SetAttribute("height", std::to_string(bounds.Height));
    xmlElement->AppendChild(boundsElement);
    xmlElement->SetAttribute("name", Name());
    return xmlElement;
}

void AttributeElement::Parse(soul::xml::Element* xmlElement)
{
    std::unique_ptr<soul::xml::xpath::NodeSet> nodeSet = soul::xml::xpath::EvaluateToNodeSet("bounds", xmlElement);
    if (nodeSet->Count() == 1)
    {
        soul::xml::Node* node = nodeSet->GetNode(0);
        if (node->IsElementNode())
        {
            wing::RectF bounds;
            soul::xml::Element* boundsElement = static_cast<soul::xml::Element*>(node);
            std::string xStr = boundsElement->GetAttribute("x");
            if (!xStr.empty())
            {
                bounds.X = std::stof(xStr);
            }
            else
            {
                throw std::runtime_error("XML element 'attributeElement' has no 'x' attribute");
            }
            std::string yStr = boundsElement->GetAttribute("y");
            if (!yStr.empty())
            {
                bounds.Y = std::stof(yStr);
            }
            else
            {
                throw std::runtime_error("XML element 'attributeElement' has no 'y' attribute");
            }
            std::string widthStr = boundsElement->GetAttribute("width");
            if (!widthStr.empty())
            {
                bounds.Width = std::stof(widthStr);
            }
            else
            {
                throw std::runtime_error("XML element 'attributeElement' has no 'width' attribute");
            }
            std::string heightStr = boundsElement->GetAttribute("height");
            if (!heightStr.empty())
            {
                bounds.Height = std::stof(heightStr);
            }
            else
            {
                throw std::runtime_error("XML element 'attributeElement' has no 'height' attribute");
            }
            SetBounds(bounds);
        }
        else
        {
            throw std::runtime_error("XML element node expected in '" + xmlElement->Name() + "'");
        }
    }
    else
    {
        throw std::runtime_error("XML element 'bounds' not unique in '" + xmlElement->Name() + "'");
    }
    std::string nameStr = xmlElement->GetAttribute("name");
    if (!nameStr.empty())
    {
        SetName(nameStr);
    }
    else
    {
        throw std::runtime_error("XML element 'attributeElement' has no 'name' attribute");
    }
}

} // namespace springpp
