// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

module;
#include <Windows.h>
#include <gdiplus.h>
#undef min
#undef max

module springpp.relationship_element;

import springpp.container_element;
import springpp.canvas;
import springpp.configuration;
import springpp.diagram_util;
import springpp.action;
import springpp.relationships;
import soul.xml.xpath;

namespace springpp {

std::string CardinalityStr(Cardinality cardinality)
{
    switch (cardinality)
    {
        case Cardinality::zero:
        {
            return "zero";
        }
        case Cardinality::one:
        {
            return "one";
        }
        case Cardinality::many:
        {
            return "many";
        }
    }
    return std::string();
}

Cardinality ParseCardinalityStr(const std::string& cardinalityStr)
{
    if (cardinalityStr == "zero")
    {
        return Cardinality::zero;
    }
    else if (cardinalityStr == "one")
    {
        return Cardinality::one;
    }
    else if (cardinalityStr == "many")
    {
        return Cardinality::many;
    }
    else
    {
        return Cardinality::zero;
    }
}

struct DecreasingOrderBySecond
{
    bool operator()(const std::pair<int, int>& left, const std::pair<int, int>& right) const
    {
        return left.second > right.second;
    }
};

RelationshipElementRep::RelationshipElementRep(RelationshipElement* relationshipElement_) : relationshipElement(relationshipElement_)
{
}

RelationshipElementRep::~RelationshipElementRep()
{
}

Line RelationshipElementRep::GetSourceTextLine(const Line& firstLine, float& leadingWidth, float padding) const
{
    RelationshipElement* relationshipElement = GetRelationshipElement();
    Line textLine = firstLine;
    if (IsHorizontalLine(firstLine.start, firstLine.end, textLine))
    {
        if (textLine != firstLine)
        {
            leadingWidth = padding;
        }
        return textLine;
    }
    if (!relationshipElement->IntermediatePoints().empty())
    {
        wing::PointF prev = firstLine.end;
        for (int i = 1; i < relationshipElement->IntermediatePoints().size(); ++i)
        {
            wing::PointF next = relationshipElement->IntermediatePoints()[i];
            if (IsHorizontalLine(prev, next, textLine))
            {
                leadingWidth = padding;
                return textLine;
            }
            prev = next;
        }
        if (IsHorizontalLine(prev, relationshipElement->Target().Point(), textLine))
        {
            leadingWidth = padding;
            return textLine;
        }
    }
    return textLine;
}

void RelationshipElementRep::DrawSourceText(wing::Graphics& graphics, wing::Font* font, wing::Brush* textBrush, const Line& line, float leadingWidth)
{
    RelationshipElement* relationshipElement = GetRelationshipElement();
    wing::GraphicsState state = graphics.Save();
    float dx = line.end.X - line.start.X;
    float dy = line.end.Y - line.start.Y;
    double epsilon = std::numeric_limits<double>::epsilon();
    if (std::abs(dx) >= 1.0f)
    {
        float angle = static_cast<float>(180.0 * std::atan2(dy, dx) / std::numbers::pi_v<double>);
        graphics.RotateTransform(angle, Gdiplus::MatrixOrder::MatrixOrderAppend);
    }
    else
    {
        float angle = 0;
        if (dy >= 0)
        {
            angle = 90.0f;
        }
        else
        {
            angle = -90.0f;
        }
        graphics.RotateTransform(angle, Gdiplus::MatrixOrder::MatrixOrderAppend);
    }
    graphics.TranslateTransform(line.start.X + leadingWidth, line.start.Y - relationshipElement->SourceTextSize().Height, Gdiplus::MatrixOrder::MatrixOrderAppend);
    DrawString(graphics, relationshipElement->Source().Text(), *font, wing::PointF(0, 0), *textBrush);
    graphics.Restore(state);
}

void RelationshipElementRep::DrawTargetText(wing::Graphics& graphics, wing::Font* font, wing::Brush* textBrush, const Line& line, float symbolWidth)
{
    RelationshipElement* relationshipElement = GetRelationshipElement();
    wing::GraphicsState state = graphics.Save();
    float dx = line.end.X - line.start.X;
    float dy = line.end.Y - line.start.Y;
    if (std::abs(dx) >= 1.0f)
    {
        float angle = static_cast<float>(180.0 * std::atan2(dy, dx) / std::numbers::pi_v<double>);
        graphics.RotateTransform(angle, Gdiplus::MatrixOrder::MatrixOrderAppend);
    }
    else
    {
        float angle = 0;
        if (dy >= 0)
        {
            angle = 90.0f;
        }
        else
        {
            angle = -90.0f;
        }
        graphics.RotateTransform(angle, Gdiplus::MatrixOrder::MatrixOrderAppend);
    }
    graphics.TranslateTransform(line.end.X - symbolWidth - relationshipElement->TargetTextSize().Width, line.start.Y - relationshipElement->TargetTextSize().Height, 
        Gdiplus::MatrixOrder::MatrixOrderAppend);
    DrawString(graphics, relationshipElement->Target().Text(), *font, wing::PointF(0, 0), *textBrush);
    graphics.Restore(state);
}

void RelationshipElementRep::DrawSelected(wing::Graphics& graphics)
{
    RelationshipElement* relationshipElement = GetRelationshipElement();
    Layout* layout = Configuration::Instance().GetLayout();
    RelationshipLayoutElement* relationshipLayoutElement = layout->GetRelationshipLayoutElement();
    wing::Pen* selectedLinePen = relationshipLayoutElement->SelectedLinePen();
    wing::PointF prev = relationshipElement->Source().Point();
    for (const auto& next : relationshipElement->IntermediatePoints())
    {
        graphics.DrawLine(selectedLinePen, prev, next);
        prev = next;
    }
    graphics.DrawLine(selectedLinePen, prev, relationshipElement->Target().Point());
}

RelationshipElement::RelationshipElement() : 
    DiagramElement(DiagramElementKind::relationshipElement), rkind(RelationshipKind::none), cardinality(Cardinality::one), sourceTextSize(), targetTextSize()
{
    SetRep();
}

RelationshipElement::RelationshipElement(RelationshipKind rkind_) : 
    DiagramElement(DiagramElementKind::relationshipElement), rkind(rkind_), cardinality(Cardinality::one), sourceTextSize(), targetTextSize()
{
    SetRep();
}

bool RelationshipElementRep::Contains(const wing::PointF& location) const
{
    RelationshipElement* relationshipElement = GetRelationshipElement();
    Layout* layout = Configuration::Instance().GetLayout();
    Diagram* diagram = GetDiagram();
    RelationshipLayoutElement* relationshipLayoutElement = layout->GetRelationshipLayoutElement();
    float selectedLineWidth = relationshipLayoutElement->SelectedLineWidth();
    wing::PointF from = relationshipElement->Source().Point();
    for (wing::PointF to : relationshipElement->IntermediatePoints())
    {
        if (springpp::Contains(from, to, location, selectedLineWidth))
        {
            return true;
        }
        from = to;
    }
    wing::PointF to = relationshipElement->Target().Point();
    for (const auto& sourceEndPoint : relationshipElement->SourceEndPoints())
    {
        wing::PointF from = sourceEndPoint.Point();
        if (springpp::Contains(from, to, location, selectedLineWidth))
        {
            return true;
        }
    }
    if (springpp::Contains(from, to, location, selectedLineWidth))
    {
        return true;
    }
    return false;
}

soul::xml::Element* RelationshipElement::ToXml() const
{
    soul::xml::Element* xmlElement = soul::xml::MakeElement("relationshipElement");
    soul::xml::Element* boundingRectElement = soul::xml::MakeElement("boundingRect");
    wing::RectF boundingRect = BoundingRect();
    boundingRectElement->SetAttribute("x", std::to_string(boundingRect.X));
    boundingRectElement->SetAttribute("y", std::to_string(boundingRect.Y));
    boundingRectElement->SetAttribute("width", std::to_string(boundingRect.Width));
    boundingRectElement->SetAttribute("height", std::to_string(boundingRect.Height));
    xmlElement->AppendChild(boundingRectElement);
    xmlElement->SetAttribute("rkind", RelationshipKindStr(rkind));
    xmlElement->SetAttribute("cardinality", CardinalityStr(cardinality));
    xmlElement->AppendChild(source.ToXml("source"));
    for (const auto& sourceEndPoint : sourceEndPoints)
    {
        xmlElement->AppendChild(sourceEndPoint.ToXml("sourceEndPoint"));
    }
    for (const auto& point : intermediatePoints)
    {
        soul::xml::Element* intermediatePointElement = soul::xml::MakeElement("intermediatePoint");
        intermediatePointElement->SetAttribute("x", std::to_string(point.X));
        intermediatePointElement->SetAttribute("y", std::to_string(point.Y));
        xmlElement->AppendChild(intermediatePointElement);
    }
    xmlElement->AppendChild(target.ToXml("target"));
    return xmlElement;
}

void RelationshipElement::Parse(soul::xml::Element* xmlElement)
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
                throw std::runtime_error("XML element 'relationshipElement' has no 'x' attribute");
            }
            std::string yStr = boundingRectElement->GetAttribute("y");
            if (!yStr.empty())
            {
                boundingRect.Y = std::stof(yStr);
            }
            else
            {
                throw std::runtime_error("XML element 'relationshipElement' has no 'y' attribute");
            }
            std::string widthStr = boundingRectElement->GetAttribute("width");
            if (!widthStr.empty())
            {
                boundingRect.Width = std::stof(widthStr);
            }
            else
            {
                throw std::runtime_error("XML element 'relationshipElement' has no 'width' attribute");
            }
            std::string heightStr = boundingRectElement->GetAttribute("height");
            if (!heightStr.empty())
            {
                boundingRect.Height = std::stof(heightStr);
            }
            else
            {
                throw std::runtime_error("XML element 'relationshipElement' has no 'height' attribute");
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
    rkind = ParseRelationshipKindStr(xmlElement->GetAttribute("rkind"));
    SetRep();
    cardinality = ParseCardinalityStr(xmlElement->GetAttribute("cardinality"));
    std::unique_ptr<soul::xml::xpath::NodeSet> sourceNodeSet = soul::xml::xpath::EvaluateToNodeSet("source", xmlElement);
    if (sourceNodeSet->Count() == 1)
    {
        soul::xml::Node* node = sourceNodeSet->GetNode(0);
        if (node->IsElementNode())
        {
            soul::xml::Element* xmlElement = static_cast<soul::xml::Element*>(node);
            source.Parse(xmlElement);
        }
        else
        {
            throw std::runtime_error("XML element node expected in '" + xmlElement->Name() + "'");
        }
    }
    else
    {
        throw std::runtime_error("XML element 'source' not unique in '" + xmlElement->Name() + "'");
    }
    std::unique_ptr<soul::xml::xpath::NodeSet> targetNodeSet = soul::xml::xpath::EvaluateToNodeSet("target", xmlElement);
    if (targetNodeSet->Count() == 1)
    {
        soul::xml::Node* node = targetNodeSet->GetNode(0);
        if (node->IsElementNode())
        {
            soul::xml::Element* xmlElement = static_cast<soul::xml::Element*>(node);
            target.Parse(xmlElement);
        }
        else
        {
            throw std::runtime_error("XML element node expected in '" + xmlElement->Name() + "'");
        }
    }
    else
    {
        throw std::runtime_error("XML element 'target' not unique in '" + xmlElement->Name() + "'");
    }
    std::unique_ptr<soul::xml::xpath::NodeSet> sourceEndPointNodeSet = soul::xml::xpath::EvaluateToNodeSet("sourceEndPoint", xmlElement);
    int nep = sourceEndPointNodeSet->Count();
    for (int i = 0; i < nep; ++i)
    {
        soul::xml::Node* node = sourceEndPointNodeSet->GetNode(i);
        if (node->IsElementNode())
        {
            soul::xml::Element* xmlElement = static_cast<soul::xml::Element*>(node);
            EndPoint endPoint;
            endPoint.Parse(xmlElement);
            sourceEndPoints.push_back(endPoint);
        }
        else
        {
            throw std::runtime_error("XML element node expected in '" + xmlElement->Name() + "'");
        }
    }
    std::unique_ptr<soul::xml::xpath::NodeSet> intermediatePointNodeSet = soul::xml::xpath::EvaluateToNodeSet("intermediatePoint", xmlElement);
    int nip = intermediatePointNodeSet->Count();
    for (int i = 0; i < nip; ++i)
    {
        soul::xml::Node* node = intermediatePointNodeSet->GetNode(i);
        if (node->IsElementNode())
        {
            soul::xml::Element* xmlElement = static_cast<soul::xml::Element*>(node);
            wing::PointF point;
            point.X = std::stof(xmlElement->GetAttribute("x"));
            point.Y = std::stof(xmlElement->GetAttribute("y"));
            intermediatePoints.push_back(point);
        }
        else
        {
            throw std::runtime_error("XML element node expected in '" + xmlElement->Name() + "'");
        }
    }
}

