// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

module;
#include <Windows.h>
#include <gdiplus.h>
#undef max
#undef min

module springpp.note_element;

import springpp.action;
import springpp.configuration;
import springpp.layout;
import springpp.canvas;
import soul.xml.xpath;

namespace springpp {

NoteElement::NoteElement() : ContainerElement(DiagramElementKind::noteElement)
{
}

soul::xml::Element* NoteElement::ToXml() const
{
    soul::xml::Element* xmlElement = soul::xml::MakeElement("noteElement");
    soul::xml::Element* boundsElement = soul::xml::MakeElement("bounds");
    wing::RectF bounds = Bounds();
    boundsElement->SetAttribute("x", std::to_string(bounds.X));
    boundsElement->SetAttribute("y", std::to_string(bounds.Y));
    boundsElement->SetAttribute("width", std::to_string(bounds.Width));
    boundsElement->SetAttribute("height", std::to_string(bounds.Height));
    xmlElement->AppendChild(boundsElement);
    for (const auto& line : lines)
    {
        soul::xml::Element* lineElement = soul::xml::MakeElement("line");
        lineElement->SetAttribute("text", line);
        xmlElement->AppendChild(lineElement);
    }
    return xmlElement;
}

void NoteElement::Parse(soul::xml::Element* xmlElement)
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
                throw std::runtime_error("XML element 'noteElement' has no 'x' attribute");
            }
            std::string yStr = boundsElement->GetAttribute("y");
            if (!yStr.empty())
            {
                bounds.Y = std::stof(yStr);
            }
            else
            {
                throw std::runtime_error("XML element 'noteElement' has no 'y' attribute");
            }
            std::string widthStr = boundsElement->GetAttribute("width");
            if (!widthStr.empty())
            {
                bounds.Width = std::stof(widthStr);
            }
            else
            {
                throw std::runtime_error("XML element 'noteElement' has no 'width' attribute");
            }
            std::string heightStr = boundsElement->GetAttribute("height");
            if (!heightStr.empty())
            {
                bounds.Height = std::stof(heightStr);
            }
            else
            {
                throw std::runtime_error("XML element 'noteElement' has no 'height' attribute");
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
    std::unique_ptr<soul::xml::xpath::NodeSet> lineNodeSet = soul::xml::xpath::EvaluateToNodeSet("line", xmlElement);
    int n = lineNodeSet->Count();
    for (int i = 0; i < n; ++i)
    {
        soul::xml::Node* node = lineNodeSet->GetNode(i);
        if (node->IsElementNode())
        {
            soul::xml::Element* lineElement = static_cast<soul::xml::Element*>(node);
            std::string textAttribute = lineElement->GetAttribute("text");
            lines.push_back(textAttribute);
        }
    }
}

void NoteElement::SetLines(std::vector<std::string>&& lines_)
{
    lines = std::move(lines_);
}

void NoteElement::Measure(wing::Graphics& graphics)
{
    ContainerElement::Measure(graphics);
    Layout* layout = Configuration::Instance().GetLayout();
    NoteLayoutElement* noteLayoutElement = layout->GetNoteLayoutElement();
    PaddingElement* paddingElement = noteLayoutElement->GetPaddingElement();
    wing::Font* font = noteLayoutElement->GetFontElement()->GetFont();
    wing::SizeF size;
    wing::Size cornerBitmapSize(11, 11);
    wing::SizeF cornerBitmapSizeF(wing::ScreenMetrics::Get().HorizontalPixelsToMM(cornerBitmapSize.Width), wing::ScreenMetrics::Get().VerticalPixelsToMM(cornerBitmapSize.Height));
    size.Height = paddingElement->GetPadding().Vertical();
    lineSize = wing::SizeF();
    int n = static_cast<int>(lines.size());
    for (int i = 0; i < n; ++i)
    {
        const std::string& line = lines[i];
        wing::RectF lineRect = wing::MeasureString(graphics, line, *font, wing::PointF(0, 0), layout->GetStringFormat());
        lineSize.Height = std::max(lineSize.Height, lineRect.Height);
        size.Width = std::max(size.Width, paddingElement->GetPadding().Horizontal() + lineRect.Width + cornerBitmapSizeF.Width);
        size.Height = size.Height + lineSize.Height;
    }
    SetSize(size);
}

