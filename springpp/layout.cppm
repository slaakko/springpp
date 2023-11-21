// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

export module springpp.layout;

import soul.xml.dom;
import wing;
import std.core;

export namespace springpp {

struct Padding
{
    Padding();
    Padding(float left_, float top_, float right_, float bottom_);
    float Vertical() const { return top + bottom; }
    float Horizontal() const { return left + right; }
    float left;
    float top;
    float right;
    float bottom;
};

std::string DefaultKeywordFontFamily();
std::string DefaultCaptionFontFamily();
std::string DefaultOperationFontFamily();
std::string DefaultAttributeFontFamily();
std::string DefaultRelationshipFontFamily();
std::string DefaultNoteFontFamily();
std::string DefaultTextFontFamily();
float DefaultFontSize();
wing::FontStyle DefaultKeywordFontStyle();
wing::FontStyle DefaultAbstractClassCaptionFontStyle();
wing::FontStyle DefaultConcreteClassCaptionFontStyle();
wing::FontStyle DefaultConcreteOperationFontStyle();
wing::FontStyle DefaultAbstractOperationFontStyle();
wing::FontStyle DefaultObjectCaptionFontStyle();
wing::FontStyle DefaultAttributeFontStyle();
wing::FontStyle DefaultNoteFontStyle();
wing::FontStyle DefaultTextFontStyle();
wing::FontStyle DefaultDefaultRelationshipFontStyle();

float DefaultFrameWidth(wing::Graphics* graphics);
float DefaultLineWidth(wing::Graphics* graphics);
float DefaultFrameRoundingRadius();
Padding DefaultDiagramPadding();
Padding DefaultClassPadding();
Padding DefaultObjectPadding();
Padding DefaultNotePadding();
Padding DefaultRelationshipSymbolPadding();
Padding DefaultTextPadding();
float DefaultInheritanceArrowWidth();
float DefaultCompositeSymbolWidth();
float DefaultCompositeSymbolHeight();
float DefaultAggregateSymbolWidth();
float DefaultAggregateSymbolHeight();
float DefaultLineArrowWidth();
float DefaultLineArrowHeight();
float DefaultCardinalitySymbolRadius();
float DefaultRelationshipSymbolRadius();
float DefaultSelectedLineWidth();
float DefaultNullReferenceSymbolWidth();
float DefaultNullReferenceSymbolHeight();
wing::Color DefaultTextColor();
wing::Color DefaultFrameColor();
wing::Color DefaultLineColor();
wing::Color DefaultSelectionColor();
wing::Color DefaultArrowColor();
wing::Color DefaultCompositeSourceColor();
float DefaultResizeHandleWidth();
wing::Color DefaultResizeHandleLineColor();
wing::Color DefaultResizeHandleInsideColor();

std::string SpringPPDirectory();
std::string LayoutDirectory();
std::string DiagramDirectory();

class Layout;

class LayoutElement
{
public:
    LayoutElement(Layout* layout_, const std::string& name_);
    virtual ~LayoutElement();
    Layout* GetLayout() { return layout; }
    const std::string& Name() const { return name; }
    virtual soul::xml::Element* ToXml() const = 0;
    virtual void Parse(soul::xml::Element* parentXmlElement) = 0;
private:
    Layout* layout;
    std::string name;
};

class ColorElement : public LayoutElement
{
public:
    ColorElement(Layout* layout_, const std::string& name_, const wing::Color& color_);
    const wing::Color& Color() const { return color; }
    soul::xml::Element* ToXml() const override;
    void Parse(soul::xml::Element* parentXmlElement) override;
    wing::Brush* GetBrush();
private:
    wing::Color color;
    wing::Brush* brush;
};

class FontElement : public LayoutElement
{
public:
    FontElement(Layout* layout_, const std::string& name_, const std::string& family_, float size_, wing::FontStyle style_);
    const std::string& Family() const { return family; }
    float Size() const { return size; }
    wing::FontStyle Style() const { return style; }
    soul::xml::Element* ToXml() const override;
    void Parse(soul::xml::Element* parentXmlElement) override;
    wing::Font* GetFont();
private:
    std::string family;
    float size;
    wing::FontStyle style;
    wing::Font* font;
};

class PaddingElement : public LayoutElement
{
public:
    PaddingElement(Layout* layout_, const std::string& name_, const Padding& padding_);
    const Padding& GetPadding() const { return padding; }
    soul::xml::Element* ToXml() const override;
    void Parse(soul::xml::Element* parentXmlElement) override;
private:
    Padding padding;
};

class CaptionElement : public LayoutElement
{
public:
    CaptionElement(Layout* layout_, FontElement* keywordFontElement_, FontElement* nameFontElement_);
    FontElement* GetKeywordFontElement() const { return keywordFontElement.get(); }
    FontElement* GetNameFontElement() const { return nameFontElement.get(); }
    soul::xml::Element* ToXml() const override;
    void Parse(soul::xml::Element* parentXmlElement) override;
private:
    std::unique_ptr<FontElement> keywordFontElement;
    std::unique_ptr<FontElement> nameFontElement;
};

class ClassLayoutElement : public LayoutElement
{
public:
    ClassLayoutElement(Layout* layout_, const std::string& name_, 
        CaptionElement* captionElement_, PaddingElement* paddingElement_, ColorElement* textColorElement_, ColorElement* frameColorElement_, float frameWidth_);
    CaptionElement* GetCaptionElement() const { return captionElement.get(); }
    PaddingElement* GetPaddingElement() const { return paddingElement.get(); }
    ColorElement* GetTextColorElement() const { return textColorElement.get(); }
    ColorElement* GetFrameColorElement() const { return frameColorElement.get(); }
    soul::xml::Element* ToXml() const override;
    void Parse(soul::xml::Element* parentXmlElement) override;
    wing::Pen* FramePen();
private:
    std::unique_ptr<CaptionElement> captionElement;
    std::unique_ptr<PaddingElement> paddingElement;
    std::unique_ptr<ColorElement> textColorElement;
    std::unique_ptr<ColorElement> frameColorElement;
    float frameWidth;
    wing::Pen* framePen;
};

class AbstractClassLayoutElement : public ClassLayoutElement
{
public:
    AbstractClassLayoutElement(Layout* layout_);
};

class ConcreteClassLayoutElement : public ClassLayoutElement
{
public:
    ConcreteClassLayoutElement(Layout* layout_);
};

class ObjectLayoutElement : public LayoutElement
{
public:
    ObjectLayoutElement(Layout* layout_);
    CaptionElement* GetCaptionElement() const { return captionElement.get(); }
    PaddingElement* GetPaddingElement() const { return paddingElement.get(); }
    ColorElement* GetTextColorElement() const { return textColorElement.get(); }
    ColorElement* GetFrameColorElement() const { return frameColorElement.get(); }
    float FrameWidth() const { return frameWidth; }
    float FrameRoundingRadius() const { return frameRoundingRadius; }
    soul::xml::Element* ToXml() const override;
    void Parse(soul::xml::Element* parentXmlElement) override;
    wing::Pen* FramePen();
private:
    std::unique_ptr<CaptionElement> captionElement;
    std::unique_ptr<PaddingElement> paddingElement;
    std::unique_ptr<ColorElement> textColorElement;
    std::unique_ptr<ColorElement> frameColorElement;
    float frameWidth;
    float frameRoundingRadius;
    wing::Pen* framePen;
};

class NoteLayoutElement : public LayoutElement
{
public:
    NoteLayoutElement(Layout* layout_);
    PaddingElement* GetPaddingElement() const { return paddingElement.get(); }
    ColorElement* TextColorElement() const { return textColorElement.get(); }
    ColorElement* FrameColorElement() const { return frameColorElement.get(); }
    FontElement* GetFontElement() const { return fontElement.get(); }
    float FrameWidth() const { return frameWidth; }
    soul::xml::Element* ToXml() const override;
    void Parse(soul::xml::Element* parentXmlElement) override;
    wing::Pen* FramePen();
private:
    std::unique_ptr<PaddingElement> paddingElement;
    std::unique_ptr<ColorElement> textColorElement;
    std::unique_ptr<ColorElement> frameColorElement;
    std::unique_ptr<FontElement> fontElement;
    float frameWidth;
    wing::Pen* framePen;
};

class OperationLayoutElement : public LayoutElement
{
public:
    OperationLayoutElement(Layout* layout_, const std::string& name_, ColorElement* textColorElement_, FontElement* fontElement_);
    ColorElement* GetTextColorElement() const { return textColorElement.get(); }
    FontElement* GetFontElement() const { return fontElement.get(); }
    soul::xml::Element* ToXml() const override;
    void Parse(soul::xml::Element* parentXmlElement) override;
private:
    std::unique_ptr<ColorElement> textColorElement;
    std::unique_ptr<FontElement> fontElement;
};

class ConcreteOperationLayoutElement : public OperationLayoutElement
{
public:
    ConcreteOperationLayoutElement(Layout* layout_);
};

class AbstractOperationLayoutElement : public OperationLayoutElement
{
public:
    AbstractOperationLayoutElement(Layout* layout_);
};

class AttributeLayoutElement : public LayoutElement
{
public:
    AttributeLayoutElement(Layout* layout_);
    ColorElement* GetTextColorElement() const { return textColorElement.get(); }
    FontElement* GetFontElement() const { return fontElement.get(); }
    soul::xml::Element* ToXml() const override;
    void Parse(soul::xml::Element* parentXmlElement) override;
private:
    std::unique_ptr<ColorElement> textColorElement;
    std::unique_ptr<FontElement> fontElement;
};

class TextLayoutElement : public LayoutElement
{
public:
    TextLayoutElement(Layout* layout_);
    PaddingElement* GetPaddingElement() const { return paddingElement.get(); }
    ColorElement* GetTextColorElement() const { return textColorElement.get(); }
    FontElement* GetFontElement() const { return fontElement.get(); }
    soul::xml::Element* ToXml() const override;
    void Parse(soul::xml::Element* parentXmlElement) override;
private:
    std::unique_ptr<PaddingElement> paddingElement;
    std::unique_ptr<ColorElement> textColorElement;
    std::unique_ptr<FontElement> fontElement;
};

class ResizeHandleLayoutElement : public LayoutElement
{
public:
    ResizeHandleLayoutElement(Layout* layout_);
    soul::xml::Element* ToXml() const override;
    void Parse(soul::xml::Element* parentXmlElement) override;
    float LineWidth() const { return lineWidth; }
    float ResizeHandleWidth() const { return resizeHandleWidth; }
    wing::Pen* LinePen();
    wing::Brush* InsideBrush();
private:
    float lineWidth;
    float resizeHandleWidth;
    std::unique_ptr<ColorElement> lineColorElement;
    std::unique_ptr<ColorElement> insideColorElement;
    wing::Pen* linePen;
    wing::Brush* insideBrush;
};

class RelationshipLayoutElement : public LayoutElement
{
public:
    RelationshipLayoutElement(Layout* layout_);
    soul::xml::Element* ToXml() const override;
    void Parse(soul::xml::Element* parentXmlElement) override;
    float InheritanceArrowWidth() const { return inheritanceArrowWidth; }
    float CompositeSymbolWidth() const { return compositeSymbolWidth; }
    float CompositeSymbolHeight() const { return compositeSymbolHeight; }
    float AggregateSymbolWidth() const { return aggregateSymbolWidth; }
    float AggregateSymbolHeight() const { return aggregateSymbolHeight; }
    float LineArrowWidth() const { return lineArrowWidth; }
    float LineArrowHeight() const { return lineArrowHeight; }
    float CardinalitySymbolRadius() const { return cardinalitySymbolRadius; }
    float RelationshipSymbolRadius() const { return relationshipSymbolRadius; }
    float NullReferenceSymbolWidth() const { return nullReferenceSymbolWidth; }
    float NullReferenceSymbolHeight() const { return nullReferenceSymbolHeight; }
    float LineWidth() const { return lineWidth; }
    float SelectedLineWidth() const { return selectedLineWidth; }
    PaddingElement* GetPaddingElement() const { return paddingElement.get(); }
    FontElement* GetFontElement() const { return fontElement.get(); }
    ColorElement* TextColorElement() const { return textColorElement.get(); }
    ColorElement* LineColorElement() const { return lineColorElement.get(); }
    ColorElement* CompositeSourceColorElement() const { return compositeSourceColorElement.get(); }
    wing::Pen* LinePen();
    wing::Pen* DashLinePen();
    wing::Pen* DotLinePen();
    wing::Pen* SelectedLinePen();
    wing::Brush* GetArrowBrush();
private:
    float inheritanceArrowWidth;
    float compositeSymbolWidth;
    float compositeSymbolHeight;
    float aggregateSymbolWidth;
    float aggregateSymbolHeight;
    float lineArrowWidth;
    float lineArrowHeight;
    float cardinalitySymbolRadius;
    float relationshipSymbolRadius;
    float nullReferenceSymbolWidth;
    float nullReferenceSymbolHeight;
    float lineWidth;
    float selectedLineWidth;
    std::unique_ptr<PaddingElement> paddingElement;
    std::unique_ptr<FontElement> fontElement;
    std::unique_ptr<ColorElement> lineColorElement;
    std::unique_ptr<ColorElement> textColorElement;
    std::unique_ptr<ColorElement> arrowColorElement;
    std::unique_ptr<ColorElement> compositeSourceColorElement;
    wing::Pen* linePen;
    wing::Pen* dashLinePen;
    wing::Pen* dotLinePen;
    wing::Pen* selectedLinePen;
};

class Layout
{
public:
    Layout(wing::Graphics* graphics_, const std::string& xmlFileName_);
    soul::xml::Element* ToXml() const;
    void Parse(soul::xml::Document* layoutDoc);
    wing::Graphics* Graphics() const { return graphics; }
    void SetGraphics(wing::Graphics* graphics_) { graphics = graphics_; }
    const std::string& XmlFilePath() const { return xmlFilePath; }
    void Save();
    void Load();
    ColorElement* GetSelectionColorElement() const { return selectionColorElement.get(); }
    PaddingElement* GetDiagramPaddingElement() const { return diagramPaddingElement.get(); }
    ClassLayoutElement* GetAbstractClassLayoutElement() const { return abstractClassLayoutElement.get(); }
    ClassLayoutElement* GetConcreteClassLayoutElement() const { return concreteClassLayoutElement.get(); }
    ObjectLayoutElement* GetObjectLayoutElement() const { return objectLayoutElement.get(); }
    NoteLayoutElement* GetNoteLayoutElement() const { return noteLayoutElement.get(); }
    TextLayoutElement* GetTextLayoutElement() const { return textLayoutElement.get(); }
    OperationLayoutElement* GetAbstractOperationLayoutElement() const { return abstractOperationLayoutElement.get(); }
    OperationLayoutElement* GetConcreteOperationLayoutElement() const { return concreteOperationLayoutElement.get(); }
    AttributeLayoutElement* GetAttributeLayoutElement() const { return attributeLayoutElement.get(); }
    ResizeHandleLayoutElement* GetResizeHandleLayoutElement() const { return resizeHandleLayoutElement.get(); }
    RelationshipLayoutElement* GetRelationshipLayoutElement() const { return relationshipLayoutElement.get(); }
    const wing::StringFormat& GetStringFormat() const { return stringFormat; }
    wing::Font* GetOrInsertFont(const std::string& fontFamily, float fontSize, wing::FontStyle fontStyle);
    wing::Brush* GetOrInsertBrush(const wing::Color& color);
    wing::Pen* GetOrInsertPen(const wing::Color& color, float width, wing::DashStyle dashStyle);
private:
    wing::Graphics* graphics;
    std::string xmlFilePath;
    std::unique_ptr<PaddingElement> diagramPaddingElement;
    std::unique_ptr<ColorElement> selectionColorElement;
    std::unique_ptr<ClassLayoutElement> abstractClassLayoutElement;
    std::unique_ptr<ClassLayoutElement> concreteClassLayoutElement;
    std::unique_ptr<ObjectLayoutElement> objectLayoutElement;
    std::unique_ptr<NoteLayoutElement> noteLayoutElement;
    std::unique_ptr<TextLayoutElement> textLayoutElement;
    std::unique_ptr<OperationLayoutElement> abstractOperationLayoutElement;
    std::unique_ptr<OperationLayoutElement> concreteOperationLayoutElement;
    std::unique_ptr<AttributeLayoutElement> attributeLayoutElement;
    std::unique_ptr<ResizeHandleLayoutElement> resizeHandleLayoutElement;
    std::unique_ptr<RelationshipLayoutElement> relationshipLayoutElement;
    wing::StringFormat stringFormat;
    std::map<std::string, wing::Font*> fontMap;
    std::vector<std::unique_ptr<wing::Font>> fonts;
    std::map<int, wing::Brush*> brushMap;
    std::vector<std::unique_ptr<wing::Brush>> brushes;
    std::map<std::string, wing::Pen*> penMap;
    std::vector<std::unique_ptr<wing::Pen>> pens;
};

float GetRelationshipSymbolFieldWidth(float relationshipSymbolRadius, float horizontalPadding);

} // namespace springpp

