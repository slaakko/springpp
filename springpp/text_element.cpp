// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

module springpp.text_element;

import springpp.layout;
import springpp.configuration;
import soul.xml.xpath;
import util;

namespace springpp {

TextElement::TextElement() : DiagramElement(DiagramElementKind::textElement), keywordTextSize(), lineSize(), linesSize(), tkind(TextKind::primaryText)
{
}

TextElement::TextElement(const std::string& name_) : DiagramElement(DiagramElementKind::textElement), keywordTextSize(), lineSize(), linesSize(), tkind(TextKind::primaryText)
{
    SetName(name_);
    if (Name() == "primaryText")
    {
        tkind = TextKind::primaryText;
    }
    else if (Name() == "secondaryText")
    {
        tkind = TextKind::secondaryText;
    }
}

bool TextElement::IsEmpty() const
{
    return keyword.empty() && lines.empty();
}

void TextElement::SetLines(std::vector<std::string>&& lines_)
{
    lines = std::move(lines_);
}

void TextElement::SetKeyword(std::string&& keyword_)
{
    keyword = std::move(keyword_);
}

soul::xml::Element* TextElement::ToXml() const
{
    std::string xmlElementName = XmlElementName();
    soul::xml::Element* xmlElement = soul::xml::MakeElement(xmlElementName);
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
    xmlElement->SetAttribute("keyword", keyword);
    return xmlElement;
}

void TextElement::Parse(soul::xml::Element* xmlElement)
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
                throw std::runtime_error("XML element 'bounds' has no 'x' attribute");
            }
            std::string yStr = boundsElement->GetAttribute("y");
            if (!yStr.empty())
            {
                bounds.Y = std::stof(yStr);
            }
            else
            {
                throw std::runtime_error("XML element 'bounds' has no 'y' attribute");
            }
            std::string widthStr = boundsElement->GetAttribute("width");
            if (!widthStr.empty())
            {
                bounds.Width = std::stof(widthStr);
            }
            else
            {
                throw std::runtime_error("XML element 'bounds' has no 'width' attribute");
            }
            std::string heightStr = boundsElement->GetAttribute("height");
            if (!heightStr.empty())
            {
                bounds.Height = std::stof(heightStr);
            }
            else
            {
                throw std::runtime_error("XML element 'bounds' has no 'height' attribute");
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
    keyword = xmlElement->GetAttribute("keyword");
}

DiagramElement* TextElement::Clone() const
{
    TextElement* clone = new TextElement(Name());
    clone->SetBounds(Bounds());
    clone->lines = lines;
    clone->keyword = keyword;
    return clone;
}

void TextElement::Measure(wing::Graphics& graphics)
{
    Layout* layout = Configuration::Instance().GetLayout();
    TextLayoutElement* textLayoutElement = layout->GetTextLayoutElement();
    PaddingElement* paddingElement = textLayoutElement->GetPaddingElement();
    wing::Font* font = textLayoutElement->GetFontElement()->GetFont();
    keywordTextSize = wing::SizeF(0.0f, 0.0f);
    if (!keyword.empty())
    {
        std::u32string keywordU32Text = std::u32string(1, char32_t(0x00AB)) + util::ToUtf32(keyword) + std::u32string(1, char32_t(0x00BB));
        std::string keywordText = util::ToUtf8(keywordU32Text);
        wing::RectF r = wing::MeasureString(graphics, keywordText, *font, wing::PointF(0, 0), layout->GetStringFormat());
        r.GetSize(&keywordTextSize);
    }
    lineSize = wing::SizeF(0.0f, 0.0f);
    linesSize = wing::SizeF(0.0f, 0.0f);
    for (const auto& line : lines)
    {
        wing::RectF r = wing::MeasureString(graphics, line, *font, wing::PointF(0, 0), layout->GetStringFormat());
        wing::SizeF sz;
        r.GetSize(&sz);
        lineSize.Width = std::max(lineSize.Width, sz.Width);
        lineSize.Height = std::max(lineSize.Height, sz.Height);
        linesSize.Height += lineSize.Height;
        linesSize.Width = std::max(linesSize.Width, lineSize.Width);
    }
    wing::SizeF size(0.0f, 0.0f);
    if (!keyword.empty() || !lines.empty())
    {
        size = wing::SizeF(paddingElement->GetPadding().Horizontal() + std::max(keywordTextSize.Width, linesSize.Width),
            paddingElement->GetPadding().Vertical() + keywordTextSize.Height + linesSize.Height);
    }
    SetSize(size);
}

