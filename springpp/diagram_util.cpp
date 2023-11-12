// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

module springpp.diagram_util;

import springpp.diagram;
import springpp.container_element;
import springpp.relationship_element;
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

} // namespace springpp
