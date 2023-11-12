// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

module springpp.relationships;

import springpp.configuration;

namespace springpp {

Inheritance::Inheritance(RelationshipElement* relationshipElement_) : RelationshipElementRep(relationshipElement_)
{
}

RelationshipElementRep* Inheritance::Clone(RelationshipElement* relationshipElement_) const 
{
    Inheritance* clone = new Inheritance(relationshipElement_);
    return clone;
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
    if (relationshipElement->IntermediatePoints().empty())
    {
        lastPoint = relationshipElement->Source().Point();
    }
    else
    {
        wing::PointF prevPoint = relationshipElement->Source().Point();
        for (const auto& intermediatePoint : relationshipElement->IntermediatePoints())
        {
            graphics.DrawLine(linePen, prevPoint, intermediatePoint);
            prevPoint = intermediatePoint;
        }
        lastPoint = relationshipElement->IntermediatePoints().back();
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

Composition::Composition(RelationshipElement* relationshipElement_) : RelationshipElementRep(relationshipElement_)
{
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
    if (relationshipElement->IntermediatePoints().empty())
    {
        end = relationshipElement->Target().Point();
    }
    else
    {
        end = relationshipElement->IntermediatePoints().front();
    }
    Line compositionLine(start, end);
    if (!relationshipElement->Source().Text().empty())
    {
        float leadinglWidth = compositionSymbolWidth;
        Line textLine = GetSourceTextLine(compositionLine, leadinglWidth, classPaddingElement->GetPadding().left);
        DrawSourceText(graphics, font, textBrush, textLine, leadinglWidth);
    }
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
    for (const wing::PointF& intermediatePoint : relationshipElement->IntermediatePoints())
    {
        graphics.DrawLine(linePen, prevPoint, intermediatePoint);
        prevPoint = intermediatePoint;
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
        if (!relationshipElement->Target().Text().empty())
        {
            DrawTargetText(graphics, font, textBrush, arrowEndLine, lineArrowWidth);
        }
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
        if (!relationshipElement->Target().Text().empty())
        {
            DrawTargetText(graphics, font, textBrush, arrowEndLine, lineArrowWidth);
        }
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
    if (relationshipElement->IntermediatePoints().empty())
    {
        end = relationshipElement->Target().Point();
    }
    else
    {
        end = relationshipElement->IntermediatePoints().front();
    }
    Line aggregateLine(start, end);
    if (!relationshipElement->Source().Text().empty())
    {
        float leadinglWidth = aggregateSymbolWidth;
        Line textLine = GetSourceTextLine(aggregateLine, leadinglWidth, classPaddingElement->GetPadding().left);
        DrawSourceText(graphics, font, textBrush, textLine, leadinglWidth);
    }
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
    for (const wing::PointF& intermediatePoint : relationshipElement->IntermediatePoints())
    {
        graphics.DrawLine(linePen, prevPoint, intermediatePoint);
        prevPoint = intermediatePoint;
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
        if (!relationshipElement->Target().Text().empty())
        {
            DrawTargetText(graphics, font, textBrush, arrowEndLine, lineArrowWidth);
        }
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
        if (!relationshipElement->Target().Text().empty())
        {
            DrawTargetText(graphics, font, textBrush, arrowEndLine, lineArrowWidth);
        }
    }
}

} // namespace springpp
