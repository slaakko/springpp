// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

export module springpp.relationships;

import springpp.relationship_element;
import wing;
import std.core;

export namespace springpp {

class Inheritance : public RelationshipElementRep
{
public:
    Inheritance(RelationshipElement* relationshipElement_);
    RelationshipElementRep* Clone(RelationshipElement* relationshipElement_) const override;
    void Draw(wing::Graphics& graphics) override;
    float TargetSymbolWidth() const override;
};

class CombinedInheritance : public RelationshipElementRep
{
public:
    CombinedInheritance(RelationshipElement* relationshipElement_);
    RelationshipElementRep* Clone(RelationshipElement* relationshipElement_) const override;
    void Draw(wing::Graphics& graphics) override;
    void DrawSelected(wing::Graphics& graphics) override;
    bool Contains(const wing::PointF& location) const override;
    float TargetSymbolWidth() const override;
private:
    void DrawCombinedInheritance0(wing::Graphics& graphics, RelationshipElement* relationshipElement);
    void DrawCombinedInheritance90(wing::Graphics& graphics, RelationshipElement* relationshipElement);
    void DrawCombinedInheritance180(wing::Graphics& graphics, RelationshipElement* relationshipElement);
    void DrawCombinedInheritance270(wing::Graphics& graphics, RelationshipElement* relationshipElement);
    void DrawCombinedInheritance0Selected(wing::Graphics& graphics, RelationshipElement* relationshipElement);
    void DrawCombinedInheritance90Selected(wing::Graphics& graphics, RelationshipElement* relationshipElement);
    void DrawCombinedInheritance180Selected(wing::Graphics& graphics, RelationshipElement* relationshipElement);
    void DrawCombinedInheritance270Selected(wing::Graphics& graphics, RelationshipElement* relationshipElement);
    bool Contains0(const wing::PointF& location, RelationshipElement* relationshipElement) const;
    bool Contains90(const wing::PointF& location, RelationshipElement* relationshipElement) const;
    bool Contains180(const wing::PointF& location, RelationshipElement* relationshipElement) const;
    bool Contains270(const wing::PointF& location, RelationshipElement* relationshipElement) const;
};

class Composition: public RelationshipElementRep
{
public:
    Composition(RelationshipElement* relationshipElement_);
    RelationshipElementRep* Clone(RelationshipElement* relationshipElement_) const override;
    void Draw(wing::Graphics& graphics) override;
    float SourceSymbolWidth() const override;
    float TargetSymbolWidth() const override;
};

class Aggregation : public RelationshipElementRep
{
public:
    Aggregation(RelationshipElement* relationshipElement_);
    RelationshipElementRep* Clone(RelationshipElement* relationshipElement_) const override;
    void Draw(wing::Graphics& graphics) override;
    float SourceSymbolWidth() const override;
    float TargetSymbolWidth() const override;
};

class Reference : public RelationshipElementRep
{
public:    
    Reference(RelationshipElement* relationshipElement_);
    RelationshipElementRep* Clone(RelationshipElement* relationshipElement_) const override;
    void Draw(wing::Graphics& graphics) override;
    float TargetSymbolWidth() const override;
};

class CreateInstance : public RelationshipElementRep
{
public:
    CreateInstance(RelationshipElement* relationshipElement_);
    RelationshipElementRep* Clone(RelationshipElement* relationshipElement_) const override;
    void Draw(wing::Graphics& graphics) override;
    float TargetSymbolWidth() const override;
};

class AttachNote : public RelationshipElementRep
{
public:
    AttachNote(RelationshipElement* relationshipElement_);
    RelationshipElementRep* Clone(RelationshipElement* relationshipElement_) const override;
    void Draw(wing::Graphics& graphics) override;
};

} // namespace springpp
