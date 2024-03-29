// =================================
// Copyright (c) 2024 Seppo Laakko
// Distributed under the MIT license
// =================================

export module springpp.class_element;

import springpp.container_element;
import springpp.operation_element;
import springpp.attribute_element;
import springpp.index_list;
import wing;
import std.core;

export namespace springpp {

class ClassElement;
class ClassLayoutElement;
class Layout;

class ClassElementRep
{
public:
    ClassElementRep(ClassElement* classElement_);
    virtual ~ClassElementRep();
    ClassElement* GetClassElement() const { return classElement; }
    void Measure(wing::Graphics& graphics);
    void Draw(wing::Graphics& graphics);
    float MaxChildElementWidth() const { return maxChildElementWidth; }
private:
    void MeasureAttributes(wing::Graphics& graphics, ClassLayoutElement* classLayout, bool& hasRelationship);
    void MeasureOperations(wing::Graphics& graphics, ClassLayoutElement* classLayout, bool& hasRelationship);
    void DrawFrame(wing::Graphics& graphics, ClassLayoutElement* classLayout);
    void DrawCaption(wing::Graphics& graphics, ClassLayoutElement* classLayout);
    void DrawOperations(wing::Graphics& graphics);
    void DrawAttributes(wing::Graphics& graphics);
    void SetSize();
    virtual ClassLayoutElement* GetClassLayout(Layout* layout) const = 0;
    ClassElement* classElement;
    wing::SizeF keywordTextSize;
    wing::SizeF nameTextSize;
    float captionTextHeight;
    wing::RectF captionRect;
    wing::RectF attributeRect;
    wing::RectF operationRect;
    float maxChildElementWidth;
};

class ConcreteClassElement : public ClassElementRep
{
public:
    ConcreteClassElement(ClassElement* classElement_);
    ClassLayoutElement* GetClassLayout(Layout* layout) const override;
};

class AbstractClassElement : public ClassElementRep
{
public:
    AbstractClassElement(ClassElement* classElement_);
    ClassLayoutElement* GetClassLayout(Layout* layout) const override;
};

class ClassElement : public ContainerElement
{
public:
    ClassElement();
    soul::xml::Element* ToXml() const override;
    void Parse(soul::xml::Element* xmlElement) override;
    void Measure(wing::Graphics& graphics) override;
    void Draw(wing::Graphics& graphics) override;
    DiagramElement* Clone() const override;
    bool IsAbstract() const { return isAbstract; }
    void SetAbstract();
    void ResetAbstract();
    void ResetAbstractOperations();
    const std::string& Keyword() const { return keyword; }
    void SetKeyword(const std::string& keyword_);
    const IndexList<OperationElement>& Operations() const { return operations; }
    IndexList<OperationElement>& Operations() { return operations; }
    void SetOperations(IndexList<OperationElement>&& operations_);
    const IndexList<AttributeElement>& Attributes() const { return attributes; }
    IndexList<AttributeElement>& Attributes() { return attributes; }
    void SetAttributes(IndexList<AttributeElement>&& attributes_);
    AttributeElement* GetAttribute(int attributeIndex) const override;
    int GetIndexOfAttributeElement(AttributeElement* attributeElement) const override;
    OperationElement* GetOperation(int operationIndex) const override;
    int GetIndexOfOperationElement(OperationElement* operationElement) const override;
    std::vector<RelationshipElement*> GetAllRelationships() const override;
    void MapChildObjects(ContainerElement* from, std::map<DiagramElement*, DiagramElement*>& cloneMap, std::map<DiagramElement*, DiagramElement*>& reverseCloneMap) override;
    float GetMaxChildElementWidth() const override;
    std::vector<EndPoint> GetEndPoints(EndPointKind endPointKind, Tool* tool) const override;
    void AddActions(Diagram* diagram, int elementIndex, wing::ContextMenu* contextMenu) const override;
    void SetRelationshipPoints();
private:
    bool isAbstract;
    std::string keyword;
    std::unique_ptr<ClassElementRep> rep;
    IndexList<OperationElement> operations;
    IndexList<AttributeElement> attributes;
};

} // namespace springpp