void NoteElement::Draw(wing::Graphics& graphics)
{
    ContainerElement::Draw(graphics);
    Canvas* canvas = GetCanvas();
    Layout* layout = Configuration::Instance().GetLayout();
    NoteLayoutElement* noteLayoutElement = layout->GetNoteLayoutElement();
    PaddingElement* paddingElement = noteLayoutElement->GetPaddingElement();
    wing::Font* font = noteLayoutElement->GetFontElement()->GetFont();
    wing::Brush* brush = noteLayoutElement->TextColorElement()->GetBrush();
    wing::Pen* framePen = noteLayoutElement->FramePen();
    wing::PointF location = Location();
    wing::SizeF size = Size();
    wing::Size cornerBitmapSize(13, 13);
    wing::SizeF cornerBitmapSizeF(wing::ScreenMetrics::Get().HorizontalPixelsToMM(cornerBitmapSize.Width), wing::ScreenMetrics::Get().VerticalPixelsToMM(cornerBitmapSize.Height));
    wing::PointF cornerBitmapLocF(location.X + size.Width - cornerBitmapSizeF.Width, location.Y);
    wing::Point cornerBitmapLoc(canvas->ToControlLocation(cornerBitmapLocF));
    Gdiplus::Bitmap* noteCornerBitmap = canvas->NoteCornerBitmap();
    graphics.DrawLine(framePen, location, wing::PointF(location.X + size.Width - cornerBitmapSizeF.Width, location.Y));
    graphics.DrawLine(framePen, wing::PointF(location.X + size.Width, location.Y + cornerBitmapSizeF.Height), wing::PointF(location.X + size.Width, location.Y + size.Height));
    graphics.DrawLine(framePen, location, wing::PointF(location.X, location.Y + size.Height));
    graphics.DrawLine(framePen, wing::PointF(location.X, location.Y + size.Height), wing::PointF(location.X + size.Width, location.Y + size.Height));
    wing::PointF origin(location.X + paddingElement->GetPadding().left, location.Y + paddingElement->GetPadding().top);
    int n = static_cast<int>(lines.size());
    for (int i = 0; i < n; ++i)
    {
        const std::string& line = lines[i];
        wing::DrawString(graphics, line, *font, origin, *brush);
        origin.Y += lineSize.Height;
    }
    Gdiplus::RectF r(cornerBitmapLocF, cornerBitmapSizeF);
    Gdiplus::ImageAttributes imageAttributes;
    wing::Color transparentColor(0u, 128u, 128u);
    imageAttributes.SetColorKey(transparentColor, transparentColor, Gdiplus::ColorAdjustType::ColorAdjustTypeDefault);
    Gdiplus::RectF s(wing::PointF(0, 0), cornerBitmapSizeF);
    wing::SmoothingMode prevSmoothingMode = graphics.GetSmoothingMode();
    graphics.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeNone);
    graphics.DrawImage(noteCornerBitmap, r, s.X, s.Y, s.Width, s.Height, Gdiplus::Unit::UnitMillimeter, &imageAttributes);
    graphics.SetSmoothingMode(prevSmoothingMode);
}

DiagramElement* NoteElement::Clone() const
{
    NoteElement* clone = new NoteElement();
    clone->SetBounds(Bounds());
    clone->lines = lines;
    return clone;
}

void NoteElement::AddActions(Diagram* diagram, int elementIndex, wing::ContextMenu* contextMenu) const
{
    ContainerElement::AddActions(diagram, elementIndex, contextMenu);
    wing::MenuItem* propertiesMenuItem = new wing::MenuItem("Properties...");
    contextMenu->AddMenuItem(propertiesMenuItem);
    contextMenu->AddAction(new NoteElementPropertiesAction(diagram, elementIndex, propertiesMenuItem));
}

std::vector<RelationshipElement*> NoteElement::GetAllRelationships() const
{
    return Relationships();
}

} // namespace springpp
