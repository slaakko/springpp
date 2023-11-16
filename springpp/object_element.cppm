// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

export module springpp.object_element;

import springpp.container_element;
import springpp.attribute_element;
import springpp.index_list;
import wing;
import std.core;

export namespace springpp {

class ObjectLayoutElement;

class ObjectElement : public ContainerElement
{
public:
    ObjectElement();
    ObjectElement(const std::string& name_);
    soul::xml::Element* ToXml() const override;
    void Parse(soul::xml::Element* xmlElement) override;
    void Measure(wing::Graphics& graphics) override;
    void Draw(wing::Graphics& graphics) override;
    DiagramElement* Clone() const override;
    void AddActions(Diagram* diagram, int elementIndex, wing::ContextMenu* contextMenu) const override;
    const IndexList<AttributeElement>& Attributes() const { return attributes; }
    IndexList<AttributeElement>& Attributes() { return attributes; }
    void SetAttributes(IndexList<AttributeElement>&& attributes_);
    AttributeElement* GetAttribute(int attributeIndex) const override;
    int GetIndexOfAttributeElement(AttributeElement* attributeElement) const override;
    std::vector<RelationshipElement*> GetAllRelationships() const override;
    void MapChildObjects(ContainerElement* from, std::map<DiagramElement*, DiagramElement*>& cloneMap) override;
    std::vector<EndPoint> GetEndPoints(EndPointKind endPointKind, Tool* tool) const override;
    float GetMaxChildElementWidth() const override;
private:
    void SetSize();
    void MeasureAttributes(wing::Graphics& graphics, ObjectLayoutElement* objectLayout, bool& hasRelationship);
    void SetRelationshipPoints();
    void DrawFrame(wing::Graphics& graphics, ObjectLayoutElement* objectLayout);
    void DrawCaption(wing::Graphics& graphics, ObjectLayoutElement* objectLayout);
    void DrawAttributes(wing::Graphics& graphics, ObjectLayoutElement* objectLayout);
    IndexList<AttributeElement> attributes;
    float captionTextHeight;
    wing::RectF captionRect;
    wing::RectF attributeRect;
    float maxChildElementWidth;
};

} // namespace springpp
