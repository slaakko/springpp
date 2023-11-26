// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

export module springpp.container_element;

import springpp.diagram_element;
import wing;
import std.core;

export namespace springpp {

class RelationshipElement;
class AttributeElement;
class OperationElement;

class ContainerElement : public DiagramElement
{
public:
    ContainerElement(DiagramElementKind kind_);
    ContainerElement(DiagramElementKind kind_, const std::string& name_);
    ~ContainerElement();
    virtual std::vector<RelationshipElement*> GetAllRelationships() const = 0;
    virtual AttributeElement* GetAttribute(int attributeIndex) const;
    virtual int GetIndexOfAttributeElement(AttributeElement* attributeElement) const;
    virtual OperationElement* GetOperation(int operationIndex) const;
    virtual int GetIndexOfOperationElement(OperationElement* operationElement) const;
    virtual float GetMaxChildElementWidth() const { return 0.0f; }
    virtual void MapChildObjects(ContainerElement* from, std::map<DiagramElement*, DiagramElement*>& cloneMap, std::map<DiagramElement*, DiagramElement*>& revereseCloneMap) {}
    ContainerElement* GetContainerElement() const override { return const_cast<ContainerElement*>(this); }
    void AddRelationship(RelationshipElement* relationship_) override;
    void RemoveRelationship(RelationshipElement* relationship_) override;
    const std::vector<RelationshipElement*>& Relationships() const { return relationships; }
    EndPoint GetEndPoint(const Connector& connector) const override;
    std::vector<EndPoint> GetEndPoints(EndPointKind endPointKind, Tool* tool) const override;
    void Measure(wing::Graphics& graphics) override;
private:
    void MeasureRelationships(const Connector& connector, float w);
    std::vector<RelationshipElement*> PickRelationships(const Connector& connector) const;
    std::vector<RelationshipElement*> relationships;
};

} // namespace springpp
