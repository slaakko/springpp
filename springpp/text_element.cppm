// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

export module springpp.text_element;

import springpp.diagram_element;
import wing;
import std.core;

export namespace springpp {

enum class TextKind
{
    primaryText, secondaryText
};

class TextElement : public DiagramElement
{
public:
    TextElement();
    TextElement(const std::string& name_);
    bool IsEmpty() const;
    TextKind TKind() const { return tkind; }
    soul::xml::Element* ToXml() const override;
    void Parse(soul::xml::Element* xmlElement) override;
    DiagramElement* Clone() const override;
    void Measure(wing::Graphics& graphics) override;
    void Draw(wing::Graphics& graphics) override;
    const std::vector<std::string>& Lines() const { return lines; }
    void SetLines(std::vector<std::string>&& lines_);
    const std::string& Keyword() const { return keyword; }
    void SetKeyword(std::string&& keyword_);
    std::string XmlElementName() const { return Name().empty() ? "textElement" : Name(); }
    void SetSourceLocation(const Line& line, float symbolWidth);
    void SetTargetLocation(const Line& line, float symbolWidth);
private:
    void SetPrimarySourceLocation(const Line& line, float symbolWidth);
    void SetSecondarySourceLocation(const Line& line, float symbolWidth);
    void SetPrimaryTargetLocation(const Line& line, float symbolWidth);
    void SetSecondaryTargetLocation(const Line& line, float symbolWidth);
    TextKind tkind;
    std::vector<std::string> lines;
    std::string keyword;
    wing::SizeF keywordTextSize;
    wing::SizeF lineSize;
    wing::SizeF linesSize;
};

} // namespace springpp