void TextElement::Draw(wing::Graphics& graphics)
{
    Layout* layout = Configuration::Instance().GetLayout();
    TextLayoutElement* textLayoutElement = layout->GetTextLayoutElement();
    PaddingElement* paddingElement = textLayoutElement->GetPaddingElement();
    wing::Font* font = textLayoutElement->GetFontElement()->GetFont();
    wing::Brush* brush = textLayoutElement->GetTextColorElement()->GetBrush();
    wing::PointF location = Location();
    wing::PointF origin(location.X + paddingElement->GetPadding().left, location.Y + paddingElement->GetPadding().top);
    if (!keyword.empty())
    {
        std::u32string keywordU32Text = std::u32string(1, char32_t(0x00AB)) + util::ToUtf32(keyword) + std::u32string(1, char32_t(0x00BB));
        std::string keywordText = util::ToUtf8(keywordU32Text);
        wing::DrawString(graphics, keywordText, *font, origin, *brush);
        origin.Y += keywordTextSize.Height;
    }
    for (const auto& line : lines)
    {
        wing::DrawString(graphics, line, *font, origin, *brush);
        origin.Y += lineSize.Height;
    }
}

void TextElement::SetSourceLocation(const Line& line, float symbolWidth)
{
    switch (tkind)
    {
        case TextKind::primaryText:
        {
            SetPrimarySourceLocation(line, symbolWidth);
            break;
        }
        case TextKind::secondaryText:
        {
            SetSecondarySourceLocation(line, symbolWidth);
            break;
        }
    }
}

void TextElement::SetTargetLocation(const Line& line, float symbolWidth)
{
    switch (tkind)
    {
        case TextKind::primaryText:
        {
            SetPrimaryTargetLocation(line, symbolWidth);
            break;
        }
        case TextKind::secondaryText:
        {
            SetSecondaryTargetLocation(line, symbolWidth);
            break;
        }
    }
}

void TextElement::SetPrimarySourceLocation(const Line& line, float symbolWidth)
{
    if (line.Length() == 0.0f) return;
    Vector u = line.ToVector();
    if (u.x >= 0 && u.y >= 0)
    {
        float h = Size().Height;
        Vector x_unit(1.0f, 0.0f);
        Vector y_unit(0.0f, -1.0f);
        Vector v = Projection(u, x_unit);
        Vector v_minus_u = v - u;
        float r = h * v_minus_u.Length() / Dot(u, u);
        Vector ru = u * r;
        Vector hy = y_unit * h;
        Vector su = UnitVector(u) * symbolWidth;
        Line a(line.start, su);
        Line b(a.end, ru);
        Line c(b.end, hy);
        DiagramElement::SetLocation(c.end);
    }
    else if (u.x > 0 && u.y < 0)
    {
        float w = Size().Width;
        float h = Size().Height;
        Vector x_unit(-1.0f, 0.0f);
        Vector y_unit(0.0f, -1.0f);
        Vector v = Projection(u, x_unit);
        float r = w * v.Length() / Dot(u, u);
        Vector ru = u * r;
        Vector hy = y_unit * h;
        Vector wx = x_unit * w;
        Vector su = UnitVector(u) * symbolWidth;
        Line a(line.start, su);
        Line b(a.end, ru);
        Line c(b.end, hy);
        Line d(c.end, wx);
        DiagramElement::SetLocation(d.end);
    }
    else if (u.x <= 0 && u.y < 0 || u.x < 0 && u.y == 0)
    {
        float w = Size().Width;
        float h = Size().Height;
        Vector x_unit(-1.0f, 0.0f);
        Vector y_unit(0.0f, -1.0f);
        Vector v = Projection(u, x_unit);
        float r = w * v.Length() / Dot(u, u);
        Vector ru = u * r;
        Vector hy = y_unit * h;
        Vector su = UnitVector(u) * symbolWidth;
        Line a(line.start, su);
        Line b(a.end, ru);
        Line c(b.end, hy);
        DiagramElement::SetLocation(c.end);
    }
    else
    {
        float w = Size().Width;
        float h = Size().Height;
        Vector x_unit(-1.0f, 0.0f);
        Vector y_unit(0.0f, -1.0f);
        Vector v = Projection(u, x_unit);
        Vector v_minus_u = v - u;
        float r = h * v_minus_u.Length() / Dot(u, u);
        Vector ru = u * r;
        Vector hy = y_unit * h;
        Vector wx = x_unit * w;
        Vector su = UnitVector(u) * symbolWidth;
        Line a(line.start, su);
        Line b(a.end, ru);
        Line c(b.end, hy);
        Line d(c.end, wx);
        DiagramElement::SetLocation(d.end);
    }
}

