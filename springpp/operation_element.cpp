// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

module springpp.operation_element;

import springpp.relationship_element;
import springpp.configuration;
import springpp.layout;
import springpp.container_element;
import soul.xml.xpath;

namespace springpp {

OperationElementRep::OperationElementRep(OperationElement* operationElement_) : operationElement(operationElement_)
{
}

OperationElementRep::~OperationElementRep()
{
}

void OperationElementRep::Measure(wing::Graphics& graphics)
{
    Layout* layout = Configuration::Instance().GetLayout();
    OperationLayoutElement* operationLayout = GetOperationLayout(layout);
    wing::Font* font = operationLayout->GetFontElement()->GetFont();
    wing::RectF r = wing::MeasureString(graphics, operationElement->Name(), *font, wing::PointF(0, 0), layout->GetStringFormat());
    wing::SizeF sz;
    r.GetSize(&sz);
    operationElement->SetSize(sz);
}

void OperationElementRep::Draw(wing::Graphics& graphics)
{
    Layout* layout = Configuration::Instance().GetLayout();
    OperationLayoutElement* operationLayout = GetOperationLayout(layout);
    wing::Font* font = operationLayout->GetFontElement()->GetFont();
    wing::Brush* brush = operationLayout->GetTextColorElement()->GetBrush();
    wing::DrawString(graphics, operationElement->Name(), *font, operationElement->Location(), *brush);
}

class ConcreteOperationRep : public OperationElementRep
{
public:
    ConcreteOperationRep(OperationElement* operationElement_);
private:
    OperationLayoutElement* GetOperationLayout(Layout* layout) const override;
};

ConcreteOperationRep::ConcreteOperationRep(OperationElement* operationElement_) : OperationElementRep(operationElement_)
{
}

OperationLayoutElement* ConcreteOperationRep::GetOperationLayout(Layout* layout) const
{
    return layout->GetConcreteOperationLayoutElement();
}

class AbstractOperationRep : public OperationElementRep
{
public:
    AbstractOperationRep(OperationElement* operationElement_);
private:
    OperationLayoutElement* GetOperationLayout(Layout* layout) const override;
};

AbstractOperationRep::AbstractOperationRep(OperationElement* operationElement_) : OperationElementRep(operationElement_)
{
}

OperationLayoutElement* AbstractOperationRep::GetOperationLayout(Layout* layout) const
{
    return layout->GetAbstractOperationLayoutElement();
}

OperationElement::OperationElement() : 
    DiagramElement(DiagramElementKind::operationElement), 
    isAbstract(false), 
    rep(new ConcreteOperationRep(this)), 
    containerElement(nullptr), 
    relationship(nullptr)
{
}

DiagramElement* OperationElement::Clone() const
{
    OperationElement* clone = new OperationElement();
    clone->SetName(Name());
    clone->SetBoundingRect(BoundingRect());
    if (isAbstract)
    {
        clone->SetAbstract();
    }
    clone->SetContainerElement(containerElement);
    return clone;
}

void OperationElement::AddRelationship(RelationshipElement* relationship_)
{
    relationship = relationship_;
}

void OperationElement::RemoveRelationship(RelationshipElement* relationship_)
{
    if (relationship == relationship_)
    {
        relationship = nullptr;
    }
}

EndPoint OperationElement::GetEndPoint(const Snap& snap) const
{
    EndPoint endPoint(const_cast<OperationElement*>(this), snap, GetRelationshipPoint());
    return endPoint;
}

void OperationElement::SetRelationshipPoint()
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

wing::PointF OperationElement::GetRelationshipPoint() const
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

void OperationElement::SetAbstract()
{
    if (!IsAbstract())
    {
        isAbstract = true;
        rep.reset(new AbstractOperationRep(this));
    }
}

void OperationElement::ResetAbstract()
{
    if (IsAbstract())
    {
        isAbstract = false;
        rep.reset(new ConcreteOperationRep(this));
    }
}

void OperationElement::Measure(wing::Graphics& graphics)
{
    rep->Measure(graphics);
}

void OperationElement::Draw(wing::Graphics& graphics)
{
    rep->Draw(graphics);
}

soul::xml::Element* OperationElement::ToXml() const
{
    soul::xml::Element* xmlElement = soul::xml::MakeElement("operation");
    soul::xml::Element* boundingRectElement = soul::xml::MakeElement("boundingRect");
    wing::RectF boundingRect = BoundingRect();
    boundingRectElement->SetAttribute("x", std::to_string(boundingRect.X));
    boundingRectElement->SetAttribute("y", std::to_string(boundingRect.Y));
    boundingRectElement->SetAttribute("width", std::to_string(boundingRect.Width));
    boundingRectElement->SetAttribute("height", std::to_string(boundingRect.Height));
    xmlElement->AppendChild(boundingRectElement);
    xmlElement->SetAttribute("name", Name());
    xmlElement->SetAttribute("abstract", isAbstract ? "true" : "false");
    return xmlElement;
}

void OperationElement::Parse(soul::xml::Element* xmlElement)
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
                throw std::runtime_error("XML element 'operationElement' has no 'x' attribute");
            }
            std::string yStr = boundingRectElement->GetAttribute("y");
            if (!yStr.empty())
            {
                boundingRect.Y = std::stof(yStr);
            }
            else
            {
                throw std::runtime_error("XML element 'operationElement' has no 'y' attribute");
            }
            std::string widthStr = boundingRectElement->GetAttribute("width");
            if (!widthStr.empty())
            {
                boundingRect.Width = std::stof(widthStr);
            }
            else
            {
                throw std::runtime_error("XML element 'operationElement' has no 'width' attribute");
            }
            std::string heightStr = boundingRectElement->GetAttribute("height");
            if (!heightStr.empty())
            {
                boundingRect.Height = std::stof(heightStr);
            }
            else
            {
                throw std::runtime_error("XML element 'operationElement' has no 'height' attribute");
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
    std::string nameStr = xmlElement->GetAttribute("name");
    if (!nameStr.empty())
    {
        SetName(nameStr);
    }
    else
    {
        throw std::runtime_error("XML element 'operationElement' has no 'name' attribute");
    }
    std::string abstractStr = xmlElement->GetAttribute("abstract");
    if (!abstractStr.empty())
    {
        if (abstractStr == "true")
        {
            SetAbstract();
        }
    }
    else
    {
        throw std::runtime_error("XML element 'operationElement' has no 'abstract' attribute");
    }
}

} // namespace springpp
