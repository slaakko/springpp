// =================================
// Copyright (c) 2024 Seppo Laakko
// Distributed under the MIT license
// =================================

export module springpp.relationship_element;

import springpp.diagram_element;
import springpp.end_point;
import springpp.primitive;
import wing;
import std.core;

export namespace springpp {

class ContainerElement;
class Layout;

enum class Cardinality
{
    zero, one, many
};

std::string CardinalityStr(Cardinality cardinality);
Cardinality ParseCardinalityStr(const std::string& cardinalityStr);

class RelationshipElementRep
{
public:
    RelationshipElementRep(RelationshipElement* relationshipElement_);
    virtual ~RelationshipElementRep();
    virtual RelationshipElementRep* Clone(RelationshipElement* relationshipElement) const = 0;
    virtual void Draw(wing::Graphics& graphics) = 0;
    virtual void DrawSelected(wing::Graphics& graphics);
    virtual bool Contains(const wing::PointF& location) const;
    virtual float SourceSymbolWidth() const { return 0.0f; }
    virtual float TargetSymbolWidth() const { return 0.0f; }
    RelationshipElement* GetRelationshipElement() const { return relationshipElement; }
private:
    RelationshipElement* relationshipElement;
};

class RelationshipElement : public DiagramElement
{
public:
    RelationshipElement();
    RelationshipElement(RelationshipKind rkind_);
    soul::xml::Element* ToXml() const override;
    void Parse(soul::xml::Element* xmlElement) override;
    DiagramElement* Clone() const override;
    RelationshipKind RKind() const { return rkind; }
    void SetRKind(RelationshipKind rkind_);
    Cardinality GetCardinality() const { return cardinality; }
    void SetCardinality(Cardinality cardinality_) { cardinality = cardinality_; }
    const EndPoint& Source() const { return source; }
    EndPoint& Source() { return source; }
    const EndPoint& Target() const { return target; }
    EndPoint& Target() { return target; }
    std::vector<EndPoint>& SourceEndPoints() { return sourceEndPoints; }
    const std::vector<EndPoint>& SourceEndPoints() const { return sourceEndPoints; }
    const std::vector<wing::PointF>& RoutingPoints() const { return routingPoints; }
    std::vector<wing::PointF>& RoutingPoints() { return routingPoints; }
    wing::PointF LastPoint() const;
    void SetLastPoint(const wing::PointF& lastPoint);
    Line StartLine() const;
    Line EndLine() const;
    void Measure(wing::Graphics& graphics) override;
    void Draw(wing::Graphics& graphics) override;
    void DrawSelected(wing::Graphics& graphics) override;
    wing::RectF Bounds() const override;
    void Offset(float dx, float dy) override;
    bool IntersectsWith(const wing::RectF& rect) const override;
    bool Contains(const wing::PointF& location) const override;
    void SetCompoundLocation(const CompoundLocation& compoundLocation) override;
    CompoundLocation GetCompoundLocation() const override;
    void AddActions(Diagram* diagram, int elementIndex, wing::ContextMenu* contextMenu) const override;
    bool IsInheritance() const { return rkind == RelationshipKind::inheritance; }
    bool IsCombinedInheritance() const { return rkind == RelationshipKind::combinedInheritance; }
    bool IsComposition() const { return rkind == RelationshipKind::composition; }
    bool IsAggregation() const { return rkind == RelationshipKind::aggregation; }
    bool IsReference() const { return rkind == RelationshipKind::reference; }
    bool IsCreateInstance() const { return rkind == RelationshipKind::createInstance; }
    bool IsAttachNote() const { return rkind == RelationshipKind::attachNote; }
    void RemoveContainer(ContainerElement* container);
    void AddToElements();
    void RemoveFromElements();
    void SetContainerElementIndeces(const std::map<ContainerElement*, int>& containerElementIndexMap);
    void MapContainerElements(const std::map<DiagramElement*, DiagramElement*>& cloneMap, bool& orphan);
    void Resolve(Diagram* diagram);
    void Calculate(const Connector& connector, DiagramElement* element, float w, int index, int count);
    void Straighten();
    void SetCenterConnectors();
    int MainDirection() const;
    void MapIndeces(const std::map<int, int>& indexMap);
private:
    void SetRep();
    void SetTextLocations();
    RelationshipKind rkind;
    Cardinality cardinality;
    EndPoint source;
    EndPoint target;
    std::vector<EndPoint> sourceEndPoints;
    std::vector<wing::PointF> routingPoints;
    std::unique_ptr<RelationshipElementRep> rep;
};

} // namespace springpp
