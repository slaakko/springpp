// =================================
// Copyright (c) 2024 Seppo Laakko
// Distributed under the MIT license
// =================================

module springpp.diagram_element;

import springpp.canvas;
import springpp.diagram;
import springpp.layout;
import springpp.configuration;
import springpp.class_element;
import springpp.object_element;
import springpp.note_element;
import springpp.relationship_element;
import springpp.action;

namespace springpp {

std::string RelationshipKindStr(RelationshipKind rkind)
{
    switch (rkind)
    {
        case RelationshipKind::none:
        {
            return "none";
        }
        case RelationshipKind::inheritance:
        {
            return "inheritance";
        }
        case RelationshipKind::combinedInheritance:
        {
            return "combinedInheritance";
        }
        case RelationshipKind::composition:
        {
            return "composition";
        }
        case RelationshipKind::aggregation:
        {
            return "aggregation";
        }
        case RelationshipKind::reference:
        {
            return "reference";
        }
        case RelationshipKind::createInstance:
        {
            return "createInstance";
        }
        case RelationshipKind::attachNote:
        {
            return "attachNote";
        }
    }
    return std::string();
}

const std::map<std::string, RelationshipKind> relationshipKindMap =
{
    { "none", RelationshipKind::none },
    { "inheritance", RelationshipKind::inheritance },
    { "combinedInheritance", RelationshipKind::combinedInheritance },
    { "composition", RelationshipKind::composition },
    { "aggregation", RelationshipKind::aggregation },
    { "reference", RelationshipKind::reference },
    { "createInstance", RelationshipKind::createInstance },
    { "attachNote", RelationshipKind::attachNote }
};

RelationshipKind ParseRelationshipKindStr(const std::string& relationshipKindStr)
{
    auto it = relationshipKindMap.find(relationshipKindStr);
    if (it != relationshipKindMap.end())
    {
        return it->second;
    }
    else
    {
        return RelationshipKind::none;
    }
}

DiagramElement::DiagramElement(DiagramElementKind kind_) : kind(kind_), flags(DiagramElementFlags::none), bounds(), diagram(nullptr)
{
}

void DiagramElement::Measure(wing::Graphics& graphics)
{
}

void DiagramElement::Draw(wing::Graphics& graphics)
{
    if (IsSelected())
    {
        DrawSelected(graphics);
    }
}

void DiagramElement::DrawSelected(wing::Graphics& graphics)
{
    Layout* layout = Configuration::Instance().GetLayout();
    wing::Brush* brush = layout->GetSelectionColorElement()->GetBrush();
    graphics.FillRectangle(brush, bounds);
}

void DiagramElement::Offset(float dx, float dy)
{
    bounds.Offset(dx, dy);
}

wing::PointF DiagramElement::Location() const
{
    wing::PointF location;
    bounds.GetLocation(&location);
    return location;
}

wing::SizeF DiagramElement::Size() const
{
    wing::SizeF size;
    bounds.GetSize(&size);
    return size;
}

void DiagramElement::SetLocation(const wing::PointF& location)
{
    bounds.X = location.X;
    bounds.Y = location.Y;
}

void DiagramElement::SetSize(const wing::SizeF& size)
{
    bounds.Width = size.Width;
    bounds.Height = size.Height;
}

void DiagramElement::SetCompoundLocation(const CompoundLocation& compoundLocation)
{
    SetLocation(compoundLocation.Location());
}

CompoundLocation DiagramElement::GetCompoundLocation() const
{
    return CompoundLocation(Location());
}

void DiagramElement::SetName(const std::string& name_)
{
    name = name_;
}

void DiagramElement::SetBounds(const wing::RectF& bounds_)
{
    bounds = bounds_;
}

wing::PointF DiagramElement::Center() const
{
    wing::PointF location = Location();
    wing::SizeF size = Size();
    return wing::PointF(location.X + size.Width / 2, location.Y + size.Height / 2);
}

void DiagramElement::Select()
{
    if (!IsSelected())
    {
        SetSelected();
        springpp::GetDiagram()->Invalidate();
    }
}

void DiagramElement::ResetSelected()
{
    if (IsSelected())
    {
        flags = flags & ~DiagramElementFlags::selected;
        springpp::GetDiagram()->Invalidate();
    }
}

bool DiagramElement::Contains(const wing::PointF& location) const
{
    return bounds.Contains(location);
}

bool DiagramElement::IntersectsWith(const wing::RectF& rect) const
{
    return bounds.IntersectsWith(rect);
}

void DiagramElement::AddActions(Diagram* diagram, int elementIndex, wing::ContextMenu* contextMenu) const
{
    wing::MenuItem* deleteMenuItem = new wing::MenuItem("Delete");
    contextMenu->AddMenuItem(deleteMenuItem);
    contextMenu->AddAction(new DeleteDiagramElementAction(diagram, elementIndex, deleteMenuItem));
}

class AbstractDiagramElementCreator
{
public:
    virtual DiagramElement* CreateDiagramElement() const = 0;
};

template<typename T>
class ConcreteDiagramElementCreator : public AbstractDiagramElementCreator
{
public:
    DiagramElement* CreateDiagramElement() const override
    {
        return new T();
    }
};

class DiagramElementFactory
{
public:
    static DiagramElementFactory& Instance();
    DiagramElement* CreateDiagramElement(const std::string& xmlElementName) const;
private:
    DiagramElementFactory();
    std::map<std::string, AbstractDiagramElementCreator*> creatorMap;
    std::vector<std::unique_ptr< AbstractDiagramElementCreator>> creators;
};

DiagramElementFactory& DiagramElementFactory::Instance()
{
    static DiagramElementFactory instance;
    return instance;
}

DiagramElementFactory::DiagramElementFactory()
{
    AbstractDiagramElementCreator* classElementCreator = new ConcreteDiagramElementCreator<ClassElement>();
    creatorMap["classElement"] = classElementCreator;
    creators.push_back(std::unique_ptr<AbstractDiagramElementCreator>(classElementCreator));

    AbstractDiagramElementCreator* objectElementCreator = new ConcreteDiagramElementCreator<ObjectElement>();
    creatorMap["objectElement"] = objectElementCreator;
    creators.push_back(std::unique_ptr<AbstractDiagramElementCreator>(objectElementCreator));

    AbstractDiagramElementCreator* noteElementCreator = new ConcreteDiagramElementCreator<NoteElement>();
    creatorMap["noteElement"] = noteElementCreator;
    creators.push_back(std::unique_ptr<AbstractDiagramElementCreator>(noteElementCreator));

    AbstractDiagramElementCreator* relationshipElementCreator = new ConcreteDiagramElementCreator<RelationshipElement>();
    creatorMap["relationshipElement"] = relationshipElementCreator;
    creators.push_back(std::unique_ptr<AbstractDiagramElementCreator>(relationshipElementCreator));
}

DiagramElement* DiagramElementFactory::CreateDiagramElement(const std::string& xmlElementName) const
{
    auto it = creatorMap.find(xmlElementName);
    if (it != creatorMap.end())
    {
        AbstractDiagramElementCreator* creator = it->second;
        return creator->CreateDiagramElement();
    }
    else
    {
        throw std::runtime_error("DiagramElementFactory: creator for '" + xmlElementName + "' not found");
    }
}

DiagramElement* CreateDiagramElement(const std::string& xmlElementName)
{
    return DiagramElementFactory::Instance().CreateDiagramElement(xmlElementName);
}

} // namespace springpp
