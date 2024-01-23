// =================================
// Copyright (c) 2024 Seppo Laakko
// Distributed under the MIT license
// =================================

module springpp.object_element;

import springpp.action;
import springpp.tool;
import springpp.layout;
import springpp.configuration;
import soul.xml.xpath;

namespace springpp {

ObjectElement::ObjectElement() : ContainerElement(DiagramElementKind::objectElement)
{
}

ObjectElement::ObjectElement(const std::string& name_) : ContainerElement(DiagramElementKind::objectElement, name_)
{
}

soul::xml::Element* ObjectElement::ToXml() const
{
    soul::xml::Element* xmlElement = soul::xml::MakeElement("objectElement");
    soul::xml::Element* boundsElement = soul::xml::MakeElement("bounds");
    wing::RectF bounds = Bounds();
    boundsElement->SetAttribute("x", std::to_string(bounds.X));
    boundsElement->SetAttribute("y", std::to_string(bounds.Y));
    boundsElement->SetAttribute("width", std::to_string(bounds.Width));
    boundsElement->SetAttribute("height", std::to_string(bounds.Height));
    xmlElement->AppendChild(boundsElement);
    xmlElement->SetAttribute("name", Name());
    for (const auto& attribute : attributes)
    {
        soul::xml::Element* attributeElement = attribute->ToXml();
        xmlElement->AppendChild(attributeElement);
    }
    return xmlElement;
}

void ObjectElement::Parse(soul::xml::Element* xmlElement)
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
                throw std::runtime_error("XML element 'objectElement' has no 'x' attribute");
            }
            std::string yStr = boundsElement->GetAttribute("y");
            if (!yStr.empty())
            {
                bounds.Y = std::stof(yStr);
            }
            else
            {
                throw std::runtime_error("XML element 'objectElement' has no 'y' attribute");
            }
            std::string widthStr = boundsElement->GetAttribute("width");
            if (!widthStr.empty())
            {
                bounds.Width = std::stof(widthStr);
            }
            else
            {
                throw std::runtime_error("XML element 'objectElement' has no 'width' attribute");
            }
            std::string heightStr = boundsElement->GetAttribute("height");
            if (!heightStr.empty())
            {
                bounds.Height = std::stof(heightStr);
            }
            else
            {
                throw std::runtime_error("XML element 'objectElement' has no 'height' attribute");
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
        throw std::runtime_error("XML element 'objectElement' has no 'name' attribute");
    }
    std::unique_ptr<soul::xml::xpath::NodeSet> attributeNodeSet = soul::xml::xpath::EvaluateToNodeSet("attribute", xmlElement);
    int nf = attributeNodeSet->Count();
    for (int i = 0; i < nf; ++i)
    {
        soul::xml::Node* node = attributeNodeSet->GetNode(i);
        if (node->IsElementNode())
        {
            soul::xml::Element* xmlElement = static_cast<soul::xml::Element*>(node);
            AttributeElement* attributeElement = new AttributeElement();
            attributeElement->SetContainerElement(this);
            attributeElement->Parse(xmlElement);
            attributes.Add(attributeElement);
        }
        else
        {
            throw std::runtime_error("XML element node expected in '" + xmlElement->Name() + "'");
        }
    }
}

