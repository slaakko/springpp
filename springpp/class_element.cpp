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
    MeasureFields(graphics, classLayout, hasRelationship);
    if (hasRelationship)
    {
        float w = GetRelationshipSymbolFieldWidth(relationshipLayout->RelationshipSymbolRadius(), relationshipLayout->GetPaddingElement()->GetPadding().Horizontal());
        captionRect.Width += w;
        operationRect.Width += w;
        fieldRect.Width += w;
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

void ClassElementRep::MeasureFields(wing::Graphics& graphics, ClassLayoutElement* classLayout, bool& hasRelationship)
{
    PaddingElement* paddingElement = classLayout->GetPaddingElement();
    wing::PointF location = classElement->Location();
    fieldRect = wing::RectF();
    fieldRect.X = location.X;
    fieldRect.Y = operationRect.Y + operationRect.Height;
    fieldRect.Width = operationRect.Width;
    fieldRect.Height = paddingElement->GetPadding().top;
    wing::PointF origin;
    origin.X = fieldRect.X + paddingElement->GetPadding().left;
    origin.Y = fieldRect.Y + paddingElement->GetPadding().top;
    int n = classElement->Fields().Count();
    for (int i = 0; i < n; ++i)
    {
        FieldElement* field = classElement->Fields().Get(i);
        field->Measure(graphics);
        wing::SizeF fieldSize = field->Size();
        float w = fieldSize.Width + paddingElement->GetPadding().Horizontal();
        float h = fieldSize.Height;
        maxChildElementWidth = std::max(maxChildElementWidth, w);
        if (field->Relationship() != nullptr)
        {
            hasRelationship = true;
        }
        field->SetLocation(origin);
        fieldRect.Width = std::max(fieldRect.Width, w);
        origin.Y += h;
        fieldRect.Height += h;
    }
    fieldRect.Height += paddingElement->GetPadding().bottom;
}

void ClassElementRep::SetSize()
{
    ClassElement* classElement = GetClassElement();
    wing::SizeF captionRectSize;
    captionRect.GetSize(&captionRectSize);
    wing::SizeF size(0.0f, 0.0f);
    size.Width = std::max(size.Width, captionRectSize.Width);
    float h = captionRectSize.Height;
    if (!classElement->Operations().IsEmpty() || !classElement->Fields().IsEmpty())
    {
        wing::SizeF operationRectSize;
        operationRect.GetSize(&operationRectSize);
        size.Width = std::max(size.Width, operationRectSize.Width);
        h = h + operationRectSize.Height;
    }
    if (!classElement->Fields().IsEmpty())
    {
        wing::SizeF fieldRectSize;
        fieldRect.GetSize(&fieldRectSize);
        size.Width = std::max(size.Width, fieldRectSize.Width);
        h = h + fieldRectSize.Height;
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
    DrawFields(graphics);
}

void ClassElementRep::DrawFrame(wing::Graphics& graphics, ClassLayoutElement* classLayout)
{
    ClassElement* classElement = GetClassElement();
    PaddingElement* paddingElement = classLayout->GetPaddingElement();
    graphics.DrawRectangle(classLayout->FramePen(), classElement->BoundingRect());
    if (!classElement->Operations().IsEmpty() || !classElement->Fields().IsEmpty())
    {
        float captionLineY = paddingElement->GetPadding().Vertical() + captionTextHeight;
        wing::PointF captionLineStart = wing::PointF(classElement->Location().X, classElement->Location().Y + captionLineY);
        wing::PointF captionLineEnd = wing::PointF(classElement->Location().X + classElement->Size().Width, classElement->Location().Y + captionLineY);
        graphics.DrawLine(classLayout->FramePen(), captionLineStart, captionLineEnd);
    }
    if (!classElement->Fields().IsEmpty())
    {
        wing::PointF fieldLineStart = wing::PointF(classElement->Location().X, fieldRect.Y);
        wing::PointF fieldLineEnd = wing::PointF(classElement->Location().X + classElement->Size().Width, fieldRect.Y);
        graphics.DrawLine(classLayout->FramePen(), fieldLineStart, fieldLineEnd);
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

void ClassElementRep::DrawFields(wing::Graphics& graphics)
{
    int n = classElement->Fields().Count();
    for (int i = 0; i < n; ++i)
    {
        FieldElement* field = classElement->Fields().Get(i);
        field->Draw(graphics);
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
    soul::xml::Element* boundingRectElement = soul::xml::MakeElement("boundingRect");
    wing::RectF boundingRect = BoundingRect();
    boundingRectElement->SetAttribute("x", std::to_string(boundingRect.X));
    boundingRectElement->SetAttribute("y", std::to_string(boundingRect.Y));
    boundingRectElement->SetAttribute("width", std::to_string(boundingRect.Width));
    boundingRectElement->SetAttribute("height", std::to_string(boundingRect.Height));
    xmlElement->AppendChild(boundingRectElement);
    xmlElement->SetAttribute("name", Name());
    xmlElement->SetAttribute("abstract", isAbstract ? "true" : "false");
    for (const auto& operation : operations)
    {
        soul::xml::Element* operationElement = operation->ToXml();
        xmlElement->AppendChild(operationElement);
    }
    for (const auto& field : fields)
    {
        soul::xml::Element* fieldElement = field->ToXml();
        xmlElement->AppendChild(fieldElement);
    }
    return xmlElement;
}

void ClassElement::Parse(soul::xml::Element* xmlElement)
{
    std::unique_ptr<soul::xml::xpath::NodeSet> nodeSet = soul::xml::xpath::EvaluateToNodeSet("boundingRect", xmlElement);
    if (nodeSet->Count() == 1)
    {
        soul::xml::Node* node = nodeSet->GetNode(0);
        if (node->IsElementNode())
        {
            wing::RectF boundingRect;
            soul::xml::Element* boundingRectElement = static_cast<soul::xml::Element*>(node);
            std::string xStr = boundingRectElement->GetAttribute("x");
            if (!xStr.empty())
            {
                boundingRect.X = std::stof(xStr);
            }
            else
            {
                throw std::runtime_error("XML element 'classElement' has no 'x' attribute");
            }
            std::string yStr = boundingRectElement->GetAttribute("y");
            if (!yStr.empty())
            {
                boundingRect.Y = std::stof(yStr);
            }
            else
            {
                throw std::runtime_error("XML element 'classElement' has no 'y' attribute");
            }
            std::string widthStr = boundingRectElement->GetAttribute("width");
            if (!widthStr.empty())
            {
                boundingRect.Width = std::stof(widthStr);
            }
            else
            {
                throw std::runtime_error("XML element 'classElement' has no 'width' attribute");
            }
            std::string heightStr = boundingRectElement->GetAttribute("height");
            if (!heightStr.empty())
            {
                boundingRect.Height = std::stof(heightStr);
            }
            else
            {
                throw std::runtime_error("XML element 'classElement' has no 'height' attribute");
            }
            SetBoundingRect(boundingRect);
        }
        else
        {
            throw std::runtime_error("XML element node expected in '" + xmlElement->Name() + "'");
        }
    }
    else
    {
        throw std::runtime_error("XML element 'boundingRect' not unique in '" + xmlElement->Name() + "'");
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
    std::unique_ptr<soul::xml::xpath::NodeSet> fieldNodeSet = soul::xml::xpath::EvaluateToNodeSet("field", xmlElement);
    int nf = fieldNodeSet->Count();
    for (int i = 0; i < nf; ++i)
    {
        soul::xml::Node* node = fieldNodeSet->GetNode(i);
        if (node->IsElementNode())
        {
            soul::xml::Element* xmlElement = static_cast<soul::xml::Element*>(node);
            FieldElement* fieldElement = new FieldElement();
            fieldElement->SetContainerElement(this);
            fieldElement->Parse(xmlElement);
            fields.Add(fieldElement);
        }
        else
        {
            throw std::runtime_error("XML element node expected in '" + xmlElement->Name() + "'");
        }
    }
}

FieldElement* ClassElement::GetField(int fieldIndex) const
{
    return fields.Get(fieldIndex);
}

int ClassElement::GetIndexOfFieldElement(FieldElement* fieldElement) const
{
    int n = fields.Count();
    for (int i = 0; i < n; ++i)
    {
        if (fields.Get(i) == fieldElement) return i;
    }
    throw std::runtime_error("field '" + fieldElement->Name() + "' not found from class '" + Name() + "'");
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
    clone->SetBoundingRect(BoundingRect());
    if (isAbstract)
    {
        clone->SetAbstract();
    }
    int no = operations.Count();
    for (int i = 0; i < no; ++i)
    {
        OperationElement* operationElement = operations.Get(i);
        clone->Operations().Add(static_cast<OperationElement*>(operationElement->Clone()));
    }
    int nf = fields.Count();
    for (int i = 0; i < nf; ++i)
    {
        FieldElement* fieldElement = fields.Get(i);
        clone->Fields().Add(static_cast<FieldElement*>(fieldElement->Clone()));
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

void ClassElement::SetFields(IndexList<FieldElement>&& fields_)
{
    fields = std::move(fields_);
    for (auto& field : fields)
    {
        field->SetContainerElement(this);
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
    int nf = fields.Count();
    for (int i = 0; i < nf; ++i)
    {
        FieldElement* field = fields.Get(i);
        RelationshipElement* relationship = field->Relationship();
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
        int nf = fromClass->fields.Count();
        for (int i = 0; i < nf; ++i)
        {
            cloneMap[fromClass->fields.Get(i)] = fields.Get(i);
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
            endPoints.push_back(operation->GetEndPoint(Snap(Snap::Operation(i))));
        }
        int nf = fields.Count();
        for (int i = 0; i < no; ++i)
        {
            FieldElement* field = fields.Get(i);
            endPoints.push_back(field->GetEndPoint(Snap(Snap::Field(i))));
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
    int nf = fields.Count();
    for (int i = 0; i < nf; ++i)
    {
        FieldElement* field = fields.Get(i);
        field->SetRelationshipPoint();
    }
}

} // namespace springpp
