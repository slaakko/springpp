// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

export module springpp.operation_element;

import springpp.diagram_element;
import springpp.end_point;
import springpp.connector;
import wing;
import std.core;

export namespace springpp {

class OperationElement;
class ContainerElement;
class RelationshipElement;
class OperationLayoutElement;
class Layout;

class OperationElementRep
{
public:
    OperationElementRep(OperationElement* operationElement_);
    virtual ~OperationElementRep();
    OperationElement* GetOperationElement() const { return operationElement; }
    void Measure(wing::Graphics& graphics);
    void Draw(wing::Graphics& graphics);
private:
    OperationElement* operationElement;
    virtual OperationLayoutElement* GetOperationLayout(Layout* layout) const = 0;
};

class ConcreteOperation : public OperationElementRep
{
public:
    ConcreteOperation(OperationElement* operationElement_);
private:
    OperationLayoutElement* GetOperationLayout(Layout* layout) const override;
};

class AbstractOperation : public OperationElementRep
{
public:
    AbstractOperation(OperationElement* operationElement_);
private:
    OperationLayoutElement* GetOperationLayout(Layout* layout) const override;
};

class OperationElement : public DiagramElement
{
public:
    OperationElement();
    DiagramElement* Clone() const override;
    void SetContainerElement(ContainerElement* containerElement_) { containerElement = containerElement_; }
    ContainerElement* GetContainerElement() const override { return containerElement; }
    void AddRelationship(RelationshipElement* relationship_) override;
    void RemoveRelationship(RelationshipElement* relationship_) override;
    EndPoint GetEndPoint(const Connector& connector) const override;
    RelationshipElement* Relationship() const { return relationship; }
    bool IsAbstract() const { return isAbstract; }
    void SetAbstract();
    void ResetAbstract();
    void SetRelationshipPoint();
    wing::PointF GetRelationshipPoint() const;
    void Measure(wing::Graphics& graphics) override;
    void Draw(wing::Graphics& graphics) override;
    soul::xml::Element* ToXml() const override;
    void Parse(soul::xml::Element* xmlElement) override;
private:
    bool isAbstract;
    std::unique_ptr<OperationElementRep> rep;
    ContainerElement* containerElement;
    RelationshipElement* relationship;
};

} // namespace springpp