void ObjectElement::Measure(wing::Graphics& graphics)
{
    ContainerElement::Measure(graphics);
    Layout* layout = Configuration::Instance().GetLayout();
    ObjectLayoutElement* objectLayout = layout->GetObjectLayoutElement();
    RelationshipLayoutElement* relationshipLayout = layout->GetRelationshipLayoutElement();
    PaddingElement* paddingElement = objectLayout->GetPaddingElement();
    wing::Font* nameFont = objectLayout->GetCaptionElement()->GetNameFontElement()->GetFont();
    wing::RectF r = wing::MeasureString(graphics, Name(), *nameFont, wing::PointF(0, 0), layout->GetStringFormat());
    wing::SizeF sz;
    r.GetSize(&sz);
    captionTextHeight = sz.Height;
    float captionRectX = Location().X;
    float captionRectY = Location().Y;
    float captionRectWidth = paddingElement->GetPadding().Horizontal() + sz.Width;
    float captionRectHeight = paddingElement->GetPadding().Vertical() + sz.Height;
    captionRect = wing::RectF(captionRectX, captionRectY, captionRectWidth, captionRectHeight);
    maxChildElementWidth = 0.0f;
    bool hasRelationship = false;
    MeasureAttributes(graphics, objectLayout, hasRelationship);
    if (hasRelationship)
    {
        float w = GetRelationshipSymbolFieldWidth(relationshipLayout->RelationshipSymbolRadius(), relationshipLayout->GetPaddingElement()->GetPadding().Horizontal());
        captionRect.Width += w;
        attributeRect.Width += w;
        SetRelationshipPoints();
    }
    SetSize();
}

void ObjectElement::MeasureAttributes(wing::Graphics& graphics, ObjectLayoutElement* objectLayout, bool& hasRelationship)
{
    PaddingElement* paddingElement = objectLayout->GetPaddingElement();
    attributeRect = wing::RectF();
    wing::PointF location = Location();
    attributeRect.X = location.X;
    attributeRect.Y = captionRect.Y + captionRect.Height;
    attributeRect.Width = captionRect.Width;
    attributeRect.Height = paddingElement->GetPadding().top;
    wing::PointF origin;
    origin.X = attributeRect.X + paddingElement->GetPadding().left;
    origin.Y = attributeRect.Y + paddingElement->GetPadding().top;
    int n = attributes.Count();
    for (int i = 0; i < n; ++i)
    {
        AttributeElement* attribute = attributes.Get(i);
        attribute->Measure(graphics);
        wing::SizeF attributeSize = attribute->Size();
        float w = attributeSize.Width + paddingElement->GetPadding().Horizontal();
        float h = attributeSize.Height;
        maxChildElementWidth = std::max(maxChildElementWidth, w);
        if (attribute->Relationship() != nullptr)
        {
            hasRelationship = true;
        }
        attribute->SetLocation(origin);
        attributeRect.Width = std::max(attributeRect.Width, w);
        origin.Y += h;
        attributeRect.Height += h;
    }
    attributeRect.Height += paddingElement->GetPadding().bottom;
}

void ObjectElement::SetRelationshipPoints()
{
    int nf = attributes.Count();
    for (int i = 0; i < nf; ++i)
    {
        AttributeElement* attribute = attributes.Get(i);
        attribute->SetRelationshipPoint();
    }
}

void ObjectElement::Draw(wing::Graphics& graphics)
{
    Layout* layout = Configuration::Instance().GetLayout();
    ObjectLayoutElement* objectLayout = layout->GetObjectLayoutElement();
    ContainerElement::Draw(graphics);
    DrawFrame(graphics, objectLayout);
    DrawCaption(graphics, objectLayout);
    DrawAttributes(graphics, objectLayout);
}

