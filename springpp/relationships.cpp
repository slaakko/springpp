// =================================
// Copyright (c) 2024 Seppo Laakko
// Distributed under the MIT license
// =================================

module springpp.relationships;

import springpp.configuration;
import springpp.class_element;
import springpp.canvas;
import springpp.diagram;
import springpp.diagram_util;

namespace springpp {

Inheritance::Inheritance(RelationshipElement* relationshipElement_) : RelationshipElementRep(relationshipElement_)
{
}

RelationshipElementRep* Inheritance::Clone(RelationshipElement* relationshipElement_) const 
{
    Inheritance* clone = new Inheritance(relationshipElement_);
    return clone;
}

float Inheritance::TargetSymbolWidth() const
{
    Layout* layout = Configuration::Instance().GetLayout();
    RelationshipLayoutElement* relationshipLayoutElement = layout->GetRelationshipLayoutElement();
    float inheritanceArrowWidth = relationshipLayoutElement->InheritanceArrowWidth();
    return inheritanceArrowWidth;
}

void Inheritance::Draw(wing::Graphics& graphics)
{
    RelationshipElement* relationshipElement = GetRelationshipElement();
    Layout* layout = Configuration::Instance().GetLayout();
    RelationshipLayoutElement* relationshipLayoutElement = layout->GetRelationshipLayoutElement();
    wing::Pen* linePen = relationshipLayoutElement->LinePen();
    float inheritanceArrowWidth = relationshipLayoutElement->InheritanceArrowWidth();
    float inheritanceArrowHeight = std::sqrt(3.0) / 2.0 * inheritanceArrowWidth;
    wing::PointF lastPoint;
    if (relationshipElement->RoutingPoints().empty())
    {
        lastPoint = relationshipElement->Source().Point();
    }
    else
    {
        wing::PointF prevPoint = relationshipElement->Source().Point();
        for (const auto& routingPoint : relationshipElement->RoutingPoints())
        {
            graphics.DrawLine(linePen, prevPoint, routingPoint);
            prevPoint = routingPoint;
        }
        lastPoint = relationshipElement->RoutingPoints().back();
    }
    Line line(lastPoint, relationshipElement->Target().Point());
    Vector v = line.ToVector();
    float len = v.Length();
    float startLen = std::max(0.0f, len - inheritanceArrowHeight);
    Vector u = UnitVector(v) * startLen;
    Line startLine(lastPoint, u);
    graphics.DrawLine(linePen, startLine.start, startLine.end);
    Line arrowLine(startLine.end, relationshipElement->Target().Point());
    Line arrowRightRotatedLine = Rotate(arrowLine, 90.0f);
    Vector arv = UnitVector(arrowRightRotatedLine.ToVector()) * (inheritanceArrowWidth / 2);
    Line arrowRightLine(arrowRightRotatedLine.start, arv);
    graphics.DrawLine(linePen, arrowRightLine.end, relationshipElement->Target().Point());
    Line arrowLeftRotatedLine = Rotate(arrowLine, -90.0f);
    Vector alv = UnitVector(arrowLeftRotatedLine.ToVector()) * (inheritanceArrowWidth / 2);
    Line arrowLeftLine(arrowLeftRotatedLine.start, alv);
    graphics.DrawLine(linePen, arrowLeftLine.end, relationshipElement->Target().Point());
    graphics.DrawLine(linePen, arrowLeftLine.end, arrowRightLine.end);
}

CombinedInheritance::CombinedInheritance(RelationshipElement* relationshipElement_) : RelationshipElementRep(relationshipElement_)
{
}

float CombinedInheritance::TargetSymbolWidth() const
{
    RelationshipElement* relationshipElement = GetRelationshipElement();
    Layout* layout = Configuration::Instance().GetLayout();
    RelationshipLayoutElement* relationshipLayoutElement = layout->GetRelationshipLayoutElement();
    float inheritanceArrowWidth = relationshipLayoutElement->InheritanceArrowWidth();
    return inheritanceArrowWidth;
}

RelationshipElementRep* CombinedInheritance::Clone(RelationshipElement* relationshipElement_) const
{
    CombinedInheritance* clone = new CombinedInheritance(relationshipElement_);
    return clone;
}

void CombinedInheritance::Draw(wing::Graphics& graphics)
{
    RelationshipElement* relationshipElement = GetRelationshipElement();
    int mainDirection = relationshipElement->MainDirection();
    switch (mainDirection)
    {
        case 0:
        {
            DrawCombinedInheritance0(graphics, relationshipElement);
            break;
        }
        case 90:
        {
            DrawCombinedInheritance90(graphics, relationshipElement);
            break;
        }
        case 180:
        {
            DrawCombinedInheritance180(graphics, relationshipElement);
            break;
        }
        case 270:
        {
            DrawCombinedInheritance270(graphics, relationshipElement);
            break;
        }
    }
}

void CombinedInheritance::DrawSelected(wing::Graphics& graphics)
{
    RelationshipElement* relationshipElement = GetRelationshipElement();
    int mainDirection = relationshipElement->MainDirection();
    switch (mainDirection)
    {
        case 0:
        {
            DrawCombinedInheritance0Selected(graphics, relationshipElement);
            break;
        }
        case 90:
        {
            DrawCombinedInheritance90Selected(graphics, relationshipElement);
            break;
        }
        case 180:
        {
            DrawCombinedInheritance180Selected(graphics, relationshipElement);
            break;
        }
        case 270:
        {
            DrawCombinedInheritance270Selected(graphics, relationshipElement);
            break;
        }
    }
}

bool CombinedInheritance::Contains(const wing::PointF& location) const
{
    RelationshipElement* relationshipElement = GetRelationshipElement();
    int mainDirection = relationshipElement->MainDirection();
    switch (mainDirection)
    {
        case 0:
        {
            return Contains0(location, relationshipElement);
        }
        case 90:
        {
            return Contains90(location, relationshipElement);
        }
        case 180:
        {
            return Contains180(location, relationshipElement);
        }
        case 270:
        {
            return Contains270(location, relationshipElement);
        }
    }
    return false;
}

void CombinedInheritance::DrawCombinedInheritance0(wing::Graphics& graphics, RelationshipElement* relationshipElement)
{
    Diagram* diagram = relationshipElement->GetDiagram();
    ClassElement* targetClassElement = nullptr;
    EndPoint& targetEndPoint = relationshipElement->Target();
    if (targetEndPoint.Element() && targetEndPoint.Element()->IsClassElement())
    {
        targetClassElement = static_cast<ClassElement*>(targetEndPoint.Element());
    }
    if (!targetClassElement) return;
    EndPoint leftCenterEndPoint = targetClassElement->GetEndPoint(Connector(Connector::LeftCenter()));
    wing::PointF targetPoint = leftCenterEndPoint.Point();
    targetEndPoint.SetPoint(targetPoint);
    std::vector<int> indeces;
    for (const EndPoint& sourceEndPoint : relationshipElement->SourceEndPoints())
    {
        if (sourceEndPoint.Element() != nullptr)
        {
            if (sourceEndPoint.Element()->IsClassElement())
            {
                int index = diagram->GetIndexOfElement(sourceEndPoint.Element());
                if (index != -1)
                {
                    indeces.push_back(index);
                }
            }
        }
    }
    if (indeces.size() < 2) return;
    Layout* layout = Configuration::Instance().GetLayout();
    RelationshipLayoutElement* relationshipLayoutElement = layout->GetRelationshipLayoutElement();
    wing::Pen* linePen = relationshipLayoutElement->LinePen();
    float inheritanceArrowWidth = relationshipLayoutElement->InheritanceArrowWidth();
    float inheritanceArrowHeight = static_cast<float>(std::sqrt(3.0f) / 2.0f * inheritanceArrowWidth);
    std::vector<int> horizontalIndexOrder = indeces;
    std::sort(horizontalIndexOrder.begin(), horizontalIndexOrder.end(), HorizontallyLess(diagram));
    std::vector<ClassElement*> horizontalOrder;
    for (int index : horizontalIndexOrder)
    {
        horizontalOrder.push_back(static_cast<ClassElement*>(diagram->GetElementByIndex(index)));
    }
    std::vector<int> verticalIndexOrder = indeces;
    std::sort(verticalIndexOrder.begin(), verticalIndexOrder.end(), VerticallyLess(diagram));
    std::vector<ClassElement*> verticalOrder;
    for (int index : verticalIndexOrder)
    {
        verticalOrder.push_back(static_cast<ClassElement*>(diagram->GetElementByIndex(index)));
    }
    ClassElement* rightElement = horizontalOrder.back();
    EndPoint rightEndPoint = rightElement->GetEndPoint(Connector(Connector::RightCenter()));
    wing::PointF rightPoint = rightEndPoint.Point();
    float dx = (targetPoint.X - inheritanceArrowHeight) - rightPoint.X;
    float w = dx / 2;
    wing::PointF crossPoint(targetPoint.X - inheritanceArrowHeight - w, targetPoint.Y);
    for (ClassElement* sourceElement : verticalOrder)
    {
        EndPoint sourceEndPoint = sourceElement->GetEndPoint(Connector(Connector::RightCenter()));
        wing::PointF sourcePoint = sourceEndPoint.Point();
        wing::PointF vpoint(crossPoint.X, sourcePoint.Y);
        graphics.DrawLine(linePen, sourcePoint, vpoint);
        graphics.DrawLine(linePen, vpoint, crossPoint);
    }
    Line line(crossPoint, targetPoint);
    Vector v = line.ToVector();
    float len = v.Length();
    float startLen = std::max(0.0f, len - inheritanceArrowHeight);
    Vector u = UnitVector(v) * startLen;
    Line startLine(crossPoint, u);
    graphics.DrawLine(linePen, startLine.start, startLine.end);
    Line arrowLine(startLine.end, targetPoint);
    Line arrowRightRotatedLine = Rotate(arrowLine, 90.0f);
    Vector arv = UnitVector(arrowRightRotatedLine.ToVector()) * (inheritanceArrowWidth / 2);
    Line arrowRightLine(arrowRightRotatedLine.start, arv);
    graphics.DrawLine(linePen, arrowRightLine.end, targetPoint);
    Line arrowLeftRotatedLine = Rotate(arrowLine, -90.0f);
    Vector alv = UnitVector(arrowLeftRotatedLine.ToVector()) * (inheritanceArrowWidth / 2);
    Line arrowLeftLine(arrowLeftRotatedLine.start, alv);
    graphics.DrawLine(linePen, arrowLeftLine.end, targetPoint);
    graphics.DrawLine(linePen, arrowLeftLine.end, arrowRightLine.end);
}

void CombinedInheritance::DrawCombinedInheritance90(wing::Graphics& graphics, RelationshipElement* relationshipElement)
{
    Diagram* diagram = relationshipElement->GetDiagram();
    ClassElement* targetClassElement = nullptr;
    EndPoint& targetEndPoint = relationshipElement->Target();
    if (targetEndPoint.Element() && targetEndPoint.Element()->IsClassElement())
    {
        targetClassElement = static_cast<ClassElement*>(targetEndPoint.Element());
    }
    if (!targetClassElement) return;
    EndPoint topCenterEndPoint = targetClassElement->GetEndPoint(Connector(Connector::TopCenter()));
    wing::PointF targetPoint = topCenterEndPoint.Point();
    targetEndPoint.SetPoint(targetPoint);
    std::vector<int> indeces;
    for (const EndPoint& sourceEndPoint : relationshipElement->SourceEndPoints())
    {
        if (sourceEndPoint.Element() != nullptr)
        {
            if (sourceEndPoint.Element()->IsClassElement())
            {
                int index = diagram->GetIndexOfElement(sourceEndPoint.Element());
                if (index != -1)
                {
                    indeces.push_back(index);
                }
            }
        }
    }
    if (indeces.size() < 2) return;
    Layout* layout = Configuration::Instance().GetLayout();
    RelationshipLayoutElement* relationshipLayoutElement = layout->GetRelationshipLayoutElement();
    wing::Pen* linePen = relationshipLayoutElement->LinePen();
    float inheritanceArrowWidth = relationshipLayoutElement->InheritanceArrowWidth();
    float inheritanceArrowHeight = static_cast<float>(std::sqrt(3.0f) / 2.0f * inheritanceArrowWidth);
    std::vector<int> horizontalIndexOrder = indeces;
    std::sort(horizontalIndexOrder.begin(), horizontalIndexOrder.end(), HorizontallyLess(diagram));
    std::vector<ClassElement*> horizontalOrder;
    for (int index : horizontalIndexOrder)
    {
        horizontalOrder.push_back(static_cast<ClassElement*>(diagram->GetElementByIndex(index)));
    }
    std::vector<int> verticalIndexOrder = indeces;
    std::sort(verticalIndexOrder.begin(), verticalIndexOrder.end(), VerticallyLess(diagram));
    std::vector<ClassElement*> verticalOrder;
    for (int index : verticalIndexOrder)
    {
        verticalOrder.push_back(static_cast<ClassElement*>(diagram->GetElementByIndex(index)));
    }
    ClassElement* bottomElement = verticalOrder.back();
    EndPoint bottomEndPoint = bottomElement->GetEndPoint(Connector(Connector::BottomCenter()));
    wing::PointF bottomPoint = bottomEndPoint.Point();
    float dy = (targetPoint.Y - inheritanceArrowHeight) - bottomPoint.Y;
    float h = dy / 2;
    wing::PointF crossPoint(targetPoint.X, targetPoint.Y - inheritanceArrowHeight - h);
    for (ClassElement* sourceElement : horizontalOrder)
    {
        EndPoint sourceEndPoint = sourceElement->GetEndPoint(Connector(Connector::BottomCenter()));
        wing::PointF sourcePoint = sourceEndPoint.Point();
        wing::PointF vpoint(sourcePoint.X, crossPoint.Y);
        graphics.DrawLine(linePen, sourcePoint, vpoint);
        graphics.DrawLine(linePen, vpoint, crossPoint);
    }
    Line line(crossPoint, targetPoint);
    Vector v = line.ToVector();
    float len = v.Length();
    float startLen = std::max(0.0f, len - inheritanceArrowHeight);
    Vector u = UnitVector(v) * startLen;
    Line startLine(crossPoint, u);
    graphics.DrawLine(linePen, startLine.start, startLine.end);
    Line arrowLine(startLine.end, targetPoint);
    Line arrowRightRotatedLine = Rotate(arrowLine, 90.0f);
    Vector arv = UnitVector(arrowRightRotatedLine.ToVector()) * (inheritanceArrowWidth / 2);
    Line arrowRightLine(arrowRightRotatedLine.start, arv);
    graphics.DrawLine(linePen, arrowRightLine.end, targetPoint);
    Line arrowLeftRotatedLine = Rotate(arrowLine, -90.0f);
    Vector alv = UnitVector(arrowLeftRotatedLine.ToVector()) * (inheritanceArrowWidth / 2);
    Line arrowLeftLine(arrowLeftRotatedLine.start, alv);
    graphics.DrawLine(linePen, arrowLeftLine.end, targetPoint);
    graphics.DrawLine(linePen, arrowLeftLine.end, arrowRightLine.end);
}

void CombinedInheritance::DrawCombinedInheritance180(wing::Graphics& graphics, RelationshipElement* relationshipElement)
{
    Diagram* diagram = relationshipElement->GetDiagram();
    ClassElement* targetClassElement = nullptr;
    EndPoint& targetEndPoint = relationshipElement->Target();
    if (targetEndPoint.Element() && targetEndPoint.Element()->IsClassElement())
    {
        targetClassElement = static_cast<ClassElement*>(targetEndPoint.Element());
    }
    if (!targetClassElement) return;
    EndPoint rightCenterEndPoint = targetClassElement->GetEndPoint(Connector(Connector::RightCenter()));
    wing::PointF targetPoint = rightCenterEndPoint.Point();
    targetEndPoint.SetPoint(targetPoint);
    std::vector<int> indeces;
    for (const EndPoint& sourceEndPoint : relationshipElement->SourceEndPoints())
    {
        if (sourceEndPoint.Element() != nullptr)
        {
            if (sourceEndPoint.Element()->IsClassElement())
            {
                int index = diagram->GetIndexOfElement(sourceEndPoint.Element());
                if (index != -1)
                {
                    indeces.push_back(index);
                }
            }
        }
    }
    if (indeces.size() < 2) return;
    Layout* layout = Configuration::Instance().GetLayout();
    RelationshipLayoutElement* relationshipLayoutElement = layout->GetRelationshipLayoutElement();
    wing::Pen* linePen = relationshipLayoutElement->LinePen();
    float inheritanceArrowWidth = relationshipLayoutElement->InheritanceArrowWidth();
    float inheritanceArrowHeight = static_cast<float>(std::sqrt(3.0f) / 2.0f * inheritanceArrowWidth);
    std::vector<int> horizontalIndexOrder = indeces;
    std::sort(horizontalIndexOrder.begin(), horizontalIndexOrder.end(), HorizontallyLess(diagram));
    std::vector<ClassElement*> horizontalOrder;
    for (int index : horizontalIndexOrder)
    {
        horizontalOrder.push_back(static_cast<ClassElement*>(diagram->GetElementByIndex(index)));
    }
    std::vector<int> verticalIndexOrder = indeces;
    std::sort(verticalIndexOrder.begin(), verticalIndexOrder.end(), VerticallyLess(diagram));
    std::vector<ClassElement*> verticalOrder;
    for (int index : verticalIndexOrder)
    {
        verticalOrder.push_back(static_cast<ClassElement*>(diagram->GetElementByIndex(index)));
    }
    ClassElement* leftElement = horizontalOrder.front();
    EndPoint leftEndPoint = leftElement->GetEndPoint(Connector(Connector::LeftCenter()));
    wing::PointF leftPoint = leftEndPoint.Point();
    float dx = leftPoint.X - (targetPoint.X + inheritanceArrowHeight);
    float w = dx / 2;
    wing::PointF crossPoint(targetPoint.X + inheritanceArrowHeight + w, targetPoint.Y);
    for (ClassElement* sourceElement : verticalOrder)
    {
        EndPoint sourceEndPoint = sourceElement->GetEndPoint(Connector(Connector::LeftCenter()));
        wing::PointF sourcePoint = sourceEndPoint.Point();
        wing::PointF vpoint(crossPoint.X, sourcePoint.Y);
        graphics.DrawLine(linePen, sourcePoint, vpoint);
        graphics.DrawLine(linePen, vpoint, crossPoint);
    }
    Line line(crossPoint, targetPoint);
    Vector v = line.ToVector();
    float len = v.Length();
    float startLen = std::max(0.0f, len - inheritanceArrowHeight);
    Vector u = UnitVector(v) * startLen;
    Line startLine(crossPoint, u);
    graphics.DrawLine(linePen, startLine.start, startLine.end);
    Line arrowLine(startLine.end, targetPoint);
    Line arrowRightRotatedLine = Rotate(arrowLine, 90.0f);
    Vector arv = UnitVector(arrowRightRotatedLine.ToVector()) * (inheritanceArrowWidth / 2);
    Line arrowRightLine(arrowRightRotatedLine.start, arv);
    graphics.DrawLine(linePen, arrowRightLine.end, targetPoint);
    Line arrowLeftRotatedLine = Rotate(arrowLine, -90.0f);
    Vector alv = UnitVector(arrowLeftRotatedLine.ToVector()) * (inheritanceArrowWidth / 2);
    Line arrowLeftLine(arrowLeftRotatedLine.start, alv);
    graphics.DrawLine(linePen, arrowLeftLine.end, targetPoint);
    graphics.DrawLine(linePen, arrowLeftLine.end, arrowRightLine.end);
}

void CombinedInheritance::DrawCombinedInheritance270(wing::Graphics& graphics, RelationshipElement* relationshipElement)
{
    Diagram* diagram = relationshipElement->GetDiagram();
    ClassElement* targetClassElement = nullptr;
    EndPoint& targetEndPoint = relationshipElement->Target();
    if (targetEndPoint.Element() && targetEndPoint.Element()->IsClassElement())
    {
        targetClassElement = static_cast<ClassElement*>(targetEndPoint.Element());
    }
    if (!targetClassElement) return;
    EndPoint bottomCenterEndPoint = targetClassElement->GetEndPoint(Connector(Connector::BottomCenter()));
    wing::PointF targetPoint = bottomCenterEndPoint.Point();
    targetEndPoint.SetPoint(targetPoint);
    std::vector<int> indeces;
    for (const EndPoint& sourceEndPoint : relationshipElement->SourceEndPoints())
    {
        if (sourceEndPoint.Element() != nullptr)
        {
            if (sourceEndPoint.Element()->IsClassElement())
            {
                int index = diagram->GetIndexOfElement(sourceEndPoint.Element());
                if (index != -1)
                {
                    indeces.push_back(index);
                }
            }
        }
    }
    if (indeces.size() < 2) return;
    Layout* layout = Configuration::Instance().GetLayout();
    RelationshipLayoutElement* relationshipLayoutElement = layout->GetRelationshipLayoutElement();
    wing::Pen* linePen = relationshipLayoutElement->LinePen();
    float inheritanceArrowWidth = relationshipLayoutElement->InheritanceArrowWidth();
    float inheritanceArrowHeight = static_cast<float>(std::sqrt(3.0f) / 2.0f * inheritanceArrowWidth);
    std::vector<int> horizontalIndexOrder = indeces;
    std::sort(horizontalIndexOrder.begin(), horizontalIndexOrder.end(), HorizontallyLess(diagram));
    std::vector<ClassElement*> horizontalOrder;
    for (int index : horizontalIndexOrder)
    {
        horizontalOrder.push_back(static_cast<ClassElement*>(diagram->GetElementByIndex(index)));
    }
    std::vector<int> verticalIndexOrder = indeces;
    std::sort(verticalIndexOrder.begin(), verticalIndexOrder.end(), VerticallyLess(diagram));
    std::vector<ClassElement*> verticalOrder;
    for (int index : verticalIndexOrder)
    {
        verticalOrder.push_back(static_cast<ClassElement*>(diagram->GetElementByIndex(index)));
    }
    ClassElement* topElement = verticalOrder.front();
    EndPoint topEndPoint = topElement->GetEndPoint(Connector(Connector::TopCenter()));
    wing::PointF topPoint = topEndPoint.Point();
    float dy = topPoint.Y - (targetPoint.Y + inheritanceArrowHeight);
    float h = dy / 2;
    wing::PointF crossPoint(targetPoint.X, targetPoint.Y + inheritanceArrowHeight + h);
    for (ClassElement* sourceElement : horizontalOrder)
    {
        EndPoint sourceEndPoint = sourceElement->GetEndPoint(Connector(Connector::TopCenter()));
        wing::PointF sourcePoint = sourceEndPoint.Point();
        wing::PointF vpoint(sourcePoint.X, crossPoint.Y);
        graphics.DrawLine(linePen, sourcePoint, vpoint);
        graphics.DrawLine(linePen, vpoint, crossPoint);
    }
    Line line(crossPoint, targetPoint);
    Vector v = line.ToVector();
    float len = v.Length();
    float startLen = std::max(0.0f, len - inheritanceArrowHeight);
    Vector u = UnitVector(v) * startLen;
    Line startLine(crossPoint, u);
    graphics.DrawLine(linePen, startLine.start, startLine.end);
    Line arrowLine(startLine.end, targetPoint);
    Line arrowRightRotatedLine = Rotate(arrowLine, 90.0f);
    Vector arv = UnitVector(arrowRightRotatedLine.ToVector()) * (inheritanceArrowWidth / 2);
    Line arrowRightLine(arrowRightRotatedLine.start, arv);
    graphics.DrawLine(linePen, arrowRightLine.end, targetPoint);
    Line arrowLeftRotatedLine = Rotate(arrowLine, -90.0f);
    Vector alv = UnitVector(arrowLeftRotatedLine.ToVector()) * (inheritanceArrowWidth / 2);
    Line arrowLeftLine(arrowLeftRotatedLine.start, alv);
    graphics.DrawLine(linePen, arrowLeftLine.end, targetPoint);
    graphics.DrawLine(linePen, arrowLeftLine.end, arrowRightLine.end);
}

void CombinedInheritance::DrawCombinedInheritance0Selected(wing::Graphics& graphics, RelationshipElement* relationshipElement)
{
    Diagram* diagram = relationshipElement->GetDiagram();
    ClassElement* targetClassElement = nullptr;
    EndPoint& targetEndPoint = relationshipElement->Target();
    if (targetEndPoint.Element() && targetEndPoint.Element()->IsClassElement())
    {
        targetClassElement = static_cast<ClassElement*>(targetEndPoint.Element());
    }
    if (!targetClassElement) return;
    EndPoint leftCenterEndPoint = targetClassElement->GetEndPoint(Connector(Connector::LeftCenter()));
    wing::PointF targetPoint = leftCenterEndPoint.Point();
    targetEndPoint.SetPoint(targetPoint);
    std::vector<int> indeces;
    for (const EndPoint& sourceEndPoint : relationshipElement->SourceEndPoints())
    {
        if (sourceEndPoint.Element() != nullptr)
        {
            if (sourceEndPoint.Element()->IsClassElement())
            {
                int index = diagram->GetIndexOfElement(sourceEndPoint.Element());
                if (index != -1)
                {
                    indeces.push_back(index);
                }
            }
        }
    }
    if (indeces.size() < 2) return;
    Layout* layout = Configuration::Instance().GetLayout();
    RelationshipLayoutElement* relationshipLayoutElement = layout->GetRelationshipLayoutElement();
    wing::Pen* selectedLinePen = relationshipLayoutElement->SelectedLinePen();
    float inheritanceArrowWidth = relationshipLayoutElement->InheritanceArrowWidth();
    float inheritanceArrowHeight = static_cast<float>(std::sqrt(3.0f) / 2.0f * inheritanceArrowWidth);
    std::vector<int> horizontalIndexOrder = indeces;
    std::sort(horizontalIndexOrder.begin(), horizontalIndexOrder.end(), HorizontallyLess(diagram));
    std::vector<ClassElement*> horizontalOrder;
    for (int index : horizontalIndexOrder)
    {
        horizontalOrder.push_back(static_cast<ClassElement*>(diagram->GetElementByIndex(index)));
    }
    std::vector<int> verticalIndexOrder = indeces;
    std::sort(verticalIndexOrder.begin(), verticalIndexOrder.end(), VerticallyLess(diagram));
    std::vector<ClassElement*> verticalOrder;
    for (int index : verticalIndexOrder)
    {
        verticalOrder.push_back(static_cast<ClassElement*>(diagram->GetElementByIndex(index)));
    }
    ClassElement* rightElement = horizontalOrder.back();
    EndPoint rightEndPoint = rightElement->GetEndPoint(Connector(Connector::RightCenter()));
    wing::PointF rightPoint = rightEndPoint.Point();
    float dx = (targetPoint.X - inheritanceArrowHeight) - rightPoint.X;
    float w = dx / 2;
    wing::PointF crossPoint(targetPoint.X - inheritanceArrowHeight - w, targetPoint.Y);
    for (ClassElement* sourceElement : verticalOrder)
    {
        EndPoint sourceEndPoint = sourceElement->GetEndPoint(Connector(Connector::RightCenter()));
        wing::PointF sourcePoint = sourceEndPoint.Point();
        wing::PointF vpoint(crossPoint.X, sourcePoint.Y);
        graphics.DrawLine(selectedLinePen, sourcePoint, vpoint);
        graphics.DrawLine(selectedLinePen, vpoint, crossPoint);
    }
    Line line(crossPoint, targetPoint);
    Vector v = line.ToVector();
    float len = v.Length();
    float startLen = std::max(0.0f, len - inheritanceArrowHeight);
    Vector u = UnitVector(v) * startLen;
    Line startLine(crossPoint, u);
    graphics.DrawLine(selectedLinePen, startLine.start, startLine.end);
}

void CombinedInheritance::DrawCombinedInheritance90Selected(wing::Graphics& graphics, RelationshipElement* relationshipElement)
{
    Diagram* diagram = relationshipElement->GetDiagram();
    ClassElement* targetClassElement = nullptr;
    EndPoint& targetEndPoint = relationshipElement->Target();
    if (targetEndPoint.Element() && targetEndPoint.Element()->IsClassElement())
    {
        targetClassElement = static_cast<ClassElement*>(targetEndPoint.Element());
    }
    if (!targetClassElement) return;
    EndPoint topCenterEndPoint = targetClassElement->GetEndPoint(Connector(Connector::TopCenter()));
    wing::PointF targetPoint = topCenterEndPoint.Point();
    targetEndPoint.SetPoint(targetPoint);
    std::vector<int> indeces;
    for (const EndPoint& sourceEndPoint : relationshipElement->SourceEndPoints())
    {
        if (sourceEndPoint.Element() != nullptr)
        {
            if (sourceEndPoint.Element()->IsClassElement())
            {
                int index = diagram->GetIndexOfElement(sourceEndPoint.Element());
                if (index != -1)
                {
                    indeces.push_back(index);
                }
            }
        }
    }
    if (indeces.size() < 2) return;
    Layout* layout = Configuration::Instance().GetLayout();
    RelationshipLayoutElement* relationshipLayoutElement = layout->GetRelationshipLayoutElement();
    wing::Pen* selectedLinePen = relationshipLayoutElement->SelectedLinePen();
    float inheritanceArrowWidth = relationshipLayoutElement->InheritanceArrowWidth();
    float inheritanceArrowHeight = static_cast<float>(std::sqrt(3.0f) / 2.0f * inheritanceArrowWidth);
    std::vector<int> horizontalIndexOrder = indeces;
    std::sort(horizontalIndexOrder.begin(), horizontalIndexOrder.end(), HorizontallyLess(diagram));
    std::vector<ClassElement*> horizontalOrder;
    for (int index : horizontalIndexOrder)
    {
        horizontalOrder.push_back(static_cast<ClassElement*>(diagram->GetElementByIndex(index)));
    }
    std::vector<int> verticalIndexOrder = indeces;
    std::sort(verticalIndexOrder.begin(), verticalIndexOrder.end(), VerticallyLess(diagram));
    std::vector<ClassElement*> verticalOrder;
    for (int index : verticalIndexOrder)
    {
        verticalOrder.push_back(static_cast<ClassElement*>(diagram->GetElementByIndex(index)));
    }
    ClassElement* bottomElement = verticalOrder.back();
    EndPoint bottomEndPoint = bottomElement->GetEndPoint(Connector(Connector::BottomCenter()));
    wing::PointF bottomPoint = bottomEndPoint.Point();
    float dy = (targetPoint.Y - inheritanceArrowHeight) - bottomPoint.Y;
    float h = dy / 2;
    wing::PointF crossPoint(targetPoint.X, targetPoint.Y - inheritanceArrowHeight - h);
    for (ClassElement* sourceElement : horizontalOrder)
    {
        EndPoint sourceEndPoint = sourceElement->GetEndPoint(Connector(Connector::BottomCenter()));
        wing::PointF sourcePoint = sourceEndPoint.Point();
        wing::PointF vpoint(sourcePoint.X, crossPoint.Y);
        graphics.DrawLine(selectedLinePen, sourcePoint, vpoint);
        graphics.DrawLine(selectedLinePen, vpoint, crossPoint);
    }
    Line line(crossPoint, targetPoint);
    Vector v = line.ToVector();
    float len = v.Length();
    float startLen = std::max(0.0f, len - inheritanceArrowHeight);
    Vector u = UnitVector(v) * startLen;
    Line startLine(crossPoint, u);
    graphics.DrawLine(selectedLinePen, startLine.start, startLine.end);
}

void CombinedInheritance::DrawCombinedInheritance180Selected(wing::Graphics& graphics, RelationshipElement* relationshipElement)
{
    Diagram* diagram = relationshipElement->GetDiagram();
    ClassElement* targetClassElement = nullptr;
    EndPoint& targetEndPoint = relationshipElement->Target();
    if (targetEndPoint.Element() && targetEndPoint.Element()->IsClassElement())
    {
        targetClassElement = static_cast<ClassElement*>(targetEndPoint.Element());
    }
    if (!targetClassElement) return;
    EndPoint rightCenterEndPoint = targetClassElement->GetEndPoint(Connector(Connector::RightCenter()));
    wing::PointF targetPoint = rightCenterEndPoint.Point();
    targetEndPoint.SetPoint(targetPoint);
    std::vector<int> indeces;
    for (const EndPoint& sourceEndPoint : relationshipElement->SourceEndPoints())
    {
        if (sourceEndPoint.Element() != nullptr)
        {
            if (sourceEndPoint.Element()->IsClassElement())
            {
                int index = diagram->GetIndexOfElement(sourceEndPoint.Element());
                if (index != -1)
                {
                    indeces.push_back(index);
                }
            }
        }
    }
    if (indeces.size() < 2) return;
    Layout* layout = Configuration::Instance().GetLayout();
    RelationshipLayoutElement* relationshipLayoutElement = layout->GetRelationshipLayoutElement();
    wing::Pen* selectedLinePen = relationshipLayoutElement->SelectedLinePen();
    float inheritanceArrowWidth = relationshipLayoutElement->InheritanceArrowWidth();
    float inheritanceArrowHeight = static_cast<float>(std::sqrt(3.0f) / 2.0f * inheritanceArrowWidth);
    std::vector<int> horizontalIndexOrder = indeces;
    std::sort(horizontalIndexOrder.begin(), horizontalIndexOrder.end(), HorizontallyLess(diagram));
    std::vector<ClassElement*> horizontalOrder;
    for (int index : horizontalIndexOrder)
    {
        horizontalOrder.push_back(static_cast<ClassElement*>(diagram->GetElementByIndex(index)));
    }
    std::vector<int> verticalIndexOrder = indeces;
    std::sort(verticalIndexOrder.begin(), verticalIndexOrder.end(), VerticallyLess(diagram));
    std::vector<ClassElement*> verticalOrder;
    for (int index : verticalIndexOrder)
    {
        verticalOrder.push_back(static_cast<ClassElement*>(diagram->GetElementByIndex(index)));
    }
    ClassElement* leftElement = horizontalOrder.front();
    EndPoint leftEndPoint = leftElement->GetEndPoint(Connector(Connector::LeftCenter()));
    wing::PointF leftPoint = leftEndPoint.Point();
    float dx = leftPoint.X - (targetPoint.X + inheritanceArrowHeight);
    float w = dx / 2;
    wing::PointF crossPoint(targetPoint.X + inheritanceArrowHeight + w, targetPoint.Y);
    for (ClassElement* sourceElement : verticalOrder)
    {
        EndPoint sourceEndPoint = sourceElement->GetEndPoint(Connector(Connector::LeftCenter()));
        wing::PointF sourcePoint = sourceEndPoint.Point();
        wing::PointF vpoint(crossPoint.X, sourcePoint.Y);
        graphics.DrawLine(selectedLinePen, sourcePoint, vpoint);
        graphics.DrawLine(selectedLinePen, vpoint, crossPoint);
    }
    Line line(crossPoint, targetPoint);
    Vector v = line.ToVector();
    float len = v.Length();
    float startLen = std::max(0.0f, len - inheritanceArrowHeight);
    Vector u = UnitVector(v) * startLen;
    Line startLine(crossPoint, u);
    graphics.DrawLine(selectedLinePen, startLine.start, startLine.end);
}

void CombinedInheritance::DrawCombinedInheritance270Selected(wing::Graphics& graphics, RelationshipElement* relationshipElement)
{
    Diagram* diagram = relationshipElement->GetDiagram();
    ClassElement* targetClassElement = nullptr;
    EndPoint& targetEndPoint = relationshipElement->Target();
    if (targetEndPoint.Element() && targetEndPoint.Element()->IsClassElement())
    {
        targetClassElement = static_cast<ClassElement*>(targetEndPoint.Element());
    }
    if (!targetClassElement) return;
    EndPoint bottomCenterEndPoint = targetClassElement->GetEndPoint(Connector(Connector::BottomCenter()));
    wing::PointF targetPoint = bottomCenterEndPoint.Point();
    targetEndPoint.SetPoint(targetPoint);
    std::vector<int> indeces;
    for (const EndPoint& sourceEndPoint : relationshipElement->SourceEndPoints())
    {
        if (sourceEndPoint.Element() != nullptr)
        {
            if (sourceEndPoint.Element()->IsClassElement())
            {
                int index = diagram->GetIndexOfElement(sourceEndPoint.Element());
                if (index != -1)
                {
                    indeces.push_back(index);
                }
            }
        }
    }
    if (indeces.size() < 2) return;
    Layout* layout = Configuration::Instance().GetLayout();
    RelationshipLayoutElement* relationshipLayoutElement = layout->GetRelationshipLayoutElement();
    wing::Pen* selectedLinePen = relationshipLayoutElement->SelectedLinePen();
    float inheritanceArrowWidth = relationshipLayoutElement->InheritanceArrowWidth();
    float inheritanceArrowHeight = static_cast<float>(std::sqrt(3.0f) / 2.0f * inheritanceArrowWidth);
    std::vector<int> horizontalIndexOrder = indeces;
    std::sort(horizontalIndexOrder.begin(), horizontalIndexOrder.end(), HorizontallyLess(diagram));
    std::vector<ClassElement*> horizontalOrder;
    for (int index : horizontalIndexOrder)
    {
        horizontalOrder.push_back(static_cast<ClassElement*>(diagram->GetElementByIndex(index)));
    }
    std::vector<int> verticalIndexOrder = indeces;
    std::sort(verticalIndexOrder.begin(), verticalIndexOrder.end(), VerticallyLess(diagram));
    std::vector<ClassElement*> verticalOrder;
    for (int index : verticalIndexOrder)
    {
        verticalOrder.push_back(static_cast<ClassElement*>(diagram->GetElementByIndex(index)));
    }
    ClassElement* topElement = verticalOrder.front();
    EndPoint topEndPoint = topElement->GetEndPoint(Connector(Connector::TopCenter()));
    wing::PointF topPoint = topEndPoint.Point();
    float dy = topPoint.Y - (targetPoint.Y + inheritanceArrowHeight);
    float h = dy / 2;
    wing::PointF crossPoint(targetPoint.X, targetPoint.Y + inheritanceArrowHeight + h);
    for (ClassElement* sourceElement : horizontalOrder)
    {
        EndPoint sourceEndPoint = sourceElement->GetEndPoint(Connector(Connector::TopCenter()));
        wing::PointF sourcePoint = sourceEndPoint.Point();
        wing::PointF vpoint(sourcePoint.X, crossPoint.Y);
        graphics.DrawLine(selectedLinePen, sourcePoint, vpoint);
        graphics.DrawLine(selectedLinePen, vpoint, crossPoint);
    }
    Line line(crossPoint, targetPoint);
    Vector v = line.ToVector();
    float len = v.Length();
    float startLen = std::max(0.0f, len - inheritanceArrowHeight);
    Vector u = UnitVector(v) * startLen;
    Line startLine(crossPoint, u);
    graphics.DrawLine(selectedLinePen, startLine.start, startLine.end);
}

bool CombinedInheritance::Contains0(const wing::PointF& location, RelationshipElement* relationshipElement) const
{
    Diagram* diagram = relationshipElement->GetDiagram();
    ClassElement* targetClassElement = nullptr;
    EndPoint& targetEndPoint = relationshipElement->Target();
    if (targetEndPoint.Element() && targetEndPoint.Element()->IsClassElement())
    {
        targetClassElement = static_cast<ClassElement*>(targetEndPoint.Element());
    }
    if (!targetClassElement) return false;
    EndPoint leftCenterEndPoint = targetClassElement->GetEndPoint(Connector(Connector::LeftCenter()));
    wing::PointF targetPoint = leftCenterEndPoint.Point();
    targetEndPoint.SetPoint(targetPoint);
    std::vector<int> indeces;
    for (const EndPoint& sourceEndPoint : relationshipElement->SourceEndPoints())
    {
        if (sourceEndPoint.Element() != nullptr)
        {
            if (sourceEndPoint.Element()->IsClassElement())
            {
                int index = diagram->GetIndexOfElement(sourceEndPoint.Element());
                if (index != -1)
                {
                    indeces.push_back(index);
                }
            }
        }
    }
    if (indeces.size() < 2) return false;
    Layout* layout = Configuration::Instance().GetLayout();
    RelationshipLayoutElement* relationshipLayoutElement = layout->GetRelationshipLayoutElement();
    float selectedLineWidth = relationshipLayoutElement->SelectedLineWidth();
    float inheritanceArrowWidth = relationshipLayoutElement->InheritanceArrowWidth();
    float inheritanceArrowHeight = static_cast<float>(std::sqrt(3.0f) / 2.0f * inheritanceArrowWidth);
    std::vector<int> horizontalIndexOrder = indeces;
    std::sort(horizontalIndexOrder.begin(), horizontalIndexOrder.end(), HorizontallyLess(diagram));
    std::vector<ClassElement*> horizontalOrder;
    for (int index : horizontalIndexOrder)
    {
        horizontalOrder.push_back(static_cast<ClassElement*>(diagram->GetElementByIndex(index)));
    }
    std::vector<int> verticalIndexOrder = indeces;
    std::sort(verticalIndexOrder.begin(), verticalIndexOrder.end(), VerticallyLess(diagram));
    std::vector<ClassElement*> verticalOrder;
    for (int index : verticalIndexOrder)
    {
        verticalOrder.push_back(static_cast<ClassElement*>(diagram->GetElementByIndex(index)));
    }
    ClassElement* rightElement = horizontalOrder.back();
    EndPoint rightEndPoint = rightElement->GetEndPoint(Connector(Connector::RightCenter()));
    wing::PointF rightPoint = rightEndPoint.Point();
    float dx = (targetPoint.X - inheritanceArrowHeight) - rightPoint.X;
    float w = dx / 2;
    wing::PointF crossPoint(targetPoint.X - inheritanceArrowHeight - w, targetPoint.Y);
    for (ClassElement* sourceElement : verticalOrder)
    {
        EndPoint sourceEndPoint = sourceElement->GetEndPoint(Connector(Connector::RightCenter()));
        wing::PointF sourcePoint = sourceEndPoint.Point();
        wing::PointF vpoint(crossPoint.X, sourcePoint.Y);
        if (springpp::LineContains(sourcePoint, vpoint, location, selectedLineWidth)) return true;
        if (springpp::LineContains(vpoint, crossPoint, location, selectedLineWidth)) return true;
    }
    Line line(crossPoint, targetPoint);
    Vector v = line.ToVector();
    float len = v.Length();
    float startLen = std::max(0.0f, len - inheritanceArrowHeight);
    Vector u = UnitVector(v) * startLen;
    Line startLine(crossPoint, u);
    if (springpp::LineContains(startLine.start, startLine.end, location, selectedLineWidth)) return true;
    return false;
}

bool CombinedInheritance::Contains90(const wing::PointF& location, RelationshipElement* relationshipElement) const
{
    Diagram* diagram = relationshipElement->GetDiagram();
    ClassElement* targetClassElement = nullptr;
    EndPoint& targetEndPoint = relationshipElement->Target();
    if (targetEndPoint.Element() && targetEndPoint.Element()->IsClassElement())
    {
        targetClassElement = static_cast<ClassElement*>(targetEndPoint.Element());
    }
    if (!targetClassElement) return false;
    EndPoint topCenterEndPoint = targetClassElement->GetEndPoint(Connector(Connector::TopCenter()));
    wing::PointF targetPoint = topCenterEndPoint.Point();
    targetEndPoint.SetPoint(targetPoint);
    std::vector<int> indeces;
    for (const EndPoint& sourceEndPoint : relationshipElement->SourceEndPoints())
    {
        if (sourceEndPoint.Element() != nullptr)
        {
            if (sourceEndPoint.Element()->IsClassElement())
            {
                int index = diagram->GetIndexOfElement(sourceEndPoint.Element());
                if (index != -1)
                {
                    indeces.push_back(index);
                }
            }
        }
    }
    if (indeces.size() < 2) return false;
    Layout* layout = Configuration::Instance().GetLayout();
    RelationshipLayoutElement* relationshipLayoutElement = layout->GetRelationshipLayoutElement();
    float selectedLineWidth = relationshipLayoutElement->SelectedLineWidth();
    float inheritanceArrowWidth = relationshipLayoutElement->InheritanceArrowWidth();
    float inheritanceArrowHeight = static_cast<float>(std::sqrt(3.0f) / 2.0f * inheritanceArrowWidth);
    std::vector<int> horizontalIndexOrder = indeces;
    std::sort(horizontalIndexOrder.begin(), horizontalIndexOrder.end(), HorizontallyLess(diagram));
    std::vector<ClassElement*> horizontalOrder;
    for (int index : horizontalIndexOrder)
    {
        horizontalOrder.push_back(static_cast<ClassElement*>(diagram->GetElementByIndex(index)));
    }
    std::vector<int> verticalIndexOrder = indeces;
    std::sort(verticalIndexOrder.begin(), verticalIndexOrder.end(), VerticallyLess(diagram));
    std::vector<ClassElement*> verticalOrder;
    for (int index : verticalIndexOrder)
    {
        verticalOrder.push_back(static_cast<ClassElement*>(diagram->GetElementByIndex(index)));
    }
    ClassElement* bottomElement = verticalOrder.back();
    EndPoint bottomEndPoint = bottomElement->GetEndPoint(Connector(Connector::BottomCenter()));
    wing::PointF bottomPoint = bottomEndPoint.Point();
    float dy = (targetPoint.Y - inheritanceArrowHeight) - bottomPoint.Y;
    float h = dy / 2;
    wing::PointF crossPoint(targetPoint.X, targetPoint.Y - inheritanceArrowHeight - h);
    for (ClassElement* sourceElement : horizontalOrder)
    {
        EndPoint sourceEndPoint = sourceElement->GetEndPoint(Connector(Connector::BottomCenter()));
        wing::PointF sourcePoint = sourceEndPoint.Point();
        wing::PointF vpoint(sourcePoint.X, crossPoint.Y);
        if (springpp::LineContains(sourcePoint, vpoint, location, selectedLineWidth)) return true;
        if (springpp::LineContains(vpoint, crossPoint, location, selectedLineWidth)) return true;
    }
    Line line(crossPoint, targetPoint);
    Vector v = line.ToVector();
    float len = v.Length();
    float startLen = std::max(0.0f, len - inheritanceArrowHeight);
    Vector u = UnitVector(v) * startLen;
    Line startLine(crossPoint, u);
    if (springpp::LineContains(startLine.start, startLine.end, location, selectedLineWidth)) return true;
    return false;
}

bool CombinedInheritance::Contains180(const wing::PointF& location, RelationshipElement* relationshipElement) const
{
    Diagram* diagram = relationshipElement->GetDiagram();
    ClassElement* targetClassElement = nullptr;
    EndPoint& targetEndPoint = relationshipElement->Target();
    if (targetEndPoint.Element() && targetEndPoint.Element()->IsClassElement())
    {
        targetClassElement = static_cast<ClassElement*>(targetEndPoint.Element());
    }
    if (!targetClassElement) return false;
    EndPoint rightCenterEndPoint = targetClassElement->GetEndPoint(Connector(Connector::RightCenter()));
    wing::PointF targetPoint = rightCenterEndPoint.Point();
    targetEndPoint.SetPoint(targetPoint);
    std::vector<int> indeces;
    for (const EndPoint& sourceEndPoint : relationshipElement->SourceEndPoints())
    {
        if (sourceEndPoint.Element() != nullptr)
        {
            if (sourceEndPoint.Element()->IsClassElement())
            {
                int index = diagram->GetIndexOfElement(sourceEndPoint.Element());
                if (index != -1)
                {
                    indeces.push_back(index);
                }
            }
        }
    }
    if (indeces.size() < 2) return false;
    Layout* layout = Configuration::Instance().GetLayout();
    RelationshipLayoutElement* relationshipLayoutElement = layout->GetRelationshipLayoutElement();
    float selectedLineWidth = relationshipLayoutElement->SelectedLineWidth();
    float inheritanceArrowWidth = relationshipLayoutElement->InheritanceArrowWidth();
    float inheritanceArrowHeight = static_cast<float>(std::sqrt(3.0f) / 2.0f * inheritanceArrowWidth);
    std::vector<int> horizontalIndexOrder = indeces;
    std::sort(horizontalIndexOrder.begin(), horizontalIndexOrder.end(), HorizontallyLess(diagram));
    std::vector<ClassElement*> horizontalOrder;
    for (int index : horizontalIndexOrder)
    {
        horizontalOrder.push_back(static_cast<ClassElement*>(diagram->GetElementByIndex(index)));
    }
    std::vector<int> verticalIndexOrder = indeces;
    std::sort(verticalIndexOrder.begin(), verticalIndexOrder.end(), VerticallyLess(diagram));
    std::vector<ClassElement*> verticalOrder;
    for (int index : verticalIndexOrder)
    {
        verticalOrder.push_back(static_cast<ClassElement*>(diagram->GetElementByIndex(index)));
    }
    ClassElement* leftElement = horizontalOrder.front();
    EndPoint leftEndPoint = leftElement->GetEndPoint(Connector(Connector::LeftCenter()));
    wing::PointF leftPoint = leftEndPoint.Point();
    float dx = leftPoint.X - (targetPoint.X + inheritanceArrowHeight);
    float w = dx / 2;
    wing::PointF crossPoint(targetPoint.X + inheritanceArrowHeight + w, targetPoint.Y);
    for (ClassElement* sourceElement : verticalOrder)
    {
        EndPoint sourceEndPoint = sourceElement->GetEndPoint(Connector(Connector::LeftCenter()));
        wing::PointF sourcePoint = sourceEndPoint.Point();
        wing::PointF vpoint(crossPoint.X, sourcePoint.Y);
        if (springpp::LineContains(sourcePoint, vpoint, location, selectedLineWidth)) return true;
        if (springpp::LineContains(vpoint, crossPoint, location, selectedLineWidth)) return true;
    }
    Line line(crossPoint, targetPoint);
    Vector v = line.ToVector();
    float len = v.Length();
    float startLen = std::max(0.0f, len - inheritanceArrowHeight);
    Vector u = UnitVector(v) * startLen;
    Line startLine(crossPoint, u);
    if (springpp::LineContains(startLine.start, startLine.end, location, selectedLineWidth)) return true;
    return false;
}

bool CombinedInheritance::Contains270(const wing::PointF& location, RelationshipElement* relationshipElement) const
{
    Diagram* diagram = relationshipElement->GetDiagram();
    ClassElement* targetClassElement = nullptr;
    EndPoint& targetEndPoint = relationshipElement->Target();
    if (targetEndPoint.Element() && targetEndPoint.Element()->IsClassElement())
    {
        targetClassElement = static_cast<ClassElement*>(targetEndPoint.Element());
    }
    if (!targetClassElement) return false;
    EndPoint bottomCenterEndPoint = targetClassElement->GetEndPoint(Connector(Connector::BottomCenter()));
    wing::PointF targetPoint = bottomCenterEndPoint.Point();
    targetEndPoint.SetPoint(targetPoint);
    std::vector<int> indeces;
    for (const EndPoint& sourceEndPoint : relationshipElement->SourceEndPoints())
    {
        if (sourceEndPoint.Element() != nullptr)
        {
            if (sourceEndPoint.Element()->IsClassElement())
            {
                int index = diagram->GetIndexOfElement(sourceEndPoint.Element());
                if (index != -1)
                {
                    indeces.push_back(index);
                }
            }
        }
    }
    if (indeces.size() < 2) return false;
    Layout* layout = Configuration::Instance().GetLayout();
    RelationshipLayoutElement* relationshipLayoutElement = layout->GetRelationshipLayoutElement();
    float selectedLineWidth = relationshipLayoutElement->SelectedLineWidth();
    float inheritanceArrowWidth = relationshipLayoutElement->InheritanceArrowWidth();
    float inheritanceArrowHeight = static_cast<float>(std::sqrt(3.0f) / 2.0f * inheritanceArrowWidth);
    std::vector<int> horizontalIndexOrder = indeces;
    std::sort(horizontalIndexOrder.begin(), horizontalIndexOrder.end(), HorizontallyLess(diagram));
    std::vector<ClassElement*> horizontalOrder;
    for (int index : horizontalIndexOrder)
    {
        horizontalOrder.push_back(static_cast<ClassElement*>(diagram->GetElementByIndex(index)));
    }
    std::vector<int> verticalIndexOrder = indeces;
    std::sort(verticalIndexOrder.begin(), verticalIndexOrder.end(), VerticallyLess(diagram));
    std::vector<ClassElement*> verticalOrder;
    for (int index : verticalIndexOrder)
    {
        verticalOrder.push_back(static_cast<ClassElement*>(diagram->GetElementByIndex(index)));
    }
    ClassElement* topElement = verticalOrder.front();
    EndPoint topEndPoint = topElement->GetEndPoint(Connector(Connector::TopCenter()));
    wing::PointF topPoint = topEndPoint.Point();
    float dy = topPoint.Y - (targetPoint.Y + inheritanceArrowHeight);
    float h = dy / 2;
    wing::PointF crossPoint(targetPoint.X, targetPoint.Y + inheritanceArrowHeight + h);
    for (ClassElement* sourceElement : horizontalOrder)
    {
        EndPoint sourceEndPoint = sourceElement->GetEndPoint(Connector(Connector::TopCenter()));
        wing::PointF sourcePoint = sourceEndPoint.Point();
        wing::PointF vpoint(sourcePoint.X, crossPoint.Y);
        if (springpp::LineContains(sourcePoint, vpoint, location, selectedLineWidth)) return true;
        if (springpp::LineContains(vpoint, crossPoint, location, selectedLineWidth)) return true;
    }
    Line line(crossPoint, targetPoint);
    Vector v = line.ToVector();
    float len = v.Length();
    float startLen = std::max(0.0f, len - inheritanceArrowHeight);
    Vector u = UnitVector(v) * startLen;
    Line startLine(crossPoint, u);
    if (springpp::LineContains(startLine.start, startLine.end, location, selectedLineWidth)) return true;
    return false;
}

Composition::Composition(RelationshipElement* relationshipElement_) : RelationshipElementRep(relationshipElement_)
{
}

float Composition::SourceSymbolWidth() const
{
    Layout* layout = Configuration::Instance().GetLayout();
    RelationshipLayoutElement* relationshipLayoutElement = layout->GetRelationshipLayoutElement();
    float compositionSymbolWidth = relationshipLayoutElement->CompositeSymbolWidth();
    return compositionSymbolWidth;
}

float Composition::TargetSymbolWidth() const
{
    Layout* layout = Configuration::Instance().GetLayout();
    RelationshipLayoutElement* relationshipLayoutElement = layout->GetRelationshipLayoutElement();
    float cardinalitySymbolRadius = relationshipLayoutElement->CardinalitySymbolRadius();
    float lineArrowWidth = relationshipLayoutElement->LineArrowWidth();
    RelationshipElement* relationshipElement = GetRelationshipElement();
    Cardinality cardinality = relationshipElement->GetCardinality();
    if (cardinality == Cardinality::one)
    {
        return lineArrowWidth;
    }
    else if (cardinality == Cardinality::many)
    {
        return 2 * cardinalitySymbolRadius + lineArrowWidth;
    }
    return 0.0f;
}

RelationshipElementRep* Composition::Clone(RelationshipElement* relationshipElement_) const
{
    Composition* clone = new Composition(relationshipElement_);
    return clone;
}

void Composition::Draw(wing::Graphics& graphics)
{
    RelationshipElement* relationshipElement = GetRelationshipElement();
    Layout* layout = Configuration::Instance().GetLayout();
    RelationshipLayoutElement* relationshipLayoutElement = layout->GetRelationshipLayoutElement();
    ClassLayoutElement* classLayoutElement = layout->GetConcreteClassLayoutElement();
    PaddingElement* classPaddingElement = classLayoutElement->GetPaddingElement();
    float compositionSymbolWidth = relationshipLayoutElement->CompositeSymbolWidth();
    float compositionSymbolHeight = relationshipLayoutElement->CompositeSymbolHeight();
    float cardinalitySymbolRadius = relationshipLayoutElement->CardinalitySymbolRadius();
    float lineArrowWidth = relationshipLayoutElement->LineArrowWidth();
    float lineArrowHeight = relationshipLayoutElement->LineArrowHeight();
    wing::Font* font = relationshipLayoutElement->GetFontElement()->GetFont();
    wing::Brush* textBrush = relationshipLayoutElement->TextColorElement()->GetBrush();
    wing::Brush* arrowBrush = relationshipLayoutElement->GetArrowBrush();
    wing::Pen* linePen = relationshipLayoutElement->LinePen();
    wing::PointF start = relationshipElement->Source().Point();
    wing::PointF end;
    if (relationshipElement->RoutingPoints().empty())
    {
        end = relationshipElement->Target().Point();
    }
    else
    {
        end = relationshipElement->RoutingPoints().front();
    }
    Line compositionLine(start, end);
    Vector v(compositionLine.ToVector());
    Vector u(UnitVector(v) * compositionSymbolWidth);
    Line compositionSymbolLine(start, u);
    Vector h(u * 0.5f);
    Line halfCompositionSymbolLine(start, h);
    Vector l(UnitVector(Rotate(halfCompositionSymbolLine, 90.0f).ToVector()) * (compositionSymbolHeight / 2));
    Vector r(UnitVector(Rotate(halfCompositionSymbolLine, -90.0f).ToVector()) * (compositionSymbolHeight / 2));
    Line leftCompositionSymbolLine(halfCompositionSymbolLine.end, l);
    Line rightCompositionSymbolLine(halfCompositionSymbolLine.end, r);
    std::vector<wing::PointF> compositionPoints;
    compositionPoints.push_back(compositionSymbolLine.start);
    compositionPoints.push_back(leftCompositionSymbolLine.end);
    compositionPoints.push_back(compositionSymbolLine.end);
    compositionPoints.push_back(rightCompositionSymbolLine.end);
    compositionPoints.push_back(compositionSymbolLine.start);
    wing::Brush* compositionSourceBrush = relationshipLayoutElement->CompositeSourceColorElement()->GetBrush();
    graphics.FillPolygon(compositionSourceBrush, compositionPoints.data(), static_cast<int>(compositionPoints.size()));
    graphics.DrawLine(linePen, compositionSymbolLine.start, leftCompositionSymbolLine.end);
    graphics.DrawLine(linePen, leftCompositionSymbolLine.end, compositionSymbolLine.end);
    graphics.DrawLine(linePen, compositionSymbolLine.start, rightCompositionSymbolLine.end);
    graphics.DrawLine(linePen, rightCompositionSymbolLine.end, compositionSymbolLine.end);
    wing::PointF prevPoint = compositionSymbolLine.end;
    for (const wing::PointF& routingPoint : relationshipElement->RoutingPoints())
    {
        graphics.DrawLine(linePen, prevPoint, routingPoint);
        prevPoint = routingPoint;
    }
    Line arrowLine(relationshipElement->Target().Point(), prevPoint);
    Vector av(arrowLine.ToVector());
    if (relationshipElement->GetCardinality() == Cardinality::many)
    {
        Line line = arrowLine;
        Vector v = line.ToVector();
        Vector u(UnitVector(v) * cardinalitySymbolRadius);
        Line toCircleCenter(line.start, u);
        wing::PointF circleCenter(toCircleCenter.end);
        Vector n1(0, -1);
        Vector u1(UnitVector(n1) * cardinalitySymbolRadius);
        Line l1(circleCenter, u1);
        Vector n2(-1, 0);
        Vector u2(UnitVector(n2) * cardinalitySymbolRadius);
        Line l2(l1.end, u2);
        wing::PointF topLeftCorner(l2.end);
        wing::RectF rect(topLeftCorner, wing::SizeF(2.0f * cardinalitySymbolRadius, 2.0f * cardinalitySymbolRadius));
        graphics.FillEllipse(arrowBrush, rect);
        Line circleLine(relationshipElement->Target().Point(), UnitVector(av) * 2.0f * cardinalitySymbolRadius);
        Vector uv(UnitVector(av) * static_cast<float>(std::sqrt(3.0f) / 2.0f * lineArrowWidth));
        Line arrowStartLine(circleLine.end, uv);
        Line arrowEndLine(arrowStartLine.end, circleLine.end);
        Vector la(UnitVector(Rotate(arrowEndLine, 90.0f).ToVector()) * (lineArrowHeight / 2));
        Vector ra(UnitVector(Rotate(arrowEndLine, -90.0f).ToVector()) * (lineArrowHeight / 2));
        Line leftArrowLine(arrowEndLine.start, la);
        Line rightArrowLine(arrowEndLine.start, ra);
        std::vector<wing::PointF> points;
        points.push_back(circleLine.end);
        points.push_back(leftArrowLine.end);
        points.push_back(rightArrowLine.end);
        graphics.FillPolygon(arrowBrush, points.data(), static_cast<int>(points.size()));
        graphics.DrawLine(linePen, circleLine.end, arrowLine.end);
    }
    else if (relationshipElement->GetCardinality() == Cardinality::one)
    {
        Vector uv(UnitVector(av) * static_cast<float>(std::sqrt(3.0f) / 2.0f * lineArrowWidth));
        Line arrowStartLine(relationshipElement->Target().Point(), uv);
        Line arrowEndLine(arrowStartLine.end, relationshipElement->Target().Point());
        Vector la(UnitVector(Rotate(arrowEndLine, 90.0f).ToVector()) * (lineArrowHeight / 2));
        Vector ra(UnitVector(Rotate(arrowEndLine, -90.0f).ToVector()) * (lineArrowHeight / 2));
        Line leftArrowLine(arrowEndLine.start, la);
        Line rightArrowLine(arrowEndLine.start, ra);
        std::vector<wing::PointF> points;
        points.push_back(relationshipElement->Target().Point());
        points.push_back(leftArrowLine.end);
        points.push_back(rightArrowLine.end);
        graphics.FillPolygon(arrowBrush, points.data(), static_cast<int>(points.size()));
        graphics.DrawLine(linePen, arrowEndLine.start, arrowLine.end);
    }
}

Aggregation::Aggregation(RelationshipElement* relationshipElement_) : RelationshipElementRep(relationshipElement_)
{
}

RelationshipElementRep* Aggregation::Clone(RelationshipElement* relationshipElement_) const
{
    Aggregation* clone = new Aggregation(relationshipElement_);
    return clone;
}

float Aggregation::SourceSymbolWidth() const
{
    Layout* layout = Configuration::Instance().GetLayout();
    RelationshipLayoutElement* relationshipLayoutElement = layout->GetRelationshipLayoutElement();
    float aggregationSymbolWidth = relationshipLayoutElement->CompositeSymbolWidth();
    return aggregationSymbolWidth;
}

float Aggregation::TargetSymbolWidth() const
{
    Layout* layout = Configuration::Instance().GetLayout();
    RelationshipLayoutElement* relationshipLayoutElement = layout->GetRelationshipLayoutElement();
    float cardinalitySymbolRadius = relationshipLayoutElement->CardinalitySymbolRadius();
    float lineArrowWidth = relationshipLayoutElement->LineArrowWidth();
    RelationshipElement* relationshipElement = GetRelationshipElement();
    Cardinality cardinality = relationshipElement->GetCardinality();
    if (cardinality == Cardinality::one)
    {
        return lineArrowWidth;
    }
    else if (cardinality == Cardinality::many)
    {
        return 2 * cardinalitySymbolRadius + lineArrowWidth;
    }
    return 0.0f;
}

void Aggregation::Draw(wing::Graphics& graphics)
{
    RelationshipElement* relationshipElement = GetRelationshipElement();
    Layout* layout = Configuration::Instance().GetLayout();
    RelationshipLayoutElement* relationshipLayoutElement = layout->GetRelationshipLayoutElement();
    ClassLayoutElement* classLayoutElement = layout->GetConcreteClassLayoutElement();
    PaddingElement* classPaddingElement = classLayoutElement->GetPaddingElement();
    float aggregateSymbolWidth = relationshipLayoutElement->AggregateSymbolWidth();
    float aggregateSymbolHeight = relationshipLayoutElement->AggregateSymbolHeight();
    float cardinalitySymbolRadius = relationshipLayoutElement->CardinalitySymbolRadius();
    float lineArrowWidth = relationshipLayoutElement->LineArrowWidth();
    float lineArrowHeight = relationshipLayoutElement->LineArrowHeight();
    wing::Font* font = relationshipLayoutElement->GetFontElement()->GetFont();
    wing::Brush* textBrush = relationshipLayoutElement->TextColorElement()->GetBrush();
    wing::Brush* arrowBrush = relationshipLayoutElement->GetArrowBrush();
    wing::Pen* linePen = relationshipLayoutElement->LinePen();
    wing::PointF start = relationshipElement->Source().Point();
    wing::PointF end;
    if (relationshipElement->RoutingPoints().empty())
    {
        end = relationshipElement->Target().Point();
    }
    else
    {
        end = relationshipElement->RoutingPoints().front();
    }
    Line aggregateLine(start, end);
    Vector v(aggregateLine.ToVector());
    Vector u(UnitVector(v) * aggregateSymbolWidth);
    Line aggregateSymbolLine(start, u);
    Vector h(u * 0.5f);
    Line halfAggregateSymbolLine(start, h);
    Vector l(UnitVector(Rotate(halfAggregateSymbolLine, 90.0f).ToVector()) * (aggregateSymbolHeight / 2));
    Vector r(UnitVector(Rotate(halfAggregateSymbolLine, -90.0f).ToVector()) * (aggregateSymbolHeight / 2));
    Line leftAggregateSymbolLine(halfAggregateSymbolLine.end, l);
    Line rightAggregateSymbolLine(halfAggregateSymbolLine.end, r);
    graphics.DrawLine(linePen, aggregateSymbolLine.start, leftAggregateSymbolLine.end);
    graphics.DrawLine(linePen, leftAggregateSymbolLine.end, aggregateSymbolLine.end);
    graphics.DrawLine(linePen, aggregateSymbolLine.start, rightAggregateSymbolLine.end);
    graphics.DrawLine(linePen, rightAggregateSymbolLine.end, aggregateSymbolLine.end);
    wing::PointF prevPoint = aggregateSymbolLine.end;
    for (const wing::PointF& routingPoint : relationshipElement->RoutingPoints())
    {
        graphics.DrawLine(linePen, prevPoint, routingPoint);
        prevPoint = routingPoint;
    }
    Line arrowLine(relationshipElement->Target().Point(), prevPoint);
    Vector av(arrowLine.ToVector());
    if (relationshipElement->GetCardinality() == Cardinality::many)
    {
        Line line = arrowLine;
        Vector v = line.ToVector();
        Vector u(UnitVector(v) * cardinalitySymbolRadius);
        Line toCircleCenter(line.start, u);
        wing::PointF circleCenter(toCircleCenter.end);
        Vector n1(0, -1);
        Vector u1(UnitVector(n1) * cardinalitySymbolRadius);
        Line l1(circleCenter, u1);
        Vector n2(-1, 0);
        Vector u2(UnitVector(n2) * cardinalitySymbolRadius);
        Line l2(l1.end, u2);
        wing::PointF topLeftCorner(l2.end);
        wing::RectF rect(topLeftCorner, wing::SizeF(2.0f * cardinalitySymbolRadius, 2.0f * cardinalitySymbolRadius));
        graphics.FillEllipse(arrowBrush, rect);
        Line circleLine(relationshipElement->Target().Point(), UnitVector(av) * 2.0f * cardinalitySymbolRadius);
        Vector uv(UnitVector(av) * static_cast<float>(std::sqrt(3.0f) / 2.0f * lineArrowWidth));
        Line arrowStartLine(circleLine.end, uv);
        Line arrowEndLine(arrowStartLine.end, circleLine.end);
        Vector la(UnitVector(Rotate(arrowEndLine, 90.0f).ToVector()) * (lineArrowHeight / 2));
        Vector ra(UnitVector(Rotate(arrowEndLine, -90.0f).ToVector()) * (lineArrowHeight / 2));
        Line leftArrowLine(arrowEndLine.start, la);
        Line rightArrowLine(arrowEndLine.start, ra);
        std::vector<wing::PointF> points;
        points.push_back(circleLine.end);
        points.push_back(leftArrowLine.end);
        points.push_back(rightArrowLine.end);
        graphics.FillPolygon(arrowBrush, points.data(), static_cast<int>(points.size()));
        graphics.DrawLine(linePen, circleLine.end, arrowLine.end);
    }
    else if (relationshipElement->GetCardinality() == Cardinality::one)
    {
        Vector uv(UnitVector(av) * static_cast<float>(std::sqrt(3.0f) / 2.0f * lineArrowWidth));
        Line arrowStartLine(relationshipElement->Target().Point(), uv);
        Line arrowEndLine(arrowStartLine.end, relationshipElement->Target().Point());
        Vector la(UnitVector(Rotate(arrowEndLine, 90.0f).ToVector()) * (lineArrowHeight / 2));
        Vector ra(UnitVector(Rotate(arrowEndLine, -90.0f).ToVector()) * (lineArrowHeight / 2));
        Line leftArrowLine(arrowEndLine.start, la);
        Line rightArrowLine(arrowEndLine.start, ra);
        std::vector<wing::PointF> points;
        points.push_back(relationshipElement->Target().Point());
        points.push_back(leftArrowLine.end);
        points.push_back(rightArrowLine.end);
        graphics.FillPolygon(arrowBrush, points.data(), static_cast<int>(points.size()));
        graphics.DrawLine(linePen, arrowEndLine.start, arrowLine.end);
    }
}

Reference::Reference(RelationshipElement* relationshipElement_) : RelationshipElementRep(relationshipElement_)
{
}

RelationshipElementRep* Reference::Clone(RelationshipElement* relationshipElement_) const
{
    Reference* clone = new Reference(relationshipElement_);
    return clone;
}

float Reference::TargetSymbolWidth() const
{
    Layout* layout = Configuration::Instance().GetLayout();
    RelationshipLayoutElement* relationshipLayoutElement = layout->GetRelationshipLayoutElement();
    float cardinalitySymbolRadius = relationshipLayoutElement->CardinalitySymbolRadius();
    float lineArrowWidth = relationshipLayoutElement->LineArrowWidth();
    RelationshipElement* relationshipElement = GetRelationshipElement();
    Cardinality cardinality = relationshipElement->GetCardinality();
    if (cardinality == Cardinality::one)
    {
        return lineArrowWidth;
    }
    else if (cardinality == Cardinality::many)
    {
        return 2 * cardinalitySymbolRadius + lineArrowWidth;
    }
    return 0.0f;
}

void Reference::Draw(wing::Graphics& graphics)
{
    RelationshipElement* relationshipElement = GetRelationshipElement();
    Layout* layout = Configuration::Instance().GetLayout();
    RelationshipLayoutElement* relationshipLayoutElement = layout->GetRelationshipLayoutElement();
    ClassLayoutElement* classLayoutElement = layout->GetConcreteClassLayoutElement();
    PaddingElement* classPaddingElement = classLayoutElement->GetPaddingElement();
    float cardinalitySymbolRadius = relationshipLayoutElement->CardinalitySymbolRadius();
    float lineArrowWidth = relationshipLayoutElement->LineArrowWidth();
    float lineArrowHeight = relationshipLayoutElement->LineArrowHeight();
    float nullReferenceSymbolWidth = relationshipLayoutElement->NullReferenceSymbolWidth();
    float nullReferenceSymbolHeight = relationshipLayoutElement->NullReferenceSymbolHeight();
    wing::Font* font = relationshipLayoutElement->GetFontElement()->GetFont();
    wing::Brush* textBrush = relationshipLayoutElement->TextColorElement()->GetBrush();
    wing::Brush* arrowBrush = relationshipLayoutElement->GetArrowBrush();
    wing::Pen* linePen = relationshipLayoutElement->LinePen();
    wing::PointF start = relationshipElement->Source().Point();
    wing::PointF first = start;
    if (relationshipElement->Source().Element()->IsAttributeElement())
    {
        float relationshipSymbolRadius = relationshipLayoutElement->RelationshipSymbolRadius();
        Vector left(-1, 0);
        Vector ul(UnitVector(left) * relationshipSymbolRadius);
        Line l(start, ul);
        Vector up(0, -1);
        Vector uu(UnitVector(up) * relationshipSymbolRadius);
        Line u(l.end, uu);
        wing::RectF r(u.end, wing::SizeF(2.0f * relationshipSymbolRadius, 2.0f * relationshipSymbolRadius));
        graphics.FillEllipse(arrowBrush, r);
        wing::PointF next;
        if (relationshipElement->RoutingPoints().empty())
        {
            next = relationshipElement->Target().Point();
        }
        else
        {
            next = relationshipElement->RoutingPoints().front();
        }
        Line line(start, next);
        Vector v = line.ToVector();
        Vector n = UnitVector(v) * relationshipSymbolRadius;
        Line f(start, n);
        first = f.end;
    }
    wing::PointF end;
    if (relationshipElement->RoutingPoints().empty())
    {
        end = relationshipElement->Target().Point();
    }
    else
    {
        end = relationshipElement->RoutingPoints().front();
    }
    Line referenceLine(first, end);
    wing::PointF prevPoint = referenceLine.start;
    for (const wing::PointF& routingPoint : relationshipElement->RoutingPoints())
    {
        graphics.DrawLine(linePen, prevPoint, routingPoint);
        prevPoint = routingPoint;
    }
    Line arrowLine(relationshipElement->Target().Point(), prevPoint);
    Vector av(arrowLine.ToVector());
    Cardinality cardinality = relationshipElement->GetCardinality();
    if (cardinality == Cardinality::many)
    {
        Line line = arrowLine;
        Vector v = line.ToVector();
        Vector u(UnitVector(v) * cardinalitySymbolRadius);
        Line toCircleCenter(line.start, u);
        wing::PointF circleCenter(toCircleCenter.end);
        Vector n1(0, -1);
        Vector u1(UnitVector(n1) * cardinalitySymbolRadius);
        Line l1(circleCenter, u1);
        Vector n2(-1, 0);
        Vector u2(UnitVector(n2) * cardinalitySymbolRadius);
        Line l2(l1.end, u2);
        wing::PointF topLeftCorner(l2.end);
        wing::RectF rect(topLeftCorner, wing::SizeF(2.0f * cardinalitySymbolRadius, 2.0f * cardinalitySymbolRadius));
        graphics.FillEllipse(arrowBrush, rect);
        Line circleLine(relationshipElement->Target().Point(), UnitVector(av) * 2.0f * cardinalitySymbolRadius);
        Vector uv(UnitVector(av) * static_cast<float>(std::sqrt(3.0) / 2.0f * lineArrowWidth));
        Line arrowStartLine(circleLine.end, uv);
        Line arrowEndLine(arrowStartLine.end, circleLine.end);
        Vector la(UnitVector(Rotate(arrowEndLine, 90.0f).ToVector()) * (lineArrowHeight / 2));
        Vector ra(UnitVector(Rotate(arrowEndLine, -90.0f).ToVector()) * (lineArrowHeight / 2));
        Line leftArrowLine(arrowEndLine.start, la);
        Line rightArrowLine(arrowEndLine.start, ra);
        std::vector<wing::PointF> points;
        points.push_back(circleLine.end);
        points.push_back(leftArrowLine.end);
        points.push_back(rightArrowLine.end);
        graphics.FillPolygon(arrowBrush, points.data(), static_cast<int>(points.size()));
        graphics.DrawLine(linePen, arrowEndLine.start, arrowLine.end);
    }
    else if (cardinality == Cardinality::one)
    {
        Vector uv(UnitVector(av) * static_cast<float>(std::sqrt(3.0) / 2.0f * lineArrowWidth));
        Line arrowStartLine(relationshipElement->Target().Point(), uv);
        Line arrowEndLine(arrowStartLine.end, relationshipElement->Target().Point());
        Vector la(UnitVector(Rotate(arrowEndLine, 90.0f).ToVector()) * (lineArrowHeight / 2));
        Vector ra(UnitVector(Rotate(arrowEndLine, -90.0f).ToVector()) * (lineArrowHeight / 2));
        Line leftArrowLine(arrowEndLine.start, la);
        Line rightArrowLine(arrowEndLine.start, ra);
        std::vector<wing::PointF> points;
        points.push_back(relationshipElement->Target().Point());
        points.push_back(leftArrowLine.end);
        points.push_back(rightArrowLine.end);
        graphics.FillPolygon(arrowBrush, points.data(), static_cast<int>(points.size()));
        graphics.DrawLine(linePen, arrowEndLine.start, arrowLine.end);
    }
    else if (cardinality == Cardinality::zero)
    {
        Line line(arrowLine.end, relationshipElement->Target().Point());
        Vector v = line.ToVector();
        Vector up = UnitVector(v) * (line.Length() - 2 * nullReferenceSymbolHeight);
        Line prevLine(line.start, up);
        Vector uf = UnitVector(v) * (line.Length() - nullReferenceSymbolHeight);
        Line lastLine(line.start, uf);
        graphics.DrawLine(linePen, prevLine.start, prevLine.end);
        Line prev = Line(prevLine.end, UnitVector(v) * (nullReferenceSymbolWidth / 2));
        Line prevl = Rotate(prev, 90.0f);
        Line prevr = Rotate(prev, -90.0f);
        Line finalLine = Line(lastLine.end, UnitVector(v) * (nullReferenceSymbolWidth / 2));
        Line finall = Rotate(finalLine, 90.0f);
        Line finalr = Rotate(finalLine, -90.0f);
        graphics.DrawLine(linePen, prevl.end, prevr.end);
        graphics.DrawLine(linePen, finall.end, finalr.end);
    }
}

CreateInstance::CreateInstance(RelationshipElement* relationshipElement_) : RelationshipElementRep(relationshipElement_)
{
}

RelationshipElementRep* CreateInstance::Clone(RelationshipElement* relationshipElement_) const
{
    CreateInstance* clone = new CreateInstance(relationshipElement_);
    return clone;
}

float CreateInstance::TargetSymbolWidth() const
{
    Layout* layout = Configuration::Instance().GetLayout();
    RelationshipLayoutElement* relationshipLayoutElement = layout->GetRelationshipLayoutElement();
    float cardinalitySymbolRadius = relationshipLayoutElement->CardinalitySymbolRadius();
    float lineArrowWidth = relationshipLayoutElement->LineArrowWidth();
    RelationshipElement* relationshipElement = GetRelationshipElement();
    Cardinality cardinality = relationshipElement->GetCardinality();
    if (cardinality == Cardinality::one)
    {
        return lineArrowWidth;
    }
    else if (cardinality == Cardinality::many)
    {
        return 2 * cardinalitySymbolRadius + lineArrowWidth;
    }
    return 0.0f;
}

void CreateInstance::Draw(wing::Graphics& graphics)
{
    RelationshipElement* relationshipElement = GetRelationshipElement();
    Layout* layout = Configuration::Instance().GetLayout();
    RelationshipLayoutElement* relationshipLayoutElement = layout->GetRelationshipLayoutElement();
    ClassLayoutElement* classLayoutElement = layout->GetConcreteClassLayoutElement();
    PaddingElement* classPaddingElement = classLayoutElement->GetPaddingElement();
    float cardinalitySymbolRadius = relationshipLayoutElement->CardinalitySymbolRadius();
    float lineArrowWidth = relationshipLayoutElement->LineArrowWidth();
    float lineArrowHeight = relationshipLayoutElement->LineArrowHeight();
    wing::Font* font = relationshipLayoutElement->GetFontElement()->GetFont();
    wing::Brush* textBrush = relationshipLayoutElement->TextColorElement()->GetBrush();
    wing::Brush* arrowBrush = relationshipLayoutElement->GetArrowBrush();
    wing::Pen* linePen = relationshipLayoutElement->LinePen();
    wing::Pen* dashPen = relationshipLayoutElement->DashLinePen();
    wing::PointF start = relationshipElement->Source().Point();
    wing::PointF end;
    if (relationshipElement->RoutingPoints().empty())
    {
        end = relationshipElement->Target().Point();
    }
    else
    {
        end = relationshipElement->RoutingPoints().front();
    }
    Line instanceLine(start, end);
    Vector v(instanceLine.ToVector());
    wing::PointF prevPoint = instanceLine.start;
    if (!relationshipElement->RoutingPoints().empty())
    {
        prevPoint = relationshipElement->RoutingPoints().back();
    }
    wing::PointF finalPoint;
    Line arrowLine(relationshipElement->Target().Point(), prevPoint);
    Vector av(arrowLine.ToVector());
    Cardinality cardinality = relationshipElement->GetCardinality();
    if (cardinality == Cardinality::many)
    {
        Line line = arrowLine;
        Vector v = line.ToVector();
        Vector u(UnitVector(v) * cardinalitySymbolRadius);
        Line toCircleCenter(line.start, u);
        wing::PointF circleCenter(toCircleCenter.end);
        Vector n1(0, -1);
        Vector u1(UnitVector(n1) * cardinalitySymbolRadius);
        Line l1(circleCenter, u1);
        Vector n2(-1, 0);
        Vector u2(UnitVector(n2) * cardinalitySymbolRadius);
        Line l2(l1.end, u2);
        wing::PointF topLeftCorner(l2.end);
        wing::RectF rect(topLeftCorner, wing::SizeF(2.0f * cardinalitySymbolRadius, 2.0f * cardinalitySymbolRadius));
        graphics.FillEllipse(arrowBrush, rect);
        Line circleLine(relationshipElement->Target().Point(), UnitVector(av) * 2.0f * cardinalitySymbolRadius);
        Vector uv(UnitVector(av) * static_cast<float>(std::sqrt(3.0) / 2.0 * lineArrowWidth));
        Line arrowStartLine(circleLine.end, uv);
        Line arrowEndLine(arrowStartLine.end, circleLine.end);
        finalPoint = arrowEndLine.start;
        Vector la(UnitVector(Rotate(arrowEndLine, 90.0f).ToVector()) * (lineArrowHeight / 2));
        Vector ra(UnitVector(Rotate(arrowEndLine, -90.0f).ToVector()) * (lineArrowHeight / 2));
        Line leftArrowLine(arrowEndLine.start, la);
        Line rightArrowLine(arrowEndLine.start, ra);
        std::vector<wing::PointF> points;
        points.push_back(circleLine.end);
        points.push_back(leftArrowLine.end);
        points.push_back(rightArrowLine.end);
        graphics.FillPolygon(arrowBrush, points.data(), static_cast<int>(points.size()));
        finalPoint = arrowEndLine.start;
    }
    else if (cardinality == Cardinality::one)
    {
        Vector uv(UnitVector(av) * static_cast<float>(std::sqrt(3.0) / 2.0 * lineArrowWidth));
        Line arrowStartLine(relationshipElement->Target().Point(), uv);
        Line arrowEndLine(arrowStartLine.end, relationshipElement->Target().Point());
        Vector la(UnitVector(Rotate(arrowEndLine, 90.0f).ToVector()) * (lineArrowHeight / 2));
        Vector ra(UnitVector(Rotate(arrowEndLine, -90.0f).ToVector()) * (lineArrowHeight / 2));
        Line leftArrowLine(arrowEndLine.start, la);
        Line rightArrowLine(arrowEndLine.start, ra);
        std::vector<wing::PointF> points;
        points.push_back(relationshipElement->Target().Point());
        points.push_back(leftArrowLine.end);
        points.push_back(rightArrowLine.end);
        graphics.FillPolygon(arrowBrush, points.data(), static_cast<int>(points.size()));
        finalPoint = arrowEndLine.start;
    }
    std::vector<wing::PointF> points;
    points.push_back(relationshipElement->Source().Point());
    for (const wing::PointF& routingPoint : relationshipElement->RoutingPoints())
    {
        points.push_back(routingPoint);
    }
    points.push_back(finalPoint);
    graphics.DrawLines(dashPen, points.data(), static_cast<int>(points.size()));
}

AttachNote::AttachNote(RelationshipElement* relationshipElement_) : RelationshipElementRep(relationshipElement_)
{
}

RelationshipElementRep* AttachNote::Clone(RelationshipElement* relationshipElement_) const
{
    AttachNote* clone = new AttachNote(relationshipElement_);
    return clone;
}

void AttachNote::Draw(wing::Graphics& graphics)
{
    RelationshipElement* relationshipElement = GetRelationshipElement();
    Layout* layout = Configuration::Instance().GetLayout();
    RelationshipLayoutElement* relationshipLayoutElement = layout->GetRelationshipLayoutElement();
    wing::Pen* linePen = relationshipLayoutElement->LinePen();
    wing::Pen* dotPen = relationshipLayoutElement->DotLinePen();
    float radius = relationshipLayoutElement->RelationshipSymbolRadius();
    wing::PointF start = relationshipElement->Source().Point();
    Vector left(-1, 0);
    Vector ul(UnitVector(left) * radius);
    Line l(start, ul);
    Vector up(0, -1);
    Vector uu(UnitVector(up) * radius);
    Line u(l.end, uu);
    wing::RectF r(u.end, wing::SizeF(2.0f * radius, 2.0f * radius));
    graphics.DrawEllipse(linePen, r);
    wing::PointF next;
    if (relationshipElement->RoutingPoints().empty())
    {
        next = relationshipElement->Target().Point();
    }
    else
    {
        next = relationshipElement->RoutingPoints().front();
    }
    Line line(start, next);
    Vector v = line.ToVector();
    Vector n = UnitVector(v) * radius;
    Line f(start, n);
    wing::PointF first = f.end;
    std::vector<wing::PointF> points;
    points.push_back(first);
    if (relationshipElement->RoutingPoints().empty())
    {
        points.push_back(next);
    }
    else
    {
        for (const wing::PointF& routingPoint : relationshipElement->RoutingPoints())
        {
            points.push_back(routingPoint);
        }
        points.push_back(relationshipElement->Target().Point());
    }
    graphics.DrawLines(dotPen, points.data(), static_cast<int>(points.size()));
}

} // namespace springpp
