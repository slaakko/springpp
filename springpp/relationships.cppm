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
};

class Composition: public RelationshipElementRep
{
public:
    Composition(RelationshipElement* relationshipElement_);
    RelationshipElementRep* Clone(RelationshipElement* relationshipElement_) const override;
    void Draw(wing::Graphics& graphics) override;
};

class Aggregation : public RelationshipElementRep
{
public:
    Aggregation(RelationshipElement* relationshipElement_);
    RelationshipElementRep* Clone(RelationshipElement* relationshipElement_) const override;
    void Draw(wing::Graphics& graphics) override;
};

} // namespace springpp