void ObjectElement::DrawFrame(wing::Graphics& graphics, ObjectLayoutElement* objectLayout)
{
    PaddingElement* paddingElement = objectLayout->GetPaddingElement();
    wing::Pen* framePen = objectLayout->FramePen();
    float frameWidth = objectLayout->FrameWidth();
    float roundingRadius = objectLayout->FrameRoundingRadius();
    wing::PointF loc = Location();
    wing::SizeF size = Size();
    wing::PointF topStart(loc.X + roundingRadius - 1.0f - frameWidth, loc.Y);
    wing::PointF topEnd(loc.X + size.Width - roundingRadius + 1.0f + frameWidth, loc.Y);
    graphics.DrawLine(framePen, topStart, topEnd);
    wing::RectF topLeftRoundingRect(wing::PointF(loc.X, loc.Y), wing::SizeF(roundingRadius, roundingRadius));
    float topLeftRoundingStartAngle = 180.0f;
    float topLeftRoudingSweepAngle = 90.0f;
    graphics.DrawArc(framePen, topLeftRoundingRect, topLeftRoundingStartAngle, topLeftRoudingSweepAngle);
    wing::PointF rightStart(loc.X + size.Width, loc.Y + roundingRadius - 1.0f - 2.0f * frameWidth);
    wing::PointF rightEnd(loc.X + size.Width, loc.Y + size.Height - roundingRadius + 1.0f + 2.0f * frameWidth);
    graphics.DrawLine(framePen, rightStart, rightEnd);
    wing::RectF topRightRoundingRect(wing::PointF(loc.X + size.Width - roundingRadius, loc.Y), wing::SizeF(roundingRadius, roundingRadius));
    float topRightRoundingStartAngle = 0.0f;
    float topRightRoudingSweepAngle = -90.0f;
    graphics.DrawArc(framePen, topRightRoundingRect, topRightRoundingStartAngle, topRightRoudingSweepAngle);
    wing::RectF rightBottomRoundingRect(wing::PointF(loc.X + size.Width - roundingRadius, loc.Y + size.Height - roundingRadius), wing::SizeF(roundingRadius, roundingRadius));
    float rightBottomRoundingStartAngle = 0.0f;
    float rightBottomRoudingSweepAngle = 90.0f;
    graphics.DrawArc(framePen, rightBottomRoundingRect, rightBottomRoundingStartAngle, rightBottomRoudingSweepAngle);
    wing::PointF bottomStart(loc.X + roundingRadius - 1.0f - 2.0f * frameWidth, loc.Y + size.Height);
    wing::PointF bottomEnd(loc.X + size.Width - roundingRadius + 1.0f + 2.0f * frameWidth, loc.Y + size.Height);
    graphics.DrawLine(framePen, bottomStart, bottomEnd);
    wing::RectF leftBottomRoundingRect(wing::PointF(loc.X, loc.Y + size.Height - roundingRadius), wing::SizeF(roundingRadius, roundingRadius));
    float leftBottomRoundingStartAngle = 180.0f;
    float leftBottomRoudingSweepAngle = -90.0f;
    graphics.DrawArc(framePen, leftBottomRoundingRect, leftBottomRoundingStartAngle, leftBottomRoudingSweepAngle);
    wing::PointF leftStart(loc.X, loc.Y + roundingRadius - 1.0f - frameWidth);
    wing::PointF leftEnd(loc.X, loc.Y + size.Height - roundingRadius + 1.0f + frameWidth);
    graphics.DrawLine(framePen, leftStart, leftEnd);
    if (!attributes.IsEmpty())
    {
        float captionY = paddingElement->GetPadding().Vertical() + captionTextHeight;
        wing::PointF location = Location();
        wing::PointF captionLineStart(location.X, location.Y + captionY);
        wing::PointF captionLineEnd(location.X + Size().Width, location.Y + captionY);
        graphics.DrawLine(framePen, captionLineStart, captionLineEnd);
    }
}

void ObjectElement::DrawCaption(wing::Graphics& graphics, ObjectLayoutElement* objectLayout)
{
    PaddingElement* paddingElement = objectLayout->GetPaddingElement();
    CaptionElement* captionElement = objectLayout->GetCaptionElement();
    FontElement* nameFontElement = captionElement->GetNameFontElement();
    wing::PointF location = Location();
    wing::PointF origin(location.X + paddingElement->GetPadding().left, location.Y + paddingElement->GetPadding().top);
    wing::Font* nameFont = nameFontElement->GetFont();
    wing::Brush* brush = objectLayout->GetTextColorElement()->GetBrush();
    wing::DrawString(graphics, Name(), *nameFont, origin, *brush);
}

void ObjectElement::DrawAttributes(wing::Graphics& graphics, ObjectLayoutElement* objectLayout)
{
    int n = attributes.Count();
    for (int i = 0; i < n; ++i)
    {
        AttributeElement* attribute = attributes.Get(i);
        attribute->Draw(graphics);
    }
}

