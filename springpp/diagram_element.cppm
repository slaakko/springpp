// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

export module springpp.diagram_element;

import springpp.primitive;
import springpp.end_point;
import springpp.connector;
import soul.xml.dom;
import wing;
import std.core;

export namespace springpp {

class ContainerElement;
class RelationshipElement;
class Tool;

enum class EndPointKind
{
    source, target
};

enum class RelationshipKind
{
    none, inheritance, combinedInheritance, composition, aggregation, reference, createInstance, attachNote
};

std::string RelationshipKindStr(RelationshipKind rkind);
RelationshipKind ParseRelationshipKindStr(const std::string& relationshipKindStr);

enum class DiagramElementKind
{
    classElement, objectElement, noteElement, operationElement, attributeElement, textElement, relationshipElement
};

enum class DiagramElementFlags
{
    none = 0, selected = 1 << 0
};

constexpr DiagramElementFlags operator|(DiagramElementFlags left, DiagramElementFlags right)
{
    return DiagramElementFlags(int(left) | int(right));
}

constexpr DiagramElementFlags operator&(DiagramElementFlags left, DiagramElementFlags right)
{
    return DiagramElementFlags(int(left) & int(right));
}

constexpr DiagramElementFlags operator~(DiagramElementFlags flags)
{
    return DiagramElementFlags(~int(flags));
}
 
class DiagramElement
{
public:
    DiagramElement(DiagramElementKind kind_);
    DiagramElementKind Kind() const { return kind; }
    virtual void Measure(wing::Graphics& graphics);
    virtual void Draw(wing::Graphics& graphics);
    virtual void DrawSelected(wing::Graphics& graphics);
    virtual DiagramElement* Clone() const = 0;
    virtual soul::xml::Element* ToXml() const = 0;
    virtual void Parse(soul::xml::Element* xmlElement) = 0;
    virtual void Offset(float dx, float dy);
    virtual void SetCompoundLocation(const CompoundLocation& compoundLocation);
    virtual CompoundLocation GetCompoundLocation() const;
    virtual wing::RectF Bounds() const { return bounds; }
    virtual ContainerElement* GetContainerElement() const { return nullptr; }
    virtual void AddRelationship(RelationshipElement* relationship) {}
    virtual void RemoveRelationship(RelationshipElement* relationship) {}
    virtual EndPoint GetEndPoint(const Connector& connector) const { return EndPoint(); }
    virtual std::vector<EndPoint> GetEndPoints(EndPointKind endPointKind, Tool* tool) const { return std::vector<EndPoint>(); }
    virtual void AddActions(Diagram* diagram, int elementIndex, wing::ContextMenu* contextMenu) const;
    virtual bool IntersectsWith(const wing::RectF& rect) const;
    virtual bool Contains(const wing::PointF& location) const;
    wing::PointF Location() const;
    void SetLocation(const wing::PointF& location);
    wing::SizeF Size() const;
    void SetSize(const wing::SizeF& size);
    void SetBounds(const wing::RectF& bounds_);
    wing::PointF Center() const;
    void SetName(const std::string& name_);
    const std::string& Name() const { return name; }
    bool IsSelected() const { return (flags & DiagramElementFlags::selected) != DiagramElementFlags::none; }
    void SetSelected() { flags = flags | DiagramElementFlags::selected; }
    void Select();
    void ResetSelected();
    bool IsClassElement() const { return kind == DiagramElementKind::classElement; }
    bool IsObjectElement() const { return kind == DiagramElementKind::objectElement; }
    bool IsNoteElement() const { return kind == DiagramElementKind::noteElement; }
    bool IsContainerElement() const { return IsClassElement() || IsObjectElement() || IsNoteElement(); }
    bool IsOperationElement() const { return kind == DiagramElementKind::operationElement; }
    bool IsAttributeElement() const { return kind == DiagramElementKind::attributeElement; }
    bool IsRelationshipElement() const { return kind == DiagramElementKind::relationshipElement; }
    Diagram* GetDiagram() const { return diagram; }
    void SetDiagram(Diagram* diagram_) { diagram = diagram_; }
private:
    DiagramElementKind kind;
    DiagramElementFlags flags;
    wing::RectF bounds;
    std::string name;
    Diagram* diagram;
};

DiagramElement* CreateDiagramElement(const std::string& xmlElementName);

} // namespace springpp