void RelationshipElement::SetRKind(RelationshipKind rkind_)
{
    rkind = rkind_;
    SetRep();
}

void RelationshipElement::SetRep()
{
    switch (rkind)
    {
        case RelationshipKind::inheritance:
        {
            rep.reset(new Inheritance(this));
            break;
        }
        case RelationshipKind::combinedInheritance:
        {
            rep.reset(new CombinedInheritance(this));
            break;
        }
        case RelationshipKind::composition:
        {
            rep.reset(new Composition(this));
            break;
        }
        case RelationshipKind::aggregation:
        {
            rep.reset(new Aggregation(this));
            break;
        }
        case RelationshipKind::reference:
        {
            rep.reset(new Reference(this));
            break;
        }
        case RelationshipKind::createInstance:
        {
            rep.reset(new CreateInstance(this));
            break;
        }
    }
}

DiagramElement* RelationshipElement::Clone() const
{
    RelationshipElement* clone = new RelationshipElement(rkind);
    clone->SetName(Name());
    clone->SetBoundingRect(BoundingRect());
    clone->cardinality = cardinality;
    clone->source = source;
    clone->target = target;
    clone->sourceEndPoints = sourceEndPoints;
    clone->intermediatePoints = intermediatePoints;
    clone->sourceTextSize = sourceTextSize;
    clone->targetTextSize = targetTextSize;
    clone->rep.reset(rep->Clone(clone));
    return clone;
}

