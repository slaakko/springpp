// =================================
// Copyright (c) 2024 Seppo Laakko
// Distributed under the MIT license
// =================================

export module springpp.note_element;

import springpp.container_element;
import wing;
import std.core;

export namespace springpp {

class NoteElement : public ContainerElement
{
public:
    NoteElement();
    soul::xml::Element* ToXml() const override;
    void Parse(soul::xml::Element* xmlElement) override;
    void Measure(wing::Graphics& graphics) override;
    void Draw(wing::Graphics& graphics) override;
    DiagramElement* Clone() const override;
    void AddActions(Diagram* diagram, int elementIndex, wing::ContextMenu* contextMenu) const override;
    std::vector<RelationshipElement*> GetAllRelationships() const override;
    const std::vector<std::string>& Lines() const { return lines; }
    void SetLines(std::vector<std::string>&& lines_);
private:
    std::vector<std::string> lines;
    wing::SizeF lineSize;
};

} // namespace springpp
