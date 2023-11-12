// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

module;
#include <Windows.h>
#include <gdiplus.h>
#undef max
#undef min

module springpp.layout;

import soul.xml.xpath;
import soul.xml.dom.parser;
import util;
import std.filesystem;

namespace springpp {

Padding::Padding() : left(0.0f), top(0.0f), right(0.0f), bottom(0.0f)
{
}

Padding::Padding(float left_, float top_, float right_, float bottom_) : left(left_), top(top_), right(right_), bottom(bottom_)
{
}

std::string DefaultCaptionFontFamily()
{
    return "Arial";
}

std::string DefaultOperationFontFamily()
{
    return "Arial";
}

std::string DefaultFieldFontFamily()
{
    return "Arial";
}

std::string DefaultRelationshipFontFamily()
{
    return "Arial";
}

std::string DefaultNoteFontFamily()
{
    return "Arial";
}

float DefaultFontSize()
{
    return 9.0f;
}

wing::FontStyle DefaultAbstractClassCaptionFontStyle()
{
    return wing::FontStyle(Gdiplus::FontStyleBoldItalic);
}

wing::FontStyle DefaultConcreteClassCaptionFontStyle()
{
    return wing::FontStyle(Gdiplus::FontStyleBold);
}

wing::FontStyle DefaultConcreteOperationFontStyle()
{
    return wing::FontStyle(Gdiplus::FontStyleRegular);
}

wing::FontStyle DefaultAbstractOperationFontStyle()
{
    return wing::FontStyle(Gdiplus::FontStyleItalic);
}

wing::FontStyle DefaultFieldFontStyle()
{
    return wing::FontStyle(Gdiplus::FontStyleRegular);
}

wing::FontStyle DefaultDefaultRelationshipFontStyle()
{
    return wing::FontStyle(Gdiplus::FontStyleRegular);
}

float DefaultFrameWidth(wing::Graphics* graphics)
{
    return wing::PixelsToMM(1, std::max(graphics->GetDpiX(), graphics->GetDpiY()));
}

float DefaultLineWidth(wing::Graphics* graphics)
{
    return wing::PixelsToMM(1, std::max(graphics->GetDpiX(), graphics->GetDpiY()));
}

float DefaultFrameRoundingRadius()
{
    return 3.0f;
}

Padding  DefaultDiagramPadding()
{
    return Padding(5.0f, 5.0f, 5.0f, 5.0f);
}

Padding DefaultClassPadding()
{
    return Padding(1.5f, 1.5f, 1.5f, 1.5f);
}

Padding DefaultObjectPadding()
{
    return Padding(1.5f, 1.5f, 1.5f, 1.5f);
}

Padding DefaultNotePadding()
{
    return Padding(1.5f, 1.5f, 1.5f, 1.5f);
}

Padding DefaultRelationshipSymbolPadding()
{
    return Padding(0.5f, 0.5f, 0.5f, 0.5f);
}

float DefaultInheritanceArrowWidth()
{
    return 5.0f;
}

float DefaultCompositeSymbolWidth()
{
    return 3.0f;
}

float DefaultCompositeSymbolHeight()
{
    return 2.0f;
}

float DefaultAggregateSymbolWidth()
{
    return 3.0f;
}

float DefaultAggregateSymbolHeight()
{
    return 2.0f;
}

float DefaultLineArrowWidth()
{
    return 3.0f;
}

float DefaultLineArrowHeight()
{
    return 2.0f;
}

float DefaultCardinalitySymbolRadius()
{
    return 1.0f;
}

float DefaultRelationshipSymbolRadius()
{
    return 0.75f;
}

float DefaultSelectedLineWidth()
{
    return 4.0f;
}

float DefaultNullReferenceSymbolWidth()
{
    return 3.0f;
}

float DefaultNullReferenceSymbolHeight()
{
    return 1.0f;
}

wing::Color DefaultTextColor()
{
    return wing::Color::Black;
}

wing::Color DefaultFrameColor()
{
    return wing::Color::Black;
}

wing::Color DefaultLineColor()
{
    return wing::Color::Black;
}

wing::Color DefaultSelectionColor()
{
    return wing::Color(64u, 96u, 156u, 210u);
}

wing::Color DefaultArrowColor()
{
    return wing::Color::Black;
}

wing::Color DefaultCompositeSourceColor()
{
    return wing::Color::Black;
}

float DefaultResizeHandleWidth()
{
    return 1.0f;
}

wing::Color DefaultResizeHandleLineColor()
{
    return wing::Color::Black;
}

wing::Color DefaultResizeHandleInsideColor()
{
    return wing::Color::White;
}

std::string SpringPPDirectory()
{
    return util::GetFullPath(util::SpringPPRoot());
}

std::string LayoutDirectory()
{
    std::string layoutDirectory = util::Path::Combine(SpringPPDirectory(), "layout");
    std::filesystem::create_directories(layoutDirectory);
    return layoutDirectory;
}

std::string DiagramDirectory()
{
    std::string diagramDirectory = util::Path::Combine(SpringPPDirectory(), "diagrams");
    std::filesystem::create_directories(diagramDirectory);
    return diagramDirectory;
}

LayoutElement::LayoutElement(Layout* layout_, const std::string& name_) : layout(layout_), name(name_)
{
}

LayoutElement::~LayoutElement()
{
}

ColorElement::ColorElement(Layout* layout_, const std::string& name_, const wing::Color& color_) : LayoutElement(layout_, name_), color(color_), brush(nullptr)
{
}

soul::xml::Element* ColorElement::ToXml() const
{
    soul::xml::Element* xmlElement = soul::xml::MakeElement(Name());
    xmlElement->SetAttribute("value", wing::ColorStr(color));
    return xmlElement;
}

void ColorElement::Parse(soul::xml::Element* parentXmlElement)
{
    std::unique_ptr<soul::xml::xpath::NodeSet> nodeSet = soul::xml::xpath::EvaluateToNodeSet(Name(), parentXmlElement);
    int n = nodeSet->Count();
    if (n == 1)
    {
        soul::xml::Node* node = nodeSet->GetNode(0);
        if (node->IsElementNode())
        {
            soul::xml::Element* xmlElement = static_cast<soul::xml::Element*>(node);
            std::string value = xmlElement->GetAttribute("value");
            if (!value.empty())
            {
                color = wing::ParseColorStr(value);
            }
            else
            {
                throw std::runtime_error("XML element '" + Name() + "' has no 'value' attribute");
            }
        }
        else
        {
            throw std::runtime_error("XML element node '" + Name() + "' expected in '" + parentXmlElement->Name() + "'");
        }
    }
    else
    {
        throw std::runtime_error("XML element '" + Name() + "' not unique in '" + parentXmlElement->Name() + "'");
    }
}

wing::Brush* ColorElement::GetBrush()
{
    if (!brush)
    {
        brush = GetLayout()->GetOrInsertBrush(color);
    }
    return brush;
}

FontElement::FontElement(Layout* layout_, const std::string& name_, const std::string& family_, float size_, wing::FontStyle style_) : 
    LayoutElement(layout_, name_), family(family_), size(size_), style(style_), font(nullptr)
{
}

soul::xml::Element* FontElement::ToXml() const
{
    soul::xml::Element* xmlElement = soul::xml::MakeElement(Name());
    xmlElement->SetAttribute("family", family);
    xmlElement->SetAttribute("size", std::to_string(size));
    xmlElement->SetAttribute("style", wing::FontStyleStr(style));
    return xmlElement;
}

void FontElement::Parse(soul::xml::Element* parentXmlElement)
{
    std::unique_ptr<soul::xml::xpath::NodeSet> nodeSet = soul::xml::xpath::EvaluateToNodeSet(Name(), parentXmlElement);
    int n = nodeSet->Count();
    if (n == 1)
    {
        soul::xml::Node* node = nodeSet->GetNode(0);
        if (node->IsElementNode())
        {
            soul::xml::Element* xmlElement = static_cast<soul::xml::Element*>(node);
            family = xmlElement->GetAttribute("family");
            if (family.empty())
            {
                throw std::runtime_error("XML element '" + Name() + "' has no 'family' attribute");
            }
            std::string sizeStr = xmlElement->GetAttribute("size");
            if (sizeStr.empty())
            {
                throw std::runtime_error("XML element '" + Name() + "' has no 'size' attribute");
            }
            size = std::stof(sizeStr);
            std::string styleStr = xmlElement->GetAttribute("style");
            if (styleStr.empty())
            {
                throw std::runtime_error("XML element '" + Name() + "' has no 'style' attribute");
            }
            style = wing::ParseFontStyle(styleStr);
        }
        else
        {
            throw std::runtime_error("XML element node '" + Name() + "' expected in '" + parentXmlElement->Name() + "'");
        }
    }
    else
    {
        throw std::runtime_error("XML element '" + Name() + "' not unique in '" + parentXmlElement->Name() + "'");
    }
}

wing::Font* FontElement::GetFont()
{
    if (!font)
    {
        font = GetLayout()->GetOrInsertFont(family, size, style);
    }
    return font;
}

PaddingElement::PaddingElement(Layout* layout_, const std::string& name_, const Padding& padding_) : LayoutElement(layout_, name_), padding(padding_)
{
}

soul::xml::Element* PaddingElement::ToXml() const 
{
    soul::xml::Element* xmlElement = soul::xml::MakeElement(Name());
    xmlElement->SetAttribute("left", std::to_string(padding.left));
    xmlElement->SetAttribute("top", std::to_string(padding.top));
    xmlElement->SetAttribute("right", std::to_string(padding.right));
    xmlElement->SetAttribute("bottom", std::to_string(padding.bottom));
    return xmlElement;
}

void PaddingElement::Parse(soul::xml::Element* parentXmlElement)
{
    std::unique_ptr<soul::xml::xpath::NodeSet> nodeSet = soul::xml::xpath::EvaluateToNodeSet(Name(), parentXmlElement);
    int n = nodeSet->Count();
    if (n == 1)
    {
        soul::xml::Node* node = nodeSet->GetNode(0);
        if (node->IsElementNode())
        {
            soul::xml::Element* xmlElement = static_cast<soul::xml::Element*>(node);
            std::string leftStr = xmlElement->GetAttribute("left");
            if (leftStr.empty())
            {
                throw std::runtime_error("XML element '" + Name() + "' has no 'left' attribute");
            }
            padding.left = std::stof(leftStr);
            std::string topStr = xmlElement->GetAttribute("top");
            if (topStr.empty())
            {
                throw std::runtime_error("XML element '" + Name() + "' has no 'top' attribute");
            }
            padding.top = std::stof(topStr);
            std::string rightStr = xmlElement->GetAttribute("right");
            if (rightStr.empty())
            {
                throw std::runtime_error("XML element '" + Name() + "' has no 'right' attribute");
            }
            padding.right = std::stof(rightStr);
            std::string bottomStr = xmlElement->GetAttribute("bottom");
            if (bottomStr.empty())
            {
                throw std::runtime_error("XML element '" + Name() + "' has no 'bottom' attribute");
            }
            padding.bottom = std::stof(bottomStr);
        }
        else
        {
            throw std::runtime_error("XML element node '" + Name() + "' expected in '" + parentXmlElement->Name() + "'");
        }
    }
    else
    {
        throw std::runtime_error("XML element '" + Name() + "' not unique in '" + parentXmlElement->Name() + "'");
    }
}

CaptionElement::CaptionElement(Layout* layout_, FontElement* fontElement_) : LayoutElement(layout_, "caption"), fontElement(fontElement_)
{
}

soul::xml::Element* CaptionElement::ToXml() const
{
    soul::xml::Element* xmlElement = soul::xml::MakeElement(Name());
    xmlElement->AppendChild(fontElement->ToXml());
    return xmlElement;
}

void CaptionElement::Parse(soul::xml::Element* parentXmlElement)
{
    std::unique_ptr<soul::xml::xpath::NodeSet> nodeSet = soul::xml::xpath::EvaluateToNodeSet(Name(), parentXmlElement);
    int n = nodeSet->Count();
    if (n == 1)
    {
        soul::xml::Node* node = nodeSet->GetNode(0);
        if (node->IsElementNode())
        {
            soul::xml::Element* xmlElement = static_cast<soul::xml::Element*>(node);
            fontElement->Parse(xmlElement);
        }
        else
        {
            throw std::runtime_error("XML element node '" + Name() + "' expected in '" + parentXmlElement->Name() + "'");
        }
    }
    else
    {
        throw std::runtime_error("XML element '" + Name() + "' not unique in '" + parentXmlElement->Name() + "'");
    }
}

ClassLayoutElement::ClassLayoutElement(Layout* layout_, const std::string& name_, 
    CaptionElement* captionElement_, PaddingElement* paddingElement_, ColorElement* textColorElement_, ColorElement* frameColorElement_, float frameWidth_) : 
    LayoutElement(layout_, name_), captionElement(captionElement_), paddingElement(paddingElement_), textColorElement(textColorElement_), 
    frameColorElement(frameColorElement_), frameWidth(frameWidth_), framePen(nullptr)
{
}

soul::xml::Element* ClassLayoutElement::ToXml() const
{
    soul::xml::Element* xmlElement = soul::xml::MakeElement(Name());
    xmlElement->AppendChild(captionElement->ToXml());
    xmlElement->AppendChild(paddingElement->ToXml());
    xmlElement->AppendChild(textColorElement->ToXml());
    xmlElement->AppendChild(frameColorElement->ToXml());
    xmlElement->SetAttribute("frameWidth", std::to_string(frameWidth));
    return xmlElement;
}

void ClassLayoutElement::Parse(soul::xml::Element* parentXmlElement)
{
    std::unique_ptr<soul::xml::xpath::NodeSet> nodeSet = soul::xml::xpath::EvaluateToNodeSet(Name(), parentXmlElement);
    int n = nodeSet->Count();
    if (n == 1)
    {
        soul::xml::Node* node = nodeSet->GetNode(0);
        if (node->IsElementNode())
        {
            soul::xml::Element* xmlElement = static_cast<soul::xml::Element*>(node);
            captionElement->Parse(xmlElement);
            paddingElement->Parse(xmlElement);
            textColorElement->Parse(xmlElement);
            frameColorElement->Parse(xmlElement);
            std::string frameWidthStr = xmlElement->GetAttribute("frameWidth");
            if (frameWidthStr.empty())
            {
                throw std::runtime_error("XML element '" + Name() + "' has no 'frameWidth' attribute");
            }
            frameWidth = std::stof(frameWidthStr);
        }
        else
        {
            throw std::runtime_error("XML element node '" + Name() + "' expected in '" + parentXmlElement->Name() + "'");
        }
    }
    else
    {
        throw std::runtime_error("XML element '" + Name() + "' not unique in '" + parentXmlElement->Name() + "'");
    }
}

wing::Pen* ClassLayoutElement::FramePen()
{
    if (!framePen)
    {
        framePen = GetLayout()->GetOrInsertPen(frameColorElement->Color(), frameWidth, Gdiplus::DashStyleSolid);
    }
    return framePen;
}

AbstractClassLayoutElement::AbstractClassLayoutElement(Layout* layout_) : 
    ClassLayoutElement(layout_, "abstractClass", 
        new CaptionElement(layout_, 
            new FontElement(layout_, "font", DefaultCaptionFontFamily(), DefaultFontSize(), DefaultAbstractClassCaptionFontStyle())),
        new PaddingElement(layout_, "padding", DefaultClassPadding()),
        new ColorElement(layout_, "textColor", DefaultTextColor()),
        new ColorElement(layout_, "frameColor", DefaultFrameColor()),
        DefaultFrameWidth(layout_->Graphics()))
{
}

ConcreteClassLayoutElement::ConcreteClassLayoutElement(Layout* layout_) : 
    ClassLayoutElement(layout_, "concreteClass",
        new CaptionElement(layout_,
            new FontElement(layout_, "font", DefaultCaptionFontFamily(), DefaultFontSize(), DefaultConcreteClassCaptionFontStyle())),
        new PaddingElement(layout_, "padding", DefaultClassPadding()),
        new ColorElement(layout_, "textColor", DefaultTextColor()),
        new ColorElement(layout_, "frameColor", DefaultFrameColor()),
        DefaultFrameWidth(layout_->Graphics()))
{
}

OperationLayoutElement::OperationLayoutElement(Layout* layout_, const std::string& name_, ColorElement* textColorElement_, FontElement* fontElement_) :
    LayoutElement(layout_, name_), textColorElement(textColorElement_), fontElement(fontElement_)
{
}

soul::xml::Element* OperationLayoutElement::ToXml() const 
{
    soul::xml::Element* xmlElement = soul::xml::MakeElement(Name());
    xmlElement->AppendChild(textColorElement->ToXml());
    xmlElement->AppendChild(fontElement->ToXml());
    return xmlElement;
}

void OperationLayoutElement::Parse(soul::xml::Element* parentXmlElement)
{
    std::unique_ptr<soul::xml::xpath::NodeSet> nodeSet = soul::xml::xpath::EvaluateToNodeSet(Name(), parentXmlElement);
    int n = nodeSet->Count();
    if (n == 1)
    {
        soul::xml::Node* node = nodeSet->GetNode(0);
        if (node->IsElementNode())
        {
            soul::xml::Element* xmlElement = static_cast<soul::xml::Element*>(node);
            textColorElement->Parse(xmlElement);
            fontElement->Parse(xmlElement);
        }
        else
        {
            throw std::runtime_error("XML element node '" + Name() + "' expected in '" + parentXmlElement->Name() + "'");
        }
    }
    else
    {
        throw std::runtime_error("XML element '" + Name() + "' not unique in '" + parentXmlElement->Name() + "'");
    }
}

ConcreteOperationLayoutElement::ConcreteOperationLayoutElement(Layout* layout_) : 
    OperationLayoutElement(layout_, 
        "concreteOperation", 
        new ColorElement(layout_, "textColor", DefaultTextColor()),
        new FontElement(layout_, "font", DefaultOperationFontFamily(), DefaultFontSize(), DefaultConcreteOperationFontStyle()))
{
}

AbstractOperationLayoutElement::AbstractOperationLayoutElement(Layout* layout_) :
    OperationLayoutElement(layout_,
        "abstractOperation",
        new ColorElement(layout_, "textColor", DefaultTextColor()),
        new FontElement(layout_, "font", DefaultOperationFontFamily(), DefaultFontSize(), DefaultAbstractOperationFontStyle()))
{
}

FieldLayoutElement::FieldLayoutElement(Layout* layout_) : LayoutElement(layout_, "field"), 
    textColorElement(new ColorElement(layout_, "textColor", DefaultTextColor())), 
    fontElement(new FontElement(layout_, "font", DefaultFieldFontFamily(), DefaultFontSize(), DefaultFieldFontStyle()))
{
}

soul::xml::Element* FieldLayoutElement::ToXml() const
{
    soul::xml::Element* xmlElement = soul::xml::MakeElement(Name());
    xmlElement->AppendChild(textColorElement->ToXml());
    xmlElement->AppendChild(fontElement->ToXml());
    return xmlElement;
}

void FieldLayoutElement::Parse(soul::xml::Element* parentXmlElement)
{
    std::unique_ptr<soul::xml::xpath::NodeSet> nodeSet = soul::xml::xpath::EvaluateToNodeSet(Name(), parentXmlElement);
    int n = nodeSet->Count();
    if (n == 1)
    {
        soul::xml::Node* node = nodeSet->GetNode(0);
        if (node->IsElementNode())
        {
            soul::xml::Element* xmlElement = static_cast<soul::xml::Element*>(node);
            textColorElement->Parse(xmlElement);
            fontElement->Parse(xmlElement);
        }
        else
        {
            throw std::runtime_error("XML element node '" + Name() + "' expected in '" + parentXmlElement->Name() + "'");
        }
    }
    else
    {
        throw std::runtime_error("XML element '" + Name() + "' not unique in '" + parentXmlElement->Name() + "'");
    }
}

ResizeHandleLayoutElement::ResizeHandleLayoutElement(Layout* layout_) :
    LayoutElement(layout_, "resizeHandle"),
    lineWidth(DefaultLineWidth(layout_->Graphics())),
    resizeHandleWidth(DefaultResizeHandleWidth()),
    lineColorElement(new ColorElement(layout_, "lineColor", DefaultResizeHandleLineColor())),
    insideColorElement(new ColorElement(layout_, "insideColor", DefaultResizeHandleInsideColor())),
    linePen(nullptr),
    insideBrush(nullptr)
{
}

soul::xml::Element* ResizeHandleLayoutElement::ToXml() const
{
    soul::xml::Element* xmlElement = soul::xml::MakeElement(Name());
    xmlElement->SetAttribute("lineWidth", std::to_string(lineWidth));
    xmlElement->SetAttribute("resizeHandleWidth", std::to_string(resizeHandleWidth));
    xmlElement->AppendChild(lineColorElement->ToXml());
    xmlElement->AppendChild(insideColorElement->ToXml());
    return xmlElement;
}

void ResizeHandleLayoutElement::Parse(soul::xml::Element* parentXmlElement)
{
    std::unique_ptr<soul::xml::xpath::NodeSet> nodeSet = soul::xml::xpath::EvaluateToNodeSet(Name(), parentXmlElement);
    int n = nodeSet->Count();
    if (n == 1)
    {
        soul::xml::Node* node = nodeSet->GetNode(0);
        if (node->IsElementNode())
        {
            soul::xml::Element* xmlElement = static_cast<soul::xml::Element*>(node);
            std::string lineWidthStr = xmlElement->GetAttribute("lineWidth");
            if (lineWidthStr.empty())
            {
                throw std::runtime_error("XML element '" + Name() + "' has no 'lineWidth' attribute");
            }
            lineWidth = std::stof(lineWidthStr);
            std::string resizeHandleWidthStr = xmlElement->GetAttribute("resizeHandleWidth");
            if (resizeHandleWidthStr.empty())
            {
                throw std::runtime_error("XML element '" + Name() + "' has no 'resizeHandleWidth' attribute");
            }
            resizeHandleWidth = std::stof(resizeHandleWidthStr);
            lineColorElement->Parse(xmlElement);
            insideColorElement->Parse(xmlElement);
        }
        else
        {
            throw std::runtime_error("XML element node '" + Name() + "' expected in '" + parentXmlElement->Name() + "'");
        }
    }
    else
    {
        throw std::runtime_error("XML element '" + Name() + "' not unique in '" + parentXmlElement->Name() + "'");
    }
}

wing::Pen* ResizeHandleLayoutElement::LinePen()
{
    if (!linePen)
    {
        linePen = GetLayout()->GetOrInsertPen(lineColorElement->Color(), lineWidth, Gdiplus::DashStyleSolid);
    }
    return linePen;
}

wing::Brush* ResizeHandleLayoutElement::InsideBrush()
{
    return insideColorElement->GetBrush();
}

RelationshipLayoutElement::RelationshipLayoutElement(Layout* layout_) : 
    LayoutElement(layout_, "relationship"), 
    inheritanceArrowWidth(DefaultInheritanceArrowWidth()),
    compositeSymbolWidth(DefaultCompositeSymbolWidth()),
    compositeSymbolHeight(DefaultCompositeSymbolHeight()),
    aggregateSymbolWidth(DefaultAggregateSymbolWidth()),
    aggregateSymbolHeight(DefaultAggregateSymbolHeight()),
    lineArrowWidth(DefaultLineArrowWidth()),
    lineArrowHeight(DefaultLineArrowHeight()),
    cardinalitySymbolRadius(DefaultCardinalitySymbolRadius()),
    relationshipSymbolRadius(DefaultRelationshipSymbolRadius()),
    nullReferenceSymbolWidth(DefaultNullReferenceSymbolWidth()),
    nullReferenceSymbolHeight(DefaultNullReferenceSymbolHeight()),
    lineWidth(DefaultLineWidth(layout_->Graphics())),
    selectedLineWidth(DefaultSelectedLineWidth()),
    paddingElement(new PaddingElement(layout_, "padding", DefaultRelationshipSymbolPadding())),
    fontElement(new FontElement(layout_, "font", DefaultRelationshipFontFamily(), DefaultFontSize(), DefaultDefaultRelationshipFontStyle())),
    lineColorElement(new ColorElement(layout_, "lineColor", DefaultLineColor())),
    textColorElement(new ColorElement(layout_, "textColor", DefaultTextColor())),
    arrowColorElement(new ColorElement(layout_, "arrowColor", DefaultArrowColor())),
    compositeSourceColorElement(new ColorElement(layout_, "compositeSourceColor", DefaultCompositeSourceColor())),
    linePen(nullptr), 
    dashLinePen(nullptr),
    dotLinePen(nullptr),
    selectedLinePen(nullptr)
{
}

soul::xml::Element* RelationshipLayoutElement::ToXml() const
{
    soul::xml::Element* xmlElement = soul::xml::MakeElement("relationship");
    xmlElement->SetAttribute("inheritanceArrowWidth", std::to_string(inheritanceArrowWidth));
    xmlElement->SetAttribute("compositeSymbolWidth", std::to_string(compositeSymbolWidth));
    xmlElement->SetAttribute("compositeSymbolHeight", std::to_string(compositeSymbolHeight));
    xmlElement->SetAttribute("aggregateSymbolWidth", std::to_string(aggregateSymbolWidth));
    xmlElement->SetAttribute("aggregateSymbolHeight", std::to_string(aggregateSymbolHeight));
    xmlElement->SetAttribute("lineArrowWidth", std::to_string(lineArrowWidth));
    xmlElement->SetAttribute("lineArrowHeight", std::to_string(lineArrowHeight));
    xmlElement->SetAttribute("cardinalitySymbolRadius", std::to_string(cardinalitySymbolRadius));
    xmlElement->SetAttribute("relationshipSymbolRadius", std::to_string(relationshipSymbolRadius));
    xmlElement->SetAttribute("nullReferenceSymbolWidth", std::to_string(nullReferenceSymbolWidth));
    xmlElement->SetAttribute("nullReferenceSymbolHeight", std::to_string(nullReferenceSymbolHeight));
    xmlElement->SetAttribute("selectedLineWidth", std::to_string(selectedLineWidth));
    xmlElement->AppendChild(paddingElement->ToXml());
    xmlElement->AppendChild(fontElement->ToXml());
    xmlElement->AppendChild(lineColorElement->ToXml());
    xmlElement->AppendChild(textColorElement->ToXml());
    xmlElement->AppendChild(arrowColorElement->ToXml());
    xmlElement->AppendChild(compositeSourceColorElement->ToXml());
    return xmlElement;
}

void RelationshipLayoutElement::Parse(soul::xml::Element* parentXmlElement)
{
    std::unique_ptr<soul::xml::xpath::NodeSet> nodeSet = soul::xml::xpath::EvaluateToNodeSet(Name(), parentXmlElement);
    int n = nodeSet->Count();
    if (n == 1)
    {
        soul::xml::Node* node = nodeSet->GetNode(0);
        if (node->IsElementNode())
        {
            soul::xml::Element* xmlElement = static_cast<soul::xml::Element*>(node);
            std::string inheritanceArrowWidthStr = xmlElement->GetAttribute("inheritanceArrowWidth");
            if (inheritanceArrowWidthStr.empty())
            {
                throw std::runtime_error("XML element '" + Name() + "' has no 'inheritanceArrowWidth' attribute");
            }
            inheritanceArrowWidth = std::stof(inheritanceArrowWidthStr);
            std::string compositeSymbolWidthStr = xmlElement->GetAttribute("compositeSymbolWidth");
            if (compositeSymbolWidthStr.empty())
            {
                throw std::runtime_error("XML element '" + Name() + "' has no 'compsiteSymbolWidth' attribute");
            }
            compositeSymbolWidth = std::stof(compositeSymbolWidthStr);
            std::string compositeSymbolHeightStr = xmlElement->GetAttribute("compositeSymbolHeight");
            if (compositeSymbolHeightStr.empty())
            {
                throw std::runtime_error("XML element '" + Name() + "' has no 'compositeSymbolHeight' attribute");
            }
            compositeSymbolHeight = std::stof(compositeSymbolHeightStr);
            std::string aggregateSymbolWidthStr = xmlElement->GetAttribute("aggregateSymbolWidth");
            if (aggregateSymbolWidthStr.empty())
            {
                throw std::runtime_error("XML element '" + Name() + "' has no 'aggregateSymbolWidth' attribute");
            }
            aggregateSymbolWidth = std::stof(aggregateSymbolWidthStr);
            std::string aggregateSymbolHeightStr = xmlElement->GetAttribute("aggregateSymbolHeight");
            if (aggregateSymbolHeightStr.empty())
            {
                throw std::runtime_error("XML element '" + Name() + "' has no 'aggregateSymbolHeight' attribute");
            }
            aggregateSymbolHeight = std::stof(aggregateSymbolHeightStr);
            std::string lineArrowWidthStr = xmlElement->GetAttribute("lineArrowWidth");
            if (lineArrowWidthStr.empty())
            {
                throw std::runtime_error("XML element '" + Name() + "' has no 'lineArrowWidth' attribute");
            }
            lineArrowWidth = std::stof(lineArrowWidthStr);
            std::string lineArrowHeightStr = xmlElement->GetAttribute("lineArrowHeight");
            if (lineArrowHeightStr.empty())
            {
                throw std::runtime_error("XML element '" + Name() + "' has no 'lineArrowHeight' attribute");
            }
            lineArrowHeight = std::stof(lineArrowHeightStr);
            std::string cardinalitySymbolRadiusStr = xmlElement->GetAttribute("cardinalitySymbolRadius");
            if (cardinalitySymbolRadiusStr.empty())
            {
                throw std::runtime_error("XML element '" + Name() + "' has no 'cardinalitySymbolRadius' attribute");
            }
            cardinalitySymbolRadius = std::stof(cardinalitySymbolRadiusStr);
            std::string relationshipSymbolRadiusStr = xmlElement->GetAttribute("relationshipSymbolRadius");
            if (relationshipSymbolRadiusStr.empty())
            {
                throw std::runtime_error("XML element '" + Name() + "' has no 'relationshipSymbolRadius' attribute");
            }
            relationshipSymbolRadius = std::stof(relationshipSymbolRadiusStr);
            std::string nullReferenceSymbolWidthStr = xmlElement->GetAttribute("nullReferenceSymbolWidth");
            if (nullReferenceSymbolWidthStr.empty())
            {
                throw std::runtime_error("XML element '" + Name() + "' has no 'nullReferenceSymbolWidth' attribute");
            }
            nullReferenceSymbolWidth = std::stof(nullReferenceSymbolWidthStr);
            std::string nullReferenceSymbolHeightStr = xmlElement->GetAttribute("nullReferenceSymbolHeight");
            if (nullReferenceSymbolHeightStr.empty())
            {
                throw std::runtime_error("XML element '" + Name() + "' has no 'nullReferenceSymbolHeight' attribute");
            }
            nullReferenceSymbolHeight = std::stof(nullReferenceSymbolHeightStr);
            std::string selectedLineWidthStr = xmlElement->GetAttribute("selectedLineWidth");
            if (selectedLineWidthStr.empty())
            {
                throw std::runtime_error("XML element '" + Name() + "' has no 'selectedLineWidth' attribute");
            }
            selectedLineWidth = std::stof(selectedLineWidthStr);
            paddingElement->Parse(xmlElement);
            fontElement->Parse(xmlElement);
            lineColorElement->Parse(xmlElement);
            textColorElement->Parse(xmlElement);
            arrowColorElement->Parse(xmlElement);
            compositeSourceColorElement->Parse(xmlElement);
        }
        else
        {
            throw std::runtime_error("XML element node '" + Name() + "' expected in '" + parentXmlElement->Name() + "'");
        }
    }
    else
    {
        throw std::runtime_error("XML element '" + Name() + "' not unique in '" + parentXmlElement->Name() + "'");
    }
}

wing::Pen* RelationshipLayoutElement::LinePen()
{
    if (!linePen)
    {
        linePen = GetLayout()->GetOrInsertPen(lineColorElement->Color(), lineWidth, Gdiplus::DashStyleSolid);
    }
    return linePen;
}

wing::Pen* RelationshipLayoutElement::DashLinePen()
{
    if (!dashLinePen)
    {
        dashLinePen = GetLayout()->GetOrInsertPen(lineColorElement->Color(), lineWidth, Gdiplus::DashStyleDash);
    }
    return dashLinePen;
}

wing::Pen* RelationshipLayoutElement::DotLinePen()
{
    if (!dotLinePen)
    {
        dotLinePen = GetLayout()->GetOrInsertPen(lineColorElement->Color(), lineWidth, Gdiplus::DashStyleDot);
    }
    return dotLinePen;
}

wing::Pen* RelationshipLayoutElement::SelectedLinePen()
{
    if (!selectedLinePen)
    {
        Layout* layout = GetLayout();
        wing::Brush* brush = layout->GetSelectionColorElement()->GetBrush();
        selectedLinePen = layout->GetOrInsertPen(layout->GetSelectionColorElement()->Color(), selectedLineWidth, Gdiplus::DashStyleSolid);
        selectedLinePen->SetBrush(brush);
        selectedLinePen->SetStartCap(Gdiplus::LineCapRound);
        selectedLinePen->SetEndCap(Gdiplus::LineCapRound);
        selectedLinePen->SetLineJoin(Gdiplus::LineJoinRound);
    }
    return selectedLinePen;
}

wing::Brush* RelationshipLayoutElement::GetArrowBrush()
{
    return arrowColorElement->GetBrush();
}

Layout::Layout(wing::Graphics* graphics_, const std::string& xmlFileName_) : graphics(graphics_), stringFormat(), xmlFilePath(util::Path::Combine(LayoutDirectory(), xmlFileName_))
{
    selectionColorElement.reset(new ColorElement(this, "selectionColor", DefaultSelectionColor()));
    diagramPaddingElement.reset(new PaddingElement(this, "diagramPadding", DefaultDiagramPadding()));
    abstractClassLayoutElement.reset(new AbstractClassLayoutElement(this));
    concreteClassLayoutElement.reset(new ConcreteClassLayoutElement(this));
    abstractOperationLayoutElement.reset(new AbstractOperationLayoutElement(this));
    concreteOperationLayoutElement.reset(new ConcreteOperationLayoutElement(this));
    fieldLayoutElement.reset(new FieldLayoutElement(this));
    resizeHandleLayoutElement.reset(new ResizeHandleLayoutElement(this));
    relationshipLayoutElement.reset(new RelationshipLayoutElement(this));
}

soul::xml::Element* Layout::ToXml() const
{
    soul::xml::Element* layoutElement = soul::xml::MakeElement("layout");
    layoutElement->AppendChild(selectionColorElement->ToXml());
    layoutElement->AppendChild(diagramPaddingElement->ToXml());
    layoutElement->AppendChild(abstractClassLayoutElement->ToXml());
    layoutElement->AppendChild(concreteClassLayoutElement->ToXml());
    layoutElement->AppendChild(abstractOperationLayoutElement->ToXml());
    layoutElement->AppendChild(concreteOperationLayoutElement->ToXml());
    layoutElement->AppendChild(fieldLayoutElement->ToXml());
    layoutElement->AppendChild(resizeHandleLayoutElement->ToXml());
    layoutElement->AppendChild(relationshipLayoutElement->ToXml());
    return layoutElement;
}

void Layout::Parse(soul::xml::Document* layoutDoc)
{
    std::unique_ptr<soul::xml::xpath::NodeSet> nodeSet = soul::xml::xpath::EvaluateToNodeSet("/layout", layoutDoc);
    if (nodeSet->Count() == 1)
    {
        soul::xml::Node* node = nodeSet->GetNode(0);
        if (node->IsElementNode())
        {
            soul::xml::Element* layoutElement = static_cast<soul::xml::Element*>(node);
            selectionColorElement->Parse(layoutElement);
            diagramPaddingElement->Parse(layoutElement);
            abstractClassLayoutElement->Parse(layoutElement);
            concreteClassLayoutElement->Parse(layoutElement);
            abstractOperationLayoutElement->Parse(layoutElement);
            concreteOperationLayoutElement->Parse(layoutElement);
            fieldLayoutElement->Parse(layoutElement);
            resizeHandleLayoutElement->Parse(layoutElement);
            relationshipLayoutElement->Parse(layoutElement);
        }
        else
        {
            throw std::runtime_error("XML element node 'layout' expected in XML document");
        }
    }
    else
    {
        throw std::runtime_error("Root XML element 'layout' not unique in XML document");
    }
}

void Layout::Save()
{
    try
    {
        std::filesystem::create_directories(util::Path::GetDirectoryName(xmlFilePath));
        soul::xml::Document layoutDoc;
        layoutDoc.AppendChild(ToXml());
        std::ofstream xmlFile(xmlFilePath);
        util::CodeFormatter formatter(xmlFile);
        formatter.SetIndentSize(1);
        layoutDoc.Write(formatter);
    }
    catch (const std::exception& ex)
    {
        throw std::runtime_error("could not save layout to '" + xmlFilePath + "': " + ex.what());
    }
}

void Layout::Load()
{
    try
    {
        std::unique_ptr<soul::xml::Document> layoutXmlDoc = soul::xml::ParseXmlFile(xmlFilePath);
        Parse(layoutXmlDoc.get());
    }
    catch (const std::exception& ex)
    {
        throw std::runtime_error("could not load layout from '" + xmlFilePath + "': " + ex.what());
    }
}

wing::Font* Layout::GetOrInsertFont(const std::string& fontFamily, float fontSize, wing::FontStyle fontStyle)
{
    wing::FontKey fontKey(fontFamily, fontSize, fontStyle);
    std::string fontKeyStr = fontKey.ToString();
    auto it = fontMap.find(fontKeyStr);
    if (it != fontMap.end())
    {
        return it->second;
    }
    std::u16string familyName = util::ToUtf16(fontFamily);
    wing::FontFamily family((const WCHAR*)familyName.c_str());
    wing::Font* font = new wing::Font(family, fontSize, fontStyle, wing::Unit::UnitPoint);
    fonts.push_back(std::unique_ptr<wing::Font>(font));
    fontMap[fontKeyStr] = font;
    return font;
}

wing::Brush* Layout::GetOrInsertBrush(const wing::Color& color)
{
    auto it = brushMap.find(color.GetValue());
    if (it != brushMap.end())
    {
        return it->second;
    }
    wing::SolidBrush* brush = new wing::SolidBrush(color);
    brushes.push_back(std::unique_ptr<wing::Brush>(brush));
    brushMap[color.GetValue()] = brush;
    return brush;
}

wing::Pen* Layout::GetOrInsertPen(const wing::Color& color, float width, wing::DashStyle dashStyle)
{
    std::string penKey = wing::MakePenKey(color, width, dashStyle);
    auto it = penMap.find(penKey);
    if (it != penMap.end())
    {
        return it->second;
    }
    wing::Pen* pen = new wing::Pen(color, width);
    pen->SetDashStyle(dashStyle);
    pens.push_back(std::unique_ptr<wing::Pen>(pen));
    penMap[penKey] = pen;
    return pen;
}

float GetRelationshipSymbolFieldWidth(float relationshipSymbolRadius, float horizontalPadding)
{
    float symbolFieldWidth = horizontalPadding + 2.0f * relationshipSymbolRadius;
    return symbolFieldWidth;
}

} // namespace springpp
