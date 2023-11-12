// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

export module springpp.field_element;

import springpp.diagram_element;
import wing;
import std.core;

export namespace springpp {

class RelationshipElement;

class FieldElement : public DiagramElement
{
public:
    FieldElement();
    DiagramElement* Clone() const override;
    ContainerElement* GetContainerElement() const override { return containerElement; }
    void SetContainerElement(ContainerElement* containerElement_);
    EndPoint GetEndPoint(const Snap& snap) const override;
    RelationshipElement* Relationship() const { return relationship; }
    void AddRelationship(RelationshipElement* relationship_) override;
    void RemoveRelationship(RelationshipElement* relationship_) override;
    void SetRelationshipPoint();
    wing::PointF GetRelationshipPoint() const;
    void Measure(wing::Graphics& graphics) override;
    void Draw(wing::Graphics& graphics) override;
    soul::xml::Element* ToXml() const override;
    void Parse(soul::xml::Element* xmlElement) override;
private:
    ContainerElement* containerElement;
    RelationshipElement* relationship;
};

} // namespace springpp
