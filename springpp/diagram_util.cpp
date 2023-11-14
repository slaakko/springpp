// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

module springpp.diagram_util;

import springpp.diagram;
import springpp.class_element;
import springpp.container_element;
import util;

namespace springpp {

wing::RectF CalculateBoundingRect(const std::vector<DiagramElement*>& diagramElements)
{
    wing::RectF boundingRect;
    for (DiagramElement* diagramElement : diagramElements)
    {
        if (boundingRect.IsEmptyArea())
        {
            boundingRect = diagramElement->BoundingRect();
        }
        else
        {
            wing::RectF::Union(boundingRect, boundingRect, diagramElement->BoundingRect());
        }
    }
    return boundingRect;
}

void SaveImage(const std::string& imageFileName, const Padding& margins, Canvas* canvas, const std::vector<DiagramElement*>& diagramElements, wing::Graphics* graphics,
    wing::ImageFormat imageFormat)
{
    Diagram diagram;
    diagram.SetCanvas(canvas);
    std::map<DiagramElement*, DiagramElement*> cloneMap;
    std::vector<RelationshipElement*> relationships;
    for (auto diagramElement : diagramElements)
    {
        DiagramElement* clone = diagramElement->Clone();
        if (clone->IsContainerElement())
        {
            ContainerElement* containerElement = static_cast<ContainerElement*>(clone);
            cloneMap[diagramElement] = clone;
            containerElement->MapChildObjects(static_cast<ContainerElement*>(diagramElement), cloneMap);
        }
        else if (clone->IsRelationshipElement())
        {
            relationships.push_back(static_cast<RelationshipElement*>(clone));
        }
        diagram.AddElement(clone);
    }
    for (auto relationship : relationships)
    {
        relationship->MapContainerElements(cloneMap);
        relationship->AddToElements();
    }
    wing::RectF boundingRect = diagram.CalculateBoundingRect();
    diagram.Offset(-boundingRect.X + margins.left, -boundingRect.Y + margins.top);
    boundingRect.Width += margins.Horizontal();
    boundingRect.Height += margins.Vertical();
    int width = wing::ScreenMetrics::Get().MMToHorizontalPixels(boundingRect.Width);
    int height = wing::ScreenMetrics::Get().MMToVerticalPixels(boundingRect.Height);
    wing::Bitmap bitmap(width, height, graphics);
    wing::Graphics* bitmapGraphics = wing::Graphics::FromImage(&bitmap);
    bitmapGraphics->SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);
    bitmapGraphics->SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
    bitmapGraphics->SetPageUnit(Gdiplus::UnitMillimeter);
    bitmapGraphics->Clear(wing::Color::White);
    diagram.Draw(*bitmapGraphics);
    std::u16string fileName = util::ToUtf16(imageFileName);
    CLSID guid;
    int result = wing::GetEncoderClsId(imageFormat, guid);
    if (result == 0)
    {
        bitmap.Save((const WCHAR*)fileName.c_str(), &guid, nullptr);
    }
    else
    {
        throw std::runtime_error("Spring++: encoder CLSID not found for encoding '" + wing::GetImageFormatStr(imageFormat) + "'");
    }
}

int MainDirection(DiagramElement* source, DiagramElement* target)
{
    wing::PointF sourceCenter = source->Center();
    wing::PointF targetCenter = target->Center();
    Line line(sourceCenter, targetCenter);
    Vector v = line.ToVector();
    return MainDirection(v);
}

HorizontallyLess::HorizontallyLess(Diagram* diagram_) : diagram(diagram_)
{
}

bool HorizontallyLess::operator()(int leftIndex, int rightIndex) const
{
    DiagramElement* left = diagram->GetElementByIndex(leftIndex);
    DiagramElement* right = diagram->GetElementByIndex(rightIndex);
    if (left->Location().X < right->Location().X)
    {
        return true;
    }
    else if (left->Location().X > right->Location().X)
    {
        return false;
    }
    else
    {
        return left->Location().Y < right->Location().Y;
    }
}

