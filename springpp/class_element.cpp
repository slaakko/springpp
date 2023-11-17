// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

module springpp.class_element;

import springpp.configuration;
import springpp.action;
import springpp.tool;
import soul.xml.xpath;

namespace springpp {

ClassElementRep::ClassElementRep(ClassElement* classElement_) : classElement(classElement_), captionTextHeight(0.0f), maxChildElementWidth(0.0f)
{
}

ClassElementRep::~ClassElementRep() 
{
}

void ClassElementRep::Measure(wing::Graphics& graphics)
{
    ClassElement* classElement = GetClassElement();
    Layout* layout = Configuration::Instance().GetLayout();
    ClassLayoutElement* classLayout = GetClassLayout(layout);
    RelationshipLayoutElement* relationshipLayout = layout->GetRelationshipLayoutElement();
    PaddingElement* paddingElement = classLayout->GetPaddingElement();
    wing::Font* font = classLayout->GetCaptionElement()->GetFontElement()->GetFont();
    wing::RectF r = wing::MeasureString(graphics, classElement->Name(), *font, wing::PointF(0, 0), layout->GetStringFormat());
    wing::SizeF sz;
    r.GetSize(&sz);
    captionTextHeight = sz.Height;
    float captionRectX = classElement->Location().X;
    float captionRectY = classElement->Location().Y;
    float captionRectWidth = paddingElement->GetPadding().Horizontal() + sz.Width;
    float captionRectHeight = paddingElement->GetPadding().Vertical() + sz.Height;
    captionRect = wing::RectF(captionRectX, captionRectY, captionRectWidth, captionRectHeight);
    maxChildElementWidth = 0.0f;
    bool hasRelationship = false;
    MeasureOperations(graphics, classLayout, hasRelationship);
    MeasureAttributes(graphics, classLayout, hasRelationship);
    if (hasRelationship)
    {
        float w = GetRelationshipSymbolFieldWidth(relationshipLayout->RelationshipSymbolRadius(), relationshipLayout->GetPaddingElement()->GetPadding().Horizontal());
        captionRect.Width += w;
        operationRect.Width += w;
        attributeRect.Width += w;
        classElement->SetRelationshipPoints();
    }
    captionRect = wing::RectF(captionRectX, captionRectY, captionRectWidth, captionRectHeight);
    SetSize();
}

void ClassElementRep::MeasureOperations(wing::Graphics& graphics, ClassLayoutElement* classLayout, bool& hasRelationship)
{
    PaddingElement* paddingElement = classLayout->GetPaddingElement();
    wing::PointF location = classElement->Location();
    operationRect = wing::RectF();
    operationRect.X = location.X;
    operationRect.Y = location.Y + captionRect.Height;
    operationRect.Width = captionRect.Width;
    operationRect.Height = paddingElement->GetPadding().top;
    wing::PointF origin;
    origin.X = operationRect.X + paddingElement->GetPadding().left;
    origin.Y = operationRect.Y + paddingElement->GetPadding().top;
    int n = classElement->Operations().Count();
    for (int i = 0; i < n; ++i)
    {
        OperationElement* operation = classElement->Operations().Get(i);
        operation->Measure(graphics);
        wing::SizeF operationSize = operation->Size();
        float w = operationSize.Width + paddingElement->GetPadding().Horizontal();
        float h = operationSize.Height;
        maxChildElementWidth = std::max(maxChildElementWidth, w);
        if (operation->Relationship() != nullptr)
        {
            hasRelationship = true;
        }
        operation->SetLocation(origin);
        operationRect.Width = std::max(operationRect.Width, w);
        origin.Y += h;
        operationRect.Height += h;
    }
    operationRect.Height += paddingElement->GetPadding().bottom;
}

void ClassElementRep::MeasureAttributes(wing::Graphics& graphics, ClassLayoutElement* classLayout, bool& hasRelationship)
{
    PaddingElement* paddingElement = classLayout->GetPaddingElement();
    wing::PointF location = classElement->Location();
    attributeRect = wing::RectF();
    attributeRect.X = location.X;
    attributeRect.Y = operationRect.Y + operationRect.Height;
    attributeRect.Width = operationRect.Width;
    attributeRect.Height = paddingElement->GetPadding().top;
    wing::PointF origin;
    origin.X = attributeRect.X + paddingElement->GetPadding().left;
    origin.Y = attributeRect.Y + paddingElement->GetPadding().top;
    int n = classElement->Attributes().Count();
    for (int i = 0; i < n; ++i)
    {
        AttributeElement* attribute = classElement->Attributes().Get(i);
        attribute->Measure(graphics);
        wing::SizeF attributeSize = attribute->Size();
        float w = attributeSize.Width + paddingElement->GetPadding().Horizontal();
        float h = attributeSize.Height;
        maxChildElementWidth = std::max(maxChildElementWidth, w);
        if (attribute->Relationship() != nullptr)
        {
            hasRelationship = true;
        }
        attribute->SetLocation(origin);
        attributeRect.Width = std::max(attributeRect.Width, w);
        origin.Y += h;
        attributeRect.Height += h;
    }
    attributeRect.Height += paddingElement->GetPadding().bottom;
}

void ClassElementRep::SetSize()
{
    ClassElement* classElement = GetClassElement();
    wing::SizeF captionRectSize;
    captionRect.GetSize(&captionRectSize);
    wing::SizeF size(0.0f, 0.0f);
    size.Width = std::max(size.Width, captionRectSize.Width);
    float h = captionRectSize.Height;
    if (!classElement->Operations().IsEmpty() || !classElement->Attributes().IsEmpty())
    {
        wing::SizeF operationRectSize;
        operationRect.GetSize(&operationRectSize);
        size.Width = std::max(size.Width, operationRectSize.Width);
        h = h + operationRectSize.Height;
    }
    if (!classElement->Attributes().IsEmpty())
    {
        wing::SizeF attributeRectSize;
        attributeRect.GetSize(&attributeRectSize);
        size.Width = std::max(size.Width, attributeRectSize.Width);
        h = h + attributeRectSize.Height;
    }
    size.Height = std::max(size.Height, h);
    classElement->SetSize(size);
}

void ClassElementRep::Draw(wing::Graphics& graphics)
{
    Layout* layout = Configuration::Instance().GetLayout();
    ClassLayoutElement* classLayout = GetClassLayout(layout);
    DrawFrame(graphics, classLayout);
    DrawCaption(graphics, classLayout);
    DrawOperations(graphics);
    DrawAttributes(graphics);
}

void ClassElementRep::DrawFrame(wing::Graphics& graphics, ClassLayoutElement* classLayout)
{
    ClassElement* classElement = GetClassElement();
    PaddingElement* paddingElement = classLayout->GetPaddingElement();
    graphics.DrawRectangle(classLayout->FramePen(), classElement->Bounds());
    if (!classElement->Operations().IsEmpty() || !classElement->Attributes().IsEmpty())
    {
        float captionLineY = paddingElement->GetPadding().Vertical() + captionTextHeight;
        wing::PointF captionLineStart = wing::PointF(classElement->Location().X, classElement->Location().Y + captionLineY);
        wing::PointF captionLineEnd = wing::PointF(classElement->Location().X + classElement->Size().Width, classElement->Location().Y + captionLineY);
        graphics.DrawLine(classLayout->FramePen(), captionLineStart, captionLineEnd);
    }
    if (!classElement->Attributes().IsEmpty())
    {
        wing::PointF attributeLineStart = wing::PointF(classElement->Location().X, attributeRect.Y);
        wing::PointF attributeLineEnd = wing::PointF(classElement->Location().X + classElement->Size().Width, attributeRect.Y);
        graphics.DrawLine(classLayout->FramePen(), attributeLineStart, attributeLineEnd);
    }
}

void ClassElementRep::DrawCaption(wing::Graphics& graphics, ClassLayoutElement* classLayout)
{
    ClassElement* classElement = GetClassElement();
    PaddingElement* paddingElement = classLayout->GetPaddingElement();
    CaptionElement* captionElement = classLayout->GetCaptionElement();
    wing::PointF location = classElement->Location();
    wing::PointF origin(location.X + paddingElement->GetPadding().left, location.Y + paddingElement->GetPadding().top);
    wing::Font* font = captionElement->GetFontElement()->GetFont();
    wing::Brush* brush = classLayout->GetTextColorElement()->GetBrush();
    wing::DrawString(graphics, classElement->Name(), *font, origin, *brush);
}

void ClassElementRep::DrawOperations(wing::Graphics& graphics)
{
    int n = classElement->Operations().Count();
    for (int i = 0; i < n; ++i)
    {
        OperationElement* operation = classElement->Operations().Get(i);
        operation->Draw(graphics);
    }
}

void ClassElementRep::DrawAttributes(wing::Graphics& graphics)
{
    int n = classElement->Attributes().Count();
    for (int i = 0; i < n; ++i)
    {
        AttributeElement* attribute = classElement->Attributes().Get(i);
        attribute->Draw(graphics);
    }
}

class ConcreteClassElementRep : public ClassElementRep
{
public:
    ConcreteClassElementRep(ClassElement* classElement_);
    ClassLayoutElement* GetClassLayout(Layout* layout) const override;
};

ConcreteClassElementRep::ConcreteClassElementRep(ClassElement* classElement_) : ClassElementRep(classElement_)
{
}

ClassLayoutElement* ConcreteClassElementRep::GetClassLayout(Layout* layout) const
{
    return layout->GetConcreteClassLayoutElement();
}

class AbstractClassElementRep : public ClassElementRep
{
public:
    AbstractClassElementRep(ClassElement* classElement_);
    ClassLayoutElement* GetClassLayout(Layout* layout) const override;
};

AbstractClassElementRep::AbstractClassElementRep(ClassElement* classElement_) : ClassElementRep(classElement_)
{
}

ClassLayoutElement* AbstractClassElementRep::GetClassLayout(Layout* layout) const
{
    return layout->GetAbstractClassLayoutElement();
}

ClassElement::ClassElement() : ContainerElement(DiagramElementKind::classElement), isAbstract(false), rep(new ConcreteClassElementRep(this))
{
}

soul::xml::Element* ClassElement::ToXml() const
{
    soul::xml::Element* xmlElement = soul::xml::MakeElement("classElement");
    soul::xml::Element* boundsElement = soul::xml::MakeElement("bounds");
    wing::RectF bounds = Bounds();
    boundsElement->SetAttribute("x", std::to_string(bounds.X));
    boundsElement->SetAttribute("y", std::to_string(bounds.Y));
    boundsElement->SetAttribute("width", std::to_string(bounds.Width));
    boundsElement->SetAttribute("height", std::to_string(bounds.Height));
    xmlElement->AppendChild(boundsElement);
    xmlElement->SetAttribute("name", Name());
    xmlElement->SetAttribute("abstract", isAbstract ? "true" : "false");
    for (const auto& operation : operations)
    {
        soul::xml::Element* operationElement = operation->ToXml();
        xmlElement->AppendChild(operationElement);
    }
    for (const auto& attribute : attributes)
    {
        soul::xml::Element* attributeElement = attribute->ToXml();
        xmlElement->AppendChild(attributeElement);
    }
    return xmlElement;
}

void ClassElement::Parse(soul::xml::Element* xmlElement)
{
    std::unique_ptr<soul::xml::xpath::NodeSet> nodeSet = soul::xml::xpath::EvaluateToNodeSet("bounds", xmlElement);
    if (nodeSet->Count() == 1)
    {
        soul::xml::Node* node = nodeSet->GetNode(0);
        if (node->IsElementNode())
        {
            wing::RectF bounds;
            soul::xml::Element* boundsElement = static_cast<soul::xml::Element*>(node);
            std::string xStr = boundsElement->GetAttribute("x");
            if (!xStr.empty())
            {
                bounds.X = std::stof(xStr);
            }
            else
            {
                throw std::runtime_error("XML element 'classElement' has no 'x' attribute");
            }
            std::string yStr = boundsElement->GetAttribute("y");
            if (!yStr.empty())
            {
                bounds.Y = std::stof(yStr);
            }
            else
            {
                throw std::runtime_error("XML element 'classElement' has no 'y' attribute");
            }
            std::string widthStr = boundsElement->GetAttribute("width");
            if (!widthStr.empty())
            {
                bounds.Width = std::stof(widthStr);
            }
            else
            {
                throw std::runtime_error("XML element 'classElement' has no 'width' attribute");
            }
            std::string heightStr = boundsElement->GetAttribute("height");
            if (!heightStr.empty())
            {
                bounds.Height = std::stof(heightStr);
            }
            else
            {
                throw std::runtime_error("XML element 'classElement' has no 'height' attribute");
            }
            SetBounds(bounds);
        }
        else
        {
            throw std::runtime_error("XML element node expected in '" + xmlElement->Name() + "'");
        }
    }
    else
    {
        throw std::runtime_error("XML element 'bounds' not unique in '" + xmlElement->Name() + "'");
    }
    std::string nameStr = xmlElement->GetAttribute("name");
    if (!nameStr.empty())
    {
        SetName(nameStr);
    }
    else
    {
        throw std::runtime_error("XML element 'classElement' has no 'name' attribute");
    }
    std::string abstractStr = xmlElement->GetAttribute("abstract");
    if (abstractStr == "true")
    {
        SetAbstract();
    }
    std::unique_ptr<soul::xml::xpath::NodeSet> operationNodeSet = soul::xml::xpath::EvaluateToNodeSet("operation", xmlElement);
    int no = operationNodeSet->Count();
    for (int i = 0; i < no; ++i)
    {
        soul::xml::Node* node = operationNodeSet->GetNode(i);
        if (node->IsElementNode())
        {
            soul::xml::Element* xmlElement = static_cast<soul::xml::Element*>(node);
            OperationElement* operationElement = new OperationElement();
            operationElement->SetContainerElement(this);
            operationElement->Parse(xmlElement);
            operations.Add(operationElement);
        }
        else
        {
            throw std::runtime_error("XML element node expected in '" + xmlElement->Name() + "'");
        }
    }
    std::unique_ptr<soul::xml::xpath::NodeSet> attributeNodeSet = soul::xml::xpath::EvaluateToNodeSet("attribute", xmlElement);
    int nf = attributeNodeSet->Count();
    for (int i = 0; i < nf; ++i)
    {
        soul::xml::Node* node = attributeNodeSet->GetNode(i);
        if (node->IsElementNode())
        {
            soul::xml::Element* xmlElement = static_cast<soul::xml::Element*>(node);
            AttributeElement* attributeElement = new AttributeElement();
            attributeElement->SetContainerElement(this);
            attributeElement->Parse(xmlElement);
            attributes.Add(attributeElement);
        }
        else
        {
            throw std::runtime_error("XML element node expected in '" + xmlElement->Name() + "'");
        }
    }
}

AttributeElement* ClassElement::GetAttribute(int attributeIndex) const
{
    return attributes.Get(attributeIndex);
}

int ClassElement::GetIndexOfAttributeElement(AttributeElement* attributeElement) const
{
    int n = attributes.Count();
    for (int i = 0; i < n; ++i)
    {
        if (attributes.Get(i) == attributeElement) return i;
    }
    throw std::runtime_error("attribute '" + attributeElement->Name() + "' not found from class '" + Name() + "'");
}

OperationElement* ClassElement::GetOperation(int operationIndex) const
{
    return operations.Get(operationIndex);
}

int ClassElement::GetIndexOfOperationElement(OperationElement* operationElement) const
{
    int n = operations.Count();
    for (int i = 0; i < n; ++i)
    {
        if (operations.Get(i) == operationElement) return i;
    }
    throw std::runtime_error("operatiom '" + operationElement->Name() + "' not found from class '" + Name() + "'");
}

void ClassElement::Measure(wing::Graphics& graphics)
{
    ContainerElement::Measure(graphics);
    rep->Measure(graphics);
}

void ClassElement::Draw(wing::Graphics& graphics)
{
    ContainerElement::Draw(graphics);
    rep->Draw(graphics);
}

DiagramElement* ClassElement::Clone() const
{
    ClassElement* clone = new ClassElement();
    clone->SetName(Name());
    clone->SetBounds(Bounds());
    if (isAbstract)
    {
        clone->SetAbstract();
    }
    int no = operations.Count();
    for (int i = 0; i < no; ++i)
    {
        OperationElement* operationElement = operations.Get(i);
        OperationElement* clonedOperation = static_cast<OperationElement*>(operationElement->Clone());
        clonedOperation->SetContainerElement(clone);
        clone->Operations().Add(clonedOperation);
    }
    int nf = attributes.Count();
    for (int i = 0; i < nf; ++i)
    {
        AttributeElement* attributeElement = attributes.Get(i);
        AttributeElement* clonedAttribute = static_cast<AttributeElement*>(attributeElement->Clone());
        clonedAttribute->SetContainerElement(clone);
        clone->Attributes().Add(clonedAttribute);
    }
    return clone;
}

void ClassElement::SetAbstract()
{
    if (!isAbstract)
    {
        isAbstract = true;
        rep.reset(new AbstractClassElementRep(this));
    }
}

void ClassElement::ResetAbstract()
{
    if (isAbstract)
    {
        isAbstract = false;
        rep.reset(new ConcreteClassElementRep(this));
    }
}

void ClassElement::ResetAbstractOperations()
{
    int n = operations.Count();
    for (int i = 0; i < n; ++i)
    {
        OperationElement* operation = operations.Get(i);
        operation->ResetAbstract();
    }
}

void ClassElement::SetOperations(IndexList<OperationElement>&& operations_)
{
    operations = std::move(operations_);
    for (auto& operation : operations)
    {
        operation->SetContainerElement(this);
    }
}

void ClassElement::SetAttributes(IndexList<AttributeElement>&& attributes_)
{
    attributes = std::move(attributes_);
    for (auto& attribute : attributes)
    {
        attribute->SetContainerElement(this);
    }
}

std::vector<RelationshipElement*> ClassElement::GetAllRelationships() const
{
    std::vector<RelationshipElement*> relationships = Relationships();
    int no = operations.Count();
    for (int i = 0; i < no; ++i)
    {
        OperationElement* operation = operations.Get(i);
        RelationshipElement* relationship = operation->Relationship();
        if (relationship)
        {
            relationships.push_back(relationship);
        }
    }
    int nf = attributes.Count();
    for (int i = 0; i < nf; ++i)
    {
        AttributeElement* attribute = attributes.Get(i);
        RelationshipElement* relationship = attribute->Relationship();
        if (relationship)
        {
            relationships.push_back(relationship);
        }
    }
    return relationships;
}

float ClassElement::GetMaxChildElementWidth() const
{
    return rep->MaxChildElementWidth();
}

void ClassElement::MapChildObjects(ContainerElement* from, std::map<DiagramElement*, DiagramElement*>& cloneMap)
{
    if (from->IsClassElement())
    {
        ClassElement* fromClass = static_cast<ClassElement*>(from);
        int no = fromClass->operations.Count();
        for (int i = 0; i < no; ++i)
        {
            cloneMap[fromClass->operations.Get(i)] = operations.Get(i);
        }
        int nf = fromClass->attributes.Count();
        for (int i = 0; i < nf; ++i)
        {
            cloneMap[fromClass->attributes.Get(i)] = attributes.Get(i);
        }
    }
}

std::vector<EndPoint> ClassElement::GetEndPoints(EndPointKind endPointKind, Tool* tool) const
{
    if (endPointKind == EndPointKind::source && tool->IsAttachNoteTool())
    {
        std::vector<EndPoint> endPoints;
        int no = operations.Count();
        for (int i = 0; i < no; ++i)
        {
            OperationElement* operation = operations.Get(i);
            endPoints.push_back(operation->GetEndPoint(Connector(Connector::Operation(i))));
        }
        int nf = attributes.Count();
        for (int i = 0; i < nf; ++i)
        {
            AttributeElement* attribute = attributes.Get(i);
            endPoints.push_back(attribute->GetEndPoint(Connector(Connector::Attribute(i))));
        }
        return endPoints;
    }
    else
    {
        return ContainerElement::GetEndPoints(endPointKind, tool);
    }
}

void ClassElement::AddActions(Diagram* diagram, int elementIndex, wing::ContextMenu* contextMenu) const
{
    ContainerElement::AddActions(diagram, elementIndex, contextMenu);
    wing::MenuItem* propertiesMenuItem = new wing::MenuItem("Properties...");
    contextMenu->AddMenuItem(propertiesMenuItem);
    contextMenu->AddAction(new ClassElementPropertiesAction(diagram, elementIndex, propertiesMenuItem));
}

void ClassElement::SetRelationshipPoints()
{
    int no = operations.Count();
    for (int i = 0; i < no; ++i)
    {
        OperationElement* operation = operations.Get(i);
        operation->SetRelationshipPoint();
    }
    int nf = attributes.Count();
    for (int i = 0; i < nf; ++i)
    {
        AttributeElement* attribute = attributes.Get(i);
        attribute->SetRelationshipPoint();
    }
}

} // namespace springpp