wing::PointF RelationshipElement::LastPoint() const
{
    if (intermediatePoints.empty())
    {
        return source.Point();
    }
    else
    {
        return intermediatePoints.back();
    }
}

void RelationshipElement::SetLastPoint(const wing::PointF& lastPoint)
{
    if (intermediatePoints.empty())
    {
        source.Point() = lastPoint;
    }
    else
    {
        intermediatePoints.back() = lastPoint;
    }
}

wing::RectF RelationshipElement::BoundingRect() const
{
    wing::RectF boundingRect;
    wing::PointF minPoint(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    wing::PointF maxPoint(-1, -1);
    minPoint.X = std::min(source.Point().X, minPoint.X);
    minPoint.Y = std::min(source.Point().Y, minPoint.Y);
    for (const auto& endPoint : sourceEndPoints)
    {
        minPoint.X = std::min(endPoint.Point().X, minPoint.X);
        minPoint.Y = std::min(endPoint.Point().Y, minPoint.Y);
        maxPoint.X = std::max(endPoint.Point().X, maxPoint.X);
        maxPoint.Y = std::max(endPoint.Point().Y, maxPoint.Y);
    }
    minPoint.X = std::min(target.Point().X, minPoint.X);
    minPoint.Y = std::min(target.Point().Y, minPoint.Y);
    maxPoint.X = std::max(source.Point().X, maxPoint.X);
    maxPoint.Y = std::max(source.Point().Y, maxPoint.Y);
    maxPoint.X = std::max(target.Point().X, maxPoint.X);
    maxPoint.Y = std::max(target.Point().Y, maxPoint.Y);
    for (const wing::PointF& intermediatePoint : intermediatePoints)
    {
        minPoint.X = std::min(intermediatePoint.X, minPoint.X);
        minPoint.Y = std::min(intermediatePoint.Y, minPoint.Y);
        maxPoint.X = std::max(intermediatePoint.X, maxPoint.X);
        maxPoint.Y = std::max(intermediatePoint.Y, maxPoint.Y);
    }
    boundingRect.X = minPoint.X;
    boundingRect.Y = minPoint.Y;
    boundingRect.Width = maxPoint.X - minPoint.X;
    boundingRect.Height = maxPoint.Y - minPoint.Y;
    return boundingRect;
}

void RelationshipElement::Measure(wing::Graphics& graphics)
{
    Layout* layout = Configuration::Instance().GetLayout();
    RelationshipLayoutElement* relationshipLayoutElement = layout->GetRelationshipLayoutElement();
    wing::Font* font = relationshipLayoutElement->GetFontElement()->GetFont();
    if (!source.Text().empty())
    {
        wing::RectF r = wing::MeasureString(graphics, source.Text(), *font, wing::PointF(0, 0), layout->GetStringFormat());
        r.GetSize(&sourceTextSize);
    }
    else
    {
        sourceTextSize = wing::SizeF();
    }
    if (!target.Text().empty())
    {
        wing::RectF r = wing::MeasureString(graphics, target.Text(), *font, wing::PointF(0, 0), layout->GetStringFormat());
        r.GetSize(&targetTextSize);
    }
    else
    {
        targetTextSize = wing::SizeF();
    }
}

void RelationshipElement::Draw(wing::Graphics& graphics)
{
    DiagramElement::Draw(graphics);
    rep->Draw(graphics);
}

void RelationshipElement::DrawSelected(wing::Graphics& graphics)
{
    rep->DrawSelected(graphics);
}

void RelationshipElement::Offset(float dx, float dy)
{
    springpp::Offset(source.Point(), dx, dy);
    for (auto& sourceEndPoint : sourceEndPoints)
    {
        springpp::Offset(sourceEndPoint.Point(), dx, dy);
    }
    for (auto& intermediatePoint : intermediatePoints)
    {
        springpp::Offset(intermediatePoint, dx, dy);
    }
    springpp::Offset(target.Point(), dx, dy);
}

bool RelationshipElement::IntersectsWith(const wing::RectF& rect) const
{
    if (rect.Contains(source.Point())) return true;
    for (const auto& sourceEndPoint : sourceEndPoints)
    {
        if (rect.Contains(sourceEndPoint.Point())) return true;
    }
    if (rect.Contains(target.Point())) return true;
    for (const auto& intermediatePoint : intermediatePoints)
    {
        if (rect.Contains(intermediatePoint)) return true;
    }
    return BoundingRect().IntersectsWith(rect);
}

bool RelationshipElement::Contains(const wing::PointF& location) const
{
    return rep->Contains(location);
}

int RelationshipElement::MainDirection() const
{
    if (sourceEndPoints.empty())
    {
        if (source.Element() != nullptr && target.Element() != nullptr)
        {
            return springpp::MainDirection(source.Element(), target.Element());
        }
    }
    else if (target.Element() != nullptr)
    {
        std::map<int, int> directionMap;
        for (const auto& sourceEndPoint : sourceEndPoints)
        {
            if (sourceEndPoint.Element() != nullptr)
            {
                Line line(target.Element()->Center(), sourceEndPoint.Element()->Center());
                Vector v = line.ToVector();
                if (v.x > 0) ++directionMap[180];
                if (v.y > 0) ++directionMap[270];
                if (v.x < 0) ++directionMap[0];
                if (v.y < 0) ++directionMap[90];
            }
        }
        std::vector<std::pair<int, int>> directionList;
        for (const auto& p : directionMap)
        {
            directionList.push_back(p);
        }
        std::sort(directionList.begin(), directionList.end(), DecreasingOrderBySecond());
        std::pair<int, int> max = directionList.front();
        return max.first;
    }
    return 0;
}

void RelationshipElement::SetCompoundLocation(const CompoundLocation& compoundLocation)
{
    SetLocation(compoundLocation.Location());
    intermediatePoints = compoundLocation.Points();
    if (IsReference() && cardinality == Cardinality::zero)
    {
        target.Point() = compoundLocation.TargetPoint();
    }
}

CompoundLocation RelationshipElement::GetCompoundLocation() const
{
    CompoundLocation compoundLocation(Location(), intermediatePoints);
    if (IsReference() && cardinality == Cardinality::zero)
    {
        compoundLocation.SetTargetPoint(target.Point());
    }
    return compoundLocation;
}

void RelationshipElement::AddActions(Diagram* diagram, int elementIndex, wing::ContextMenu* contextMenu) const
{
    DiagramElement::AddActions(diagram, elementIndex, contextMenu);
    if (!IsCombinedInheritance())
    {
        wing::MenuItem* straightenMenuItem = new wing::MenuItem("Straighten");
        contextMenu->AddMenuItem(straightenMenuItem);
        contextMenu->AddAction(new StraightenRelationshipElementAction(diagram, elementIndex, straightenMenuItem));
    }
    else
    {
        wing::MenuItem* splitMenuItem = new wing::MenuItem("Split");
        contextMenu->AddMenuItem(splitMenuItem);
        contextMenu->AddAction(new SplitRelationshipElementAction(diagram, elementIndex, splitMenuItem));
    }
    wing::MenuItem* propertiesMenuItem = new wing::MenuItem("Properties...");
    contextMenu->AddMenuItem(propertiesMenuItem);
    contextMenu->AddAction(new RelationshipElementPropertiesAction(diagram, elementIndex, propertiesMenuItem));
}

void RelationshipElement::RemoveContainer(ContainerElement* container)
{
    if (source.Element() == container)
    {
        source.SetElement(nullptr);
    }
    for (auto& sourceEndPoint : sourceEndPoints)
    {
        if (sourceEndPoint.Element() == container)
        {
            sourceEndPoint.SetElement(nullptr);
        }
    }
    if (target.Element() == container)
    {
        target.SetElement(nullptr);
    }
}

void RelationshipElement::AddToElements()
{
    if (source.Element() && target.Element())
    {
        source.Element()->AddRelationship(this);
        target.Element()->AddRelationship(this);
    }
    for (auto& sourceEndPoint : sourceEndPoints)
    {
        if (sourceEndPoint.Element() && target.Element())
        {
            sourceEndPoint.Element()->AddRelationship(this);
            target.Element()->AddRelationship(this);
        }
    }
}

void RelationshipElement::RemoveFromElements()
{
    if (source.Element() != nullptr)
    {
        source.Element()->RemoveRelationship(this);
    }
    for (EndPoint& sourceEndPoint : sourceEndPoints)
    {
        if (sourceEndPoint.Element() != nullptr)
        {
            sourceEndPoint.Element()->RemoveRelationship(this);
        }
    }
    if (target.Element() != nullptr)
    {
        target.Element()->RemoveRelationship(this);
    }
}

void RelationshipElement::SetContainerElementIndeces(const std::map<ContainerElement*, int>& containerElementIndexMap)
{
    source.SetIndex(containerElementIndexMap);
    for (auto& sourceEndPoint : sourceEndPoints)
    {
        sourceEndPoint.SetIndex(containerElementIndexMap);
    }
    target.SetIndex(containerElementIndexMap);
}

void RelationshipElement::MapContainerElements(const std::map<DiagramElement*, DiagramElement*>& cloneMap)
{
    if (source.Element())
    {
        auto it = cloneMap.find(source.Element());
        if (it != cloneMap.end())
        {
            source.SetElement(it->second);
        }
    }
    for (auto& sourceEndPoint : sourceEndPoints)
    {
        if (sourceEndPoint.Element())
        {
            auto it = cloneMap.find(sourceEndPoint.Element());
            if (it != cloneMap.end())
            {
                sourceEndPoint.SetElement(it->second);
            }
        }
    }
    if (target.Element())
    {
        auto it = cloneMap.find(target.Element());
        if (it != cloneMap.end())
        {
            target.SetElement(it->second);
        }
    }
}

void RelationshipElement::Resolve(Diagram* diagram)
{
    source.Resolve(diagram);
    if (source.Element())
    {
        source.Element()->AddRelationship(this);
    }
    for (auto& sourceEndPoint : sourceEndPoints)
    {
        sourceEndPoint.Resolve(diagram);
        if (sourceEndPoint.Element())
        {
            sourceEndPoint.Element()->AddRelationship(this);
        }
    }
    target.Resolve(diagram);
    if (target.Element())
    {
        target.Element()->AddRelationship(this);
    }
}

void RelationshipElement::Calculate(const Snap& snap, DiagramElement* element, float w, int index, int count)
{
    EndPoint endPoint = element->GetEndPoint(snap);
    if (source.Element() == element && source.GetSnap() == snap)
    {
        source.Point() = snap.Calculate(endPoint.Point(), w, index, count);
    }
    if (target.Element() == element && target.GetSnap() == snap)
    {
        target.Point() = snap.Calculate(endPoint.Point(), w, index, count);
    }
}

void RelationshipElement::Straighten()
{
    if (!intermediatePoints.empty())
    {
        wing::PointF prev = source.Point();
        for (wing::PointF& next : intermediatePoints)
        {
            Line line(prev, next);
            Vector v(line.ToVector());
            switch (springpp::MainDirection(v))
            {
                case 0:
                case 180:
                {
                    next.Y = prev.Y;
                    break;
                }
                case 90:
                case 270:
                {
                    next.X = prev.X;
                    break;
                }
            }
            prev = next;
        }
        wing::PointF& last = intermediatePoints.back();
        Line line(last, target.Point());
        Vector v(line.ToVector());
        switch (springpp::MainDirection(v))
        {
            case 0:
            case 180:
            {
                last.Y = target.Point().Y;
                break;
            }
            case 90:
            case 270:
            {
                last.X = target.Point().X;
                break;
            }
        }
    }
    else
    {
        Line line(source.Point(), target.Point());
        Vector v(line.ToVector());
        switch (springpp::MainDirection(v))
        {
            case 0:
            case 180:
            {
                if (target.Element() != nullptr)
                {
                    float dy = source.Point().Y - target.Point().Y;
                    target.Element()->Offset(0.0f, dy);
                }
                else
                {
                    target.Point().Y = source.Point().Y;
                }
                break;
            }
            case 90:
            case 270:
            {
                if (target.Element() != nullptr)
                {
                    float dx = source.Point().X - target.Point().X;
                    target.Element()->Offset(dx, 0.0f);
                }
                else
                {
                    target.Point().X = source.Point().X;
                }
                break;
            }
        }
    }
}

void RelationshipElement::MapIndeces(const std::map<int, int>& indexMap)
{
    source.MapIndex(indexMap);
    for (EndPoint& sourceEndPoint : sourceEndPoints)
    {
        sourceEndPoint.MapIndex(indexMap);
    }
    target.MapIndex(indexMap);
}

} // namespace springpp
