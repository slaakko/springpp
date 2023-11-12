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

FieldElement* ContainerElement::GetField(int fieldIndex) const
{
    throw std::runtime_error("container element '" + Name() + "' has no fields");
}

int ContainerElement::GetIndexOfFieldElement(FieldElement* fieldElement) const
{
    throw std::runtime_error("container element '" + Name() + "' has no fields");
}

OperationElement* ContainerElement::GetOperation(int operationIndex) const
{
    throw std::runtime_error("container element '" + Name() + "' has no operations");
}

int ContainerElement::GetIndexOfOperationElement(OperationElement* operationElement) const
{
    throw std::runtime_error("container element '" + Name() + "' has no operations");
}

EndPoint ContainerElement::GetEndPoint(const Snap& snap) const
{
    Layout* layout = Configuration::Instance().GetLayout();
    RelationshipLayoutElement* relationshipLayoutElement = layout->GetRelationshipLayoutElement();
    float w = relationshipLayoutElement->LineArrowWidth();
    wing::PointF loc = Location();
    wing::SizeF size = Size();
    switch (snap.ToInt())
    {
        case Snap::TopLeft():
        {
            return EndPoint(const_cast<ContainerElement*>(this), Snap(Snap::TopLeft()), wing::PointF(loc.X + w, loc.Y));
        }
        case Snap::TopCenter():
        {
            return EndPoint(const_cast<ContainerElement*>(this), Snap(Snap::TopCenter()), wing::PointF(loc.X + size.Width / 2, loc.Y));
        }
        case Snap::TopRight():
        {
            return EndPoint(const_cast<ContainerElement*>(this), Snap(Snap::TopRight()), wing::PointF(loc.X + size.Width - w, loc.Y));
        }
        case Snap::RightTop():
        {
            return EndPoint(const_cast<ContainerElement*>(this), Snap(Snap::RightTop()), wing::PointF(loc.X + size.Width, loc.Y + w));
        }
        case Snap::RightCenter():
        {
            return EndPoint(const_cast<ContainerElement*>(this), Snap(Snap::RightCenter()), wing::PointF(loc.X + size.Width, loc.Y + size.Height / 2));
        }
        case Snap::RightBottom():
        {
            return EndPoint(const_cast<ContainerElement*>(this), Snap(Snap::RightBottom()), wing::PointF(loc.X + size.Width, loc.Y + size.Height - w));
        }
        case Snap::BottomRight():
        {
            return EndPoint(const_cast<ContainerElement*>(this), Snap(Snap::BottomRight()), wing::PointF(loc.X + size.Width - w, loc.Y + size.Height));
        }
        case Snap::BottomCenter():
        {
            return EndPoint(const_cast<ContainerElement*>(this), Snap(Snap::BottomCenter()), wing::PointF(loc.X + size.Width / 2, loc.Y + size.Height));
        }
        case Snap::BottomLeft():
        {
            return EndPoint(const_cast<ContainerElement*>(this), Snap(Snap::BottomLeft()), wing::PointF(loc.X + w, loc.Y + size.Height));
        }
        case Snap::LeftBottom():
        {
            return EndPoint(const_cast<ContainerElement*>(this), Snap(Snap::LeftBottom()), wing::PointF(loc.X, loc.Y + size.Height - w));
        }
        case Snap::LeftCenter():
        {
            return EndPoint(const_cast<ContainerElement*>(this), Snap(Snap::LeftCenter()), wing::PointF(loc.X, loc.Y + size.Height / 2));
        }
        case Snap::LeftTop():
        {
            return EndPoint(const_cast<ContainerElement*>(this), Snap(Snap::LeftTop()), wing::PointF(loc.X, loc.Y + w));
        }
    }
    return EndPoint();
}

std::vector<EndPoint> ContainerElement::GetEndPoints(EndPointKind endPointKind, Tool* tool) const
{
    std::vector<EndPoint> endPoints;
    endPoints.push_back(GetEndPoint(Snap(Snap::TopLeft())));
    endPoints.push_back(GetEndPoint(Snap(Snap::TopCenter())));
    endPoints.push_back(GetEndPoint(Snap(Snap::TopRight())));
    endPoints.push_back(GetEndPoint(Snap(Snap::RightTop())));
    endPoints.push_back(GetEndPoint(Snap(Snap::RightCenter())));
    endPoints.push_back(GetEndPoint(Snap(Snap::RightBottom())));
    endPoints.push_back(GetEndPoint(Snap(Snap::BottomRight())));
    endPoints.push_back(GetEndPoint(Snap(Snap::BottomCenter())));
    endPoints.push_back(GetEndPoint(Snap(Snap::BottomLeft())));
    endPoints.push_back(GetEndPoint(Snap(Snap::LeftBottom())));
    endPoints.push_back(GetEndPoint(Snap(Snap::LeftCenter())));
    endPoints.push_back(GetEndPoint(Snap(Snap::LeftTop())));
    return endPoints;
}

void ContainerElement::Measure(wing::Graphics& graphics)
{
    Layout* layout = Configuration::Instance().GetLayout();
    RelationshipLayoutElement* relationshipLayoutElement = layout->GetRelationshipLayoutElement();
    float w = relationshipLayoutElement->LineWidth();
    MeasureRelationships(Snap(Snap::TopLeft()), w);
    MeasureRelationships(Snap(Snap::TopCenter()), w);
    MeasureRelationships(Snap(Snap::TopRight()), w);
    MeasureRelationships(Snap(Snap::BottomLeft()), w);
    MeasureRelationships(Snap(Snap::BottomCenter()), w);
    MeasureRelationships(Snap(Snap::BottomRight()), w);
    MeasureRelationships(Snap(Snap::LeftTop()), w);
    MeasureRelationships(Snap(Snap::LeftCenter()), w);
    MeasureRelationships(Snap(Snap::LeftBottom()), w);
    MeasureRelationships(Snap(Snap::RightTop()), w);
    MeasureRelationships(Snap(Snap::RightCenter()), w);
    MeasureRelationships(Snap(Snap::RightBottom()), w);
}

void ContainerElement::MeasureRelationships(const Snap& snap, float w)
{
    std::vector<RelationshipElement*> pickedRelationships = PickRelationships(snap);
    int n = pickedRelationships.size();
    for (int i = 0; i < n; ++i)
    {
        RelationshipElement* relationship = pickedRelationships[i];
        relationship->Calculate(snap, this, w, i, n);
    }
}

std::vector<RelationshipElement*> ContainerElement::PickRelationships(const Snap& snap) const
{
    std::vector<RelationshipElement*> pickedRelationships;
    for (auto relationship : relationships)
    {
        if (relationship->Source().Element() == this)
        {
            if (relationship->Source().GetSnap() == snap)
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
                if (sourceEndPoint.GetSnap() == snap)
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
            if (relationship->Target().GetSnap() == snap)
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