void TextElement::SetSecondarySourceLocation(const Line& line, float symbolWidth)
{
    Vector u = line.ToVector();
    if (u.x >= 0 && u.y >= 0)
    {
        float w = Size().Width;
        Vector x_unit(-1.0f, 0.0f);
        Vector y_unit(0.0f, -1.0f);
        Vector v = Projection(u, x_unit);
        float r = w * v.Length() / Dot(u, u);
        Vector ru = u * r;
        Vector wx = x_unit * w;
        Vector su = UnitVector(u) * symbolWidth;
        Line a(line.start, su);
        Line b(a.end, ru);
        Line c(b.end, wx);
        DiagramElement::SetLocation(c.end);
    }
    else if (u.x > 0 && u.y < 0)
    {
        float h = Size().Height;
        Vector x_unit(-1.0f, 0.0f);
        Vector y_unit(0.0f, -1.0f);
        Vector v = Projection(u, x_unit);
        Vector v_minus_u = v - u;
        float r = h * v_minus_u.Length() / Dot(u, u);
        Vector ru = u * r;
        Vector su = UnitVector(u) * symbolWidth;
        Line a(line.start, su);
        Line b(a.end, ru);
        DiagramElement::SetLocation(b.end);
    }
    else if (u.x <= 0 && u.y < 0 || u.x < 0 && u.y == 0)
    {
        float w = Size().Width;
        float h = Size().Height;
        Vector x_unit(-1.0f, 0.0f);
        Vector y_unit(0.0f, -1.0f);
        Vector v = Projection(u, x_unit);
        Vector v_minus_u = v - u;
        float r = h * v_minus_u.Length() / Dot(u, u);
        Vector ru = u * r;
        Vector su = UnitVector(u) * symbolWidth;
        Vector wx = x_unit * w;
        Line a(line.start, su);
        Line b(a.end, ru);
        Line c(b.end, wx);
        DiagramElement::SetLocation(c.end);
    }
    else
    {
        float w = Size().Width;
        Vector x_unit(-1.0f, 0.0f);
        Vector v = Projection(u, x_unit);
        float r = w * v.Length() / Dot(u, u);
        Vector ru = u * r;
        Vector su = UnitVector(u) * symbolWidth;
        Line a(line.start, su);
        Line b(a.end, ru);
        DiagramElement::SetLocation(b.end);
    }
}