void ObjectElement::AddActions(Diagram* diagram, int elementIndex, wing::ContextMenu* contextMenu) const
{
    ContainerElement::AddActions(diagram, elementIndex, contextMenu);
    wing::MenuItem* propertiesMenuItem = new wing::MenuItem("Properties...");
    contextMenu->AddMenuItem(propertiesMenuItem);
    contextMenu->AddAction(new ObjectElementPropertiesAction(diagram, elementIndex, propertiesMenuItem));
}

DiagramElement* ObjectElement::Clone() const
{
    ObjectElement* clone = new ObjectElement();
    clone->SetName(Name());
    clone->SetBounds(Bounds());
    int nf = attributes.Count();
    for (int i = 0; i < nf; ++i)
    {
        AttributeElement* attributeElement = attributes.Get(i);
        AttributeElement* clonedAttribute = static_cast<AttributeElement*>(attributeElement->Clone());
        clonedAttribute->SetContainerElement(clone);
        clone->attributes.Add(clonedAttribute);
    }
    return clone;
}

void ObjectElement::SetAttributes(IndexList<AttributeElement>&& attributes_)
{
    attributes = std::move(attributes_);
    for (auto& attribute : attributes)
    {
        attribute->SetContainerElement(this);
    }
}

AttributeElement* ObjectElement::GetAttribute(int attributeIndex) const
{
    return attributes.Get(attributeIndex);
}

int ObjectElement::GetIndexOfAttributeElement(AttributeElement* attributeElement) const
{
    int n = attributes.Count();
    for (int i = 0; i < n; ++i)
    {
        if (attributes.Get(i) == attributeElement) return i;
    }
    throw std::runtime_error("attribute '" + attributeElement->Name() + "' not found from object '" + Name() + "'");
}

std::vector<RelationshipElement*> ObjectElement::GetAllRelationships() const
{
    std::vector<RelationshipElement*> relationships = Relationships();
    int nf = attributes.Count();
    for (int i = 0; i < nf; ++i)
    {
        AttributeElement* attribute = attributes.Get(i);
        RelationshipElement* relationship = attribute->Relationship();
        if (relationship != nullptr)
        {
            relationships.push_back(relationship);
        }
    }
    return relationships;
}

void ObjectElement::MapChildObjects(ContainerElement* from, std::map<DiagramElement*, DiagramElement*>& cloneMap, std::map<DiagramElement*, DiagramElement*>& reverseCloneMap)
{
    if (from->IsObjectElement())
    {
        ObjectElement* fromObject = static_cast<ObjectElement*>(from);
        int nf = fromObject->attributes.Count();
        for (int i = 0; i < nf; ++i)
        {
            AttributeElement* newAttr = attributes.Get(i);
            AttributeElement* oldAttr = fromObject->attributes.Get(i);
            cloneMap[oldAttr] = newAttr;
            reverseCloneMap[newAttr] = oldAttr;
        }
    }
}

std::vector<EndPoint> ObjectElement::GetEndPoints(EndPointKind endPointKind, Tool* tool) const
{
    if (endPointKind == EndPointKind::source && tool->IsReferenceTool())
    {
        std::vector<EndPoint> endPoints;
        int nf = attributes.Count();
        for (int i = 0; i < nf; ++i)
        {
            AttributeElement* attribute = attributes.Get(i);
            endPoints.push_back(attribute->GetEndPoint(Connector(Connector::Attribute(i))));
        }
        return endPoints;
    }
    else
    {
        return ContainerElement::GetEndPoints(endPointKind, tool);
    }
}

float ObjectElement::GetMaxChildElementWidth() const
{
    return maxChildElementWidth;
}

void ObjectElement::SetSize()
{
    wing::SizeF size;
    size.Width = std::max(size.Width, captionRect.Width);
    float h = captionRect.Height;
    if (!attributes.IsEmpty())
    {
        size.Width = std::max(size.Width, attributeRect.Width);
        h = h + attributeRect.Height;
    }
    size.Height = std::max(size.Height, h);
    ContainerElement::SetSize(size);
}

} // namespace springpp