VerticallyLess::VerticallyLess(Diagram* diagram_) : diagram(diagram_)
{
}

bool VerticallyLess::operator()(int leftIndex, int rightIndex) const
{
    DiagramElement* left = diagram->GetElementByIndex(leftIndex);
    DiagramElement* right = diagram->GetElementByIndex(rightIndex);
    if (left->Location().Y < right->Location().Y)
    {
        return true;
    }
    else if (left->Location().Y > right->Location().Y)
    {
        return false;
    }
    else
    {
        return left->Location().X < right->Location().X;
    }
}

std::unique_ptr<RelationshipElement> CombineInheritanceRelationships(const std::vector<RelationshipElement*>& inheritanceRelationships,
    std::vector<RelationshipElement*>& sourceRelationships)
{
    std::unique_ptr<RelationshipElement> combinedRelationship;
    std::map<DiagramElement*, int> targetMap;
    for (RelationshipElement* relationship : inheritanceRelationships)
    {
        if (relationship->Target().Element() != nullptr)
        {
            ++targetMap[relationship->Target().Element()];
        }
    }
    int maxCount = -1;
    DiagramElement* maxTargetElement = nullptr;
    for (const std::pair<DiagramElement*, int>& p : targetMap)
    {
        if (p.second > maxCount)
        {
            maxCount = p.second;
            maxTargetElement = p.first;
        }
    }
    if (maxTargetElement && maxTargetElement->IsClassElement())
    {
        EndPoint targetEndPoint;
        std::vector<EndPoint> sourceEndPoints;
        ClassElement* targetClassElement = static_cast<ClassElement*>(maxTargetElement);
        for (RelationshipElement* relationship : inheritanceRelationships)
        {
            if (relationship->Target().Element() != nullptr)
            {
                if (relationship->Target().Element() == targetClassElement)
                {
                    if (targetEndPoint.Element() == nullptr)
                    {
                        targetEndPoint = relationship->Target();
                    }
                    if (relationship->Source().Element() != nullptr)
                    {
                        if (relationship->Source().Element()->IsClassElement())
                        {
                            sourceEndPoints.push_back(relationship->Source());
                            sourceRelationships.push_back(relationship);
                        }
                    }
                }
            }
        }
        if (sourceRelationships.size() > 1)
        {
            combinedRelationship.reset(new RelationshipElement(RelationshipKind::combinedInheritance));
            combinedRelationship->SetCardinality(Cardinality::one);
            combinedRelationship->SourceEndPoints() = sourceEndPoints;
            combinedRelationship->Target() = targetEndPoint;
        }
    }
    return combinedRelationship;
}

std::vector<std::unique_ptr<RelationshipElement>> SplitCombinedInheritanceRelationship(RelationshipElement* combinedRelationship)
{
    std::vector<std::unique_ptr<RelationshipElement>> relationships;
    EndPoint target = combinedRelationship->Target();
    for (const EndPoint& source : combinedRelationship->SourceEndPoints())
    {
        std::unique_ptr<RelationshipElement> relationship(new RelationshipElement(RelationshipKind::inheritance));
        relationship->Source() = source;
        relationship->Target() = target;
        relationships.push_back(std::move(relationship));
    }
    combinedRelationship->RemoveFromElements();
    for (const std::unique_ptr<RelationshipElement>& relationship : relationships)
    {
        relationship->AddToElements();
    }
    return relationships;
}

bool LineContains(const wing::PointF& from, const wing::PointF& to, const wing::PointF& loc, float selectedLineWidth)
{
    Line line(from, to);
    Line lineToLoc(from, loc);
    Vector a(line.ToVector());
    Vector b(lineToLoc.ToVector());
    float f = ProjectionFactor(b, a);
    if (f >= 0 && f <= a.Length())
    {
        Vector c = Projection(b, a);
        Vector d = b - c;
        if (d.Length() <= selectedLineWidth / 2)
        {
            return true;
        }
    }
    return false;
}

} // namespace springpp