void TextElement::SetPrimaryTargetLocation(const Line& line, float symbolWidth)
{
    if (line.Length() == 0.0f) return;
    Line reverseLine(line.end, line.start);
    Vector u = reverseLine.ToVector();
    if (u.x >= 0 && u.y >= 0)
    {
        float h = Size().Height;
        Vector x_unit(1.0f, 0.0f);
        Vector y_unit(0.0f, -1.0f);
        Vector v = Projection(u, x_unit);
        Vector v_minus_u = v - u;
        float r = h * v_minus_u.Length() / Dot(u, u);
        Vector ru = u * r;
        Vector hy = y_unit * h;
        Vector su = UnitVector(u) * symbolWidth;
        Line a(reverseLine.start, su);
        Line b(a.end, ru);
        Line c(b.end, hy);
        DiagramElement::SetLocation(c.end);
    }
    else if (u.x > 0 && u.y < 0)
    {
        float w = Size().Width;
        float h = Size().Height;
        Vector x_unit(-1.0f, 0.0f);
        Vector y_unit(0.0f, -1.0f);
        Vector v = Projection(u, x_unit);
        float r = w * v.Length() / Dot(u, u);
        Vector ru = u * r;
        Vector hy = y_unit * h;
        Vector wx = x_unit * w;
        Vector su = UnitVector(u) * symbolWidth;
        Line a(reverseLine.start, su);
        Line b(a.end, ru);
        Line c(b.end, hy);
        Line d(c.end, wx);
        DiagramElement::SetLocation(d.end);
    }
    else if (u.x <= 0 && u.y < 0 || u.x < 0 && u.y == 0)
    {
        float w = Size().Width;
        float h = Size().Height;
        Vector x_unit(-1.0f, 0.0f);
        Vector y_unit(0.0f, -1.0f);
        Vector v = Projection(u, x_unit);
        float r = w * v.Length() / Dot(u, u);
        Vector ru = u * r;
        Vector hy = y_unit * h;
        Vector su = UnitVector(u) * symbolWidth;
        Line a(reverseLine.start, su);
        Line b(a.end, ru);
        Line c(b.end, hy);
        DiagramElement::SetLocation(c.end);
    }
    else
    {
        float w = Size().Width;
        float h = Size().Height;
        Vector x_unit(-1.0f, 0.0f);
        Vector y_unit(0.0f, -1.0f);
        Vector v = Projection(u, x_unit);
        Vector v_minus_u = v - u;
        float r = h * v_minus_u.Length() / Dot(u, u);
        Vector ru = u * r;
        Vector hy = y_unit * h;
        Vector wx = x_unit * w;
        Vector su = UnitVector(u) * symbolWidth;
        Line a(reverseLine.start, su);
        Line b(a.end, ru);
        Line c(b.end, hy);
        Line d(c.end, wx);
        DiagramElement::SetLocation(d.end);
    }
}

void TextElement::SetSecondaryTargetLocation(const Line& line, float symbolWidth)
{
    if (line.Length() == 0.0f) return;
    Line reverseLine(line.end, line.start);
    Vector u = reverseLine.ToVector();
    if (u.x >= 0 && u.y >= 0)
    {
        float w = Size().Width;
        Vector x_unit(-1.0f, 0.0f);
        Vector y_unit(0.0f, -1.0f);
        Vector v = Projection(u, x_unit);
        float r = w * v.Length() / Dot(u, u);
        Vector ru = u * r;
        Vector wx = x_unit * w;
        Vector su = UnitVector(u) * symbolWidth;
        Line a(reverseLine.start, su);
        Line b(a.end, ru);
        Line c(b.end, wx);
        DiagramElement::SetLocation(c.end);
    }
    else if (u.x > 0 && u.y < 0)
    {
        float h = Size().Height;
        Vector x_unit(-1.0f, 0.0f);
        Vector y_unit(0.0f, -1.0f);
        Vector v = Projection(u, x_unit);
        Vector v_minus_u = v - u;
        float r = h * v_minus_u.Length() / Dot(u, u);
        Vector ru = u * r;
        Vector su = UnitVector(u) * symbolWidth;
        Line a(reverseLine.start, su);
        Line b(a.end, ru);
        DiagramElement::SetLocation(b.end);
    }
    else if (u.x <= 0 && u.y < 0 || u.x < 0 && u.y == 0)
    {
        float w = Size().Width;
        float h = Size().Height;
        Vector x_unit(-1.0f, 0.0f);
        Vector y_unit(0.0f, -1.0f);
        Vector v = Projection(u, x_unit);
        Vector v_minus_u = v - u;
        float r = h * v_minus_u.Length() / Dot(u, u);
        Vector ru = u * r;
        Vector su = UnitVector(u) * symbolWidth;
        Vector wx = x_unit * w;
        Line a(reverseLine.start, su);
        Line b(a.end, ru);
        Line c(b.end, wx);
        DiagramElement::SetLocation(c.end);
    }
    else
    {
        float w = Size().Width;
        Vector x_unit(-1.0f, 0.0f);
        Vector v = Projection(u, x_unit);
        float r = w * v.Length() / Dot(u, u);
        Vector ru = u * r;
        Vector su = UnitVector(u) * symbolWidth;
        Line a(reverseLine.start, su);
        Line b(a.end, ru);
        DiagramElement::SetLocation(b.end);
    }
}

} // namespace springpp

