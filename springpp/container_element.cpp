// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

module springpp.container_element;

import springpp.relationship_element;
import springpp.layout;
import springpp.configuration;

namespace springpp {

ContainerElement::ContainerElement(DiagramElementKind kind_) : DiagramElement(kind_)
{
}

ContainerElement::ContainerElement(DiagramElementKind kind_, const std::string& name_) : DiagramElement(kind_)
{
    SetName(name_);
}

ContainerElement::~ContainerElement()
{
    for (RelationshipElement* relationship : relationships)
    {
        relationship->RemoveContainer(this);
    }
}

void ContainerElement::AddRelationship(RelationshipElement* relationship_)
{
    if (std::find(relationships.begin(), relationships.end(), relationship_) == relationships.end())
    {
        relationships.push_back(relationship_);
    }
}

void ContainerElement::RemoveRelationship(RelationshipElement* relationship_)
{
    relationships.erase(std::remove(relationships.begin(), relationships.end(), relationship_), relationships.end());
}

AttributeElement* ContainerElement::GetAttribute(int attributeIndex) const
{
    throw std::runtime_error("container element '" + Name() + "' has no attributes");
}

int ContainerElement::GetIndexOfAttributeElement(AttributeElement* attributeElement) const
{
    throw std::runtime_error("container element '" + Name() + "' has no attributes");
}

OperationElement* ContainerElement::GetOperation(int operationIndex) const
{
    throw std::runtime_error("container element '" + Name() + "' has no operations");
}

int ContainerElement::GetIndexOfOperationElement(OperationElement* operationElement) const
{
    throw std::runtime_error("container element '" + Name() + "' has no operations");
}

EndPoint ContainerElement::GetEndPoint(const Connector& connector) const
{
    Layout* layout = Configuration::Instance().GetLayout();
    RelationshipLayoutElement* relationshipLayoutElement = layout->GetRelationshipLayoutElement();
    float w = relationshipLayoutElement->LineArrowWidth();
    wing::PointF loc = Location();
    wing::SizeF size = Size();
    switch (connector.ToInt())
    {
        case Connector::TopLeft():
        {
            return EndPoint(const_cast<ContainerElement*>(this), Connector(Connector::TopLeft()), wing::PointF(loc.X + w, loc.Y));
        }
        case Connector::TopCenter():
        {
            return EndPoint(const_cast<ContainerElement*>(this), Connector(Connector::TopCenter()), wing::PointF(loc.X + size.Width / 2, loc.Y));
        }
        case Connector::TopRight():
        {
            return EndPoint(const_cast<ContainerElement*>(this), Connector(Connector::TopRight()), wing::PointF(loc.X + size.Width - w, loc.Y));
        }
        case Connector::RightTop():
        {
            return EndPoint(const_cast<ContainerElement*>(this), Connector(Connector::RightTop()), wing::PointF(loc.X + size.Width, loc.Y + w));
        }
        case Connector::RightCenter():
        {
            return EndPoint(const_cast<ContainerElement*>(this), Connector(Connector::RightCenter()), wing::PointF(loc.X + size.Width, loc.Y + size.Height / 2));
        }
        case Connector::RightBottom():
        {
            return EndPoint(const_cast<ContainerElement*>(this), Connector(Connector::RightBottom()), wing::PointF(loc.X + size.Width, loc.Y + size.Height - w));
        }
        case Connector::BottomRight():
        {
            return EndPoint(const_cast<ContainerElement*>(this), Connector(Connector::BottomRight()), wing::PointF(loc.X + size.Width - w, loc.Y + size.Height));
        }
        case Connector::BottomCenter():
        {
            return EndPoint(const_cast<ContainerElement*>(this), Connector(Connector::BottomCenter()), wing::PointF(loc.X + size.Width / 2, loc.Y + size.Height));
        }
        case Connector::BottomLeft():
        {
            return EndPoint(const_cast<ContainerElement*>(this), Connector(Connector::BottomLeft()), wing::PointF(loc.X + w, loc.Y + size.Height));
        }
        case Connector::LeftBottom():
        {
            return EndPoint(const_cast<ContainerElement*>(this), Connector(Connector::LeftBottom()), wing::PointF(loc.X, loc.Y + size.Height - w));
        }
        case Connector::LeftCenter():
        {
            return EndPoint(const_cast<ContainerElement*>(this), Connector(Connector::LeftCenter()), wing::PointF(loc.X, loc.Y + size.Height / 2));
        }
        case Connector::LeftTop():
        {
            return EndPoint(const_cast<ContainerElement*>(this), Connector(Connector::LeftTop()), wing::PointF(loc.X, loc.Y + w));
        }
    }
    return EndPoint();
}

std::vector<EndPoint> ContainerElement::GetEndPoints(EndPointKind endPointKind, Tool* tool) const
{
    std::vector<EndPoint> endPoints;
    endPoints.push_back(GetEndPoint(Connector(Connector::TopLeft())));
    endPoints.push_back(GetEndPoint(Connector(Connector::TopCenter())));
    endPoints.push_back(GetEndPoint(Connector(Connector::TopRight())));
    endPoints.push_back(GetEndPoint(Connector(Connector::RightTop())));
    endPoints.push_back(GetEndPoint(Connector(Connector::RightCenter())));
    endPoints.push_back(GetEndPoint(Connector(Connector::RightBottom())));
    endPoints.push_back(GetEndPoint(Connector(Connector::BottomRight())));
    endPoints.push_back(GetEndPoint(Connector(Connector::BottomCenter())));
    endPoints.push_back(GetEndPoint(Connector(Connector::BottomLeft())));
    endPoints.push_back(GetEndPoint(Connector(Connector::LeftBottom())));
    endPoints.push_back(GetEndPoint(Connector(Connector::LeftCenter())));
    endPoints.push_back(GetEndPoint(Connector(Connector::LeftTop())));
    return endPoints;
}

void ContainerElement::Measure(wing::Graphics& graphics)
{
    Layout* layout = Configuration::Instance().GetLayout();
    RelationshipLayoutElement* relationshipLayoutElement = layout->GetRelationshipLayoutElement();
    float w = relationshipLayoutElement->LineWidth();
    MeasureRelationships(Connector(Connector::TopLeft()), w);
    MeasureRelationships(Connector(Connector::TopCenter()), w);
    MeasureRelationships(Connector(Connector::TopRight()), w);
    MeasureRelationships(Connector(Connector::BottomLeft()), w);
    MeasureRelationships(Connector(Connector::BottomCenter()), w);
    MeasureRelationships(Connector(Connector::BottomRight()), w);
    MeasureRelationships(Connector(Connector::LeftTop()), w);
    MeasureRelationships(Connector(Connector::LeftCenter()), w);
    MeasureRelationships(Connector(Connector::LeftBottom()), w);
    MeasureRelationships(Connector(Connector::RightTop()), w);
    MeasureRelationships(Connector(Connector::RightCenter()), w);
    MeasureRelationships(Connector(Connector::RightBottom()), w);
}

void ContainerElement::MeasureRelationships(const Connector& connector, float w)
{
    std::vector<RelationshipElement*> pickedRelationships = PickRelationships(connector);
    int n = pickedRelationships.size();
    for (int i = 0; i < n; ++i)
    {
        RelationshipElement* relationship = pickedRelationships[i];
        relationship->Calculate(connector, this, w, i, n);
    }
}

std::vector<RelationshipElement*> ContainerElement::PickRelationships(const Connector& connector) const
{
    std::vector<RelationshipElement*> pickedRelationships;
    for (auto relationship : relationships)
    {
        if (relationship->Source().Element() == this)
        {
            if (relationship->Source().GetConnector() == connector)
            {
                if (std::find(pickedRelationships.begin(), pickedRelationships.end(), relationship) == pickedRelationships.end())
                {
                    pickedRelationships.push_back(relationship);
                }
            }
        }
        for (const EndPoint& sourceEndPoint : relationship->SourceEndPoints())
        {
            if (sourceEndPoint.Element() == this)
            {
                if (sourceEndPoint.GetConnector() == connector)
                {
                    if (std::find(pickedRelationships.begin(), pickedRelationships.end(), relationship) == pickedRelationships.end())
                    {
                        pickedRelationships.push_back(relationship);
                    }
                }
            }
        }
        if (relationship->Target().Element() == this)
        {
            if (relationship->Target().GetConnector() == connector)
            {
                if (std::find(pickedRelationships.begin(), pickedRelationships.end(), relationship) == pickedRelationships.end())
                {
                    pickedRelationships.push_back(relationship);
                }
            }
        }
    }
    return pickedRelationships;
}

} // namespace springpp
