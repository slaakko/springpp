// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

module springpp.selection;

import springpp.action;
import springpp.command;
import springpp.diagram;
import springpp.diagram_util;
import springpp.container_element;
import springpp.relationship_element;
import springpp.primitive;
import springpp.layout;
import springpp.configuration;
import springpp.canvas;
import soul.xml.dom;
import util;

namespace springpp {

void DeleteElements(Diagram* diagram, const std::vector<int>& elementIndeces)
{
    std::vector<int> indeces = elementIndeces;
    std::unique_ptr<DeleteElementsCommand> deleteElementsCommand(new DeleteElementsCommand(diagram));
    std::vector<RelationshipElement*> relationships;
    int n = indeces.size();
    for (int i = 0; i < n; ++i)
    {
        int index = indeces[i];
        DiagramElement* element = diagram->GetElementByIndex(index);
        if (element->IsContainerElement())
        {
            ContainerElement* containerElement = static_cast<ContainerElement*>(element);
            std::vector<RelationshipElement*> containerElementRelationships = containerElement->GetAllRelationships();
            for (auto relationship : containerElementRelationships)
            {
                if (std::find(relationships.begin(), relationships.end(), relationship) == relationships.end())
                {
                    relationships.push_back(relationship);
                }
            }
        }
    }
    for (auto relationship : relationships)
    {
        int index = diagram->GetIndexOfElement(relationship);
        if (index != -1)
        {
            if (std::find(indeces.begin(), indeces.end(), index) == indeces.end())
            {
                indeces.push_back(index);
            }
        }
    }
    std::sort(indeces.begin(), indeces.end());
    int m = indeces.size();
    for (int i = m - 1; i >= 0; --i)
    {
        int index = indeces[i];
        std::unique_ptr<DiagramElement> element = diagram->RemoveElementByIndex(index);
        if (element->IsRelationshipElement())
        {
            RelationshipElement* relationship = static_cast<RelationshipElement*>(element.get());
            relationship->RemoveFromElements();
        }
        deleteElementsCommand->AddDeletedElement(element.release(), index);
    }
    diagram->GetCommandList().AddCommand(deleteElementsCommand.release());
}

std::vector<std::unique_ptr<DiagramElement>> CloneElements(Diagram* diagram, const std::vector<int>& indeces)
{
    std::vector<std::unique_ptr<DiagramElement>> clonedElements;
    std::vector<ContainerElement*> containerElements;
    std::vector<RelationshipElement*> relationshipElements;
    wing::RectF bounds;
    int n = indeces.size();
    for (int i = 0; i < n; ++i)
    {
        int index = indeces[i];
        DiagramElement* element = diagram->GetElementByIndex(index);
        if (element->IsContainerElement())
        {
            if (bounds.IsEmptyArea())
            {
                bounds = element->Bounds();
            }
            else
            {
                wing::RectF::Union(bounds, bounds, element->Bounds());
            }
            containerElements.push_back(static_cast<ContainerElement*>(element));
        }
    }
    for (ContainerElement* containerElement : containerElements)
    {
        std::vector<RelationshipElement*> containerRelationships = containerElement->GetAllRelationships();
        for (auto relationship : containerRelationships)
        {
            bool sourceInClosure = false;
            if (relationship->Source().Element() != nullptr)
            {
                ContainerElement* sourceContainerElement = relationship->Source().Element()->GetContainerElement();
                if (std::find(containerElements.begin(), containerElements.end(), sourceContainerElement) != containerElements.end())
                {
                    sourceInClosure = true;
                }
            }
            bool sourcesInClosure = false;
            if (!relationship->SourceEndPoints().empty())
            {
                sourcesInClosure = true;
                for (const EndPoint& endPoint : relationship->SourceEndPoints())
                {
                    ContainerElement* sourceContainerElement = endPoint.Element()->GetContainerElement();
                    if (std::find(containerElements.begin(), containerElements.end(), sourceContainerElement) == containerElements.end())
                    {
                        sourcesInClosure = false;
                        break;
                    }
                }
            }
            bool targetInClosure = false;
            if (relationship->Target().Element() != nullptr)
            {
                ContainerElement* targetContainerElement = relationship->Target().Element()->GetContainerElement();
                if (std::find(containerElements.begin(), containerElements.end(), targetContainerElement) != containerElements.end())
                {
                    targetInClosure = true;
                }
            }
            else if (relationship->RKind() == RelationshipKind::reference && relationship->GetCardinality() == Cardinality::zero)
            {
                targetInClosure = true;
            }
            if ((sourceInClosure || sourcesInClosure) && targetInClosure)
            {
                if (bounds.IsEmptyArea())
                {
                    bounds = relationship->Bounds();
                }
                else
                {
                    wing::RectF::Union(bounds, bounds, relationship->Bounds());
                }
                if (std::find(relationshipElements.begin(), relationshipElements.end(), relationship) == relationshipElements.end())
                {
                    relationshipElements.push_back(relationship);
                }
            }
        }
    }
    std::map<DiagramElement*, DiagramElement*> cloneMap;
    std::map<ContainerElement*, int> containerElementIndexMap;
    int nc = containerElements.size();
    for (int i = 0; i < nc; ++i)
    {
        ContainerElement* containerElement = containerElements[i];
        containerElementIndexMap[containerElement] = i;
        ContainerElement* clone = static_cast<ContainerElement*>(containerElement->Clone());
        cloneMap[containerElement] = clone;
        clonedElements.push_back(std::unique_ptr<DiagramElement>(clone));
    }
    for auto relationshipElement : relationshipElements)
    {
        RelationshipElement* clone = static_cast<RelationshipElement*>(relationshipElement->Clone());
        clone->SetContainerElementIndeces(containerElementIndexMap);
        clone->MapContainerElements(cloneMap);
        clone->AddToElements();
        clonedElements.push_back(std::unique_ptr<DiagramElement>(clone));
    }
    for (auto& clonedElement : clonedElements)
    {
        clonedElement->Offset(-bounds.X, -bounds.Y);
    }
    return clonedElements;
}

Selection::Selection(SelectionKind kind_, Diagram* diagram_) : kind(kind_), diagram(diagram_)
{
}

Selection::~Selection()
{
}

void Selection::AddActions(wing::ContextMenu* contextMenu)
{
    wing::MenuItem* saveImageMenuItem = new wing::MenuItem("Save Image");
    contextMenu->AddMenuItem(saveImageMenuItem);
    contextMenu->AddAction(new SaveSelectionAsImageAction(diagram, saveImageMenuItem));
}

std::u32string Selection::Copy()
{
    std::vector<std::unique_ptr<DiagramElement>> clonedElements = Clone();
    soul::xml::Document elementDoc;
    elementDoc.AppendChild(soul::xml::MakeElement("springpp.diagram.elements"));
    for (const std::unique_ptr<DiagramElement>& clonedElement : clonedElements)
    {
        std::unique_ptr<soul::xml::Element> xmlElement(clonedElement->ToXml());
        elementDoc.DocumentElement()->AppendChild(xmlElement.release());
    }
    std::stringstream strstream;
    util::CodeFormatter formatter(strstream);
    elementDoc.Write(formatter);
    return util::ToUtf32(strstream.str());
}

void Selection::SaveImage(const std::string& fileName, const Padding& margins, wing::ImageFormat imageFormat)
{
    std::vector<std::unique_ptr<DiagramElement>> clonedElements = Clone();
    std::vector<DiagramElement*> diagramElements;
    for (const auto& clonedElement : clonedElements)
    {
        diagramElements.push_back(clonedElement.get());
    }
    Canvas* canvas = GetCanvas();
    wing::Graphics* graphics = wing::Graphics::FromHWND(canvas->Handle());
    springpp::SaveImage(fileName, margins, canvas, diagramElements, graphics, imageFormat);
}

EmptySelection::EmptySelection(Diagram* diagram_) : Selection(SelectionKind::emptySelection, diagram_)
{
}

ElementSelection::ElementSelection(Diagram* diagram_) : Selection(SelectionKind::elementSelection, diagram_)
{
}

bool ElementSelection::IsEmpty() const
{
    return indeces.empty();
}

int ElementSelection::Count() const
{
    return indeces.size();
}

bool ElementSelection::Contains(const wing::PointF& location) const
{
    Diagram* diagram = GetDiagram();
    int n = indeces.size();
    for (int i = 0; i < n; ++i)
    {
        int index = indeces[i];
        DiagramElement* element = diagram->GetElementByIndex(index);
        if (element->Contains(location))
        {
            return true;
        }
    }
    return false;
}

int ElementSelection::GetContainerElementCount() const
{
    int count = 0;
    Diagram* diagram = GetDiagram();
    int n = indeces.size();
    for (int i = 0; i < n; ++i)
    {
        int index = indeces[i];
        DiagramElement* element = diagram->GetElementByIndex(index);
        if (element->IsContainerElement())
        {
            ++count;
        }
    }
    return count;
}

std::vector<int> ElementSelection::GetContainerElementIndeces() const
{
    std::vector<int> containerElementIndeces;
    Diagram* diagram = GetDiagram();
    int n = indeces.size();
    for (int i = 0; i < n; ++i)
    {
        int index = indeces[i];
        DiagramElement* element = diagram->GetElementByIndex(index);
        if (element->IsContainerElement())
        {
            containerElementIndeces.push_back(index);
        }
    }
    return containerElementIndeces;
}

void ElementSelection::Clear()
{
    Diagram* diagram = GetDiagram();
    int n = indeces.size();
    for (int i = 0; i < n; ++i)
    {
        int index = indeces[i];
        DiagramElement* element = diagram->GetElementByIndex(index);
        element->ResetSelected();
    }
    indeces.clear();
}

void ElementSelection::Delete()
{
    DeleteElements(GetDiagram(), indeces);
    indeces.clear();
}

std::vector<std::unique_ptr<DiagramElement>> ElementSelection::Clone() const
{
    return CloneElements(GetDiagram(), indeces);
}

int ElementSelection::GetFirstIndex() const
{
    if (!indeces.empty())
    {
        return indeces.front();
    }
    return -1;
}

void ElementSelection::Add(int index)
{
    int n = indeces.size();
    for (int i = 0; i < n; ++i)
    {
        if (indeces[i] == index) return;
    }
    indeces.push_back(index);
    Diagram* diagram = GetDiagram();
    DiagramElement* element = diagram->GetElementByIndex(index);
    element->Select();
    diagram->EnableCopy();
    diagram->EnableCut();
}

void ElementSelection::Remove(int index)
{
    indeces.erase(std::remove(indeces.begin(), indeces.end(), index), indeces.end());
    Diagram* diagram = GetDiagram();
    DiagramElement* element = diagram->GetElementByIndex(index);
    element->ResetSelected();
    if (!indeces.empty())
    {
        diagram->DisableCopy();
        diagram->DisableCut();
    }
}

void ElementSelection::AddActions(wing::ContextMenu* contextMenu)
{
    Selection::AddActions(contextMenu);
    Diagram* diagram = GetDiagram();
    int containerElementCount = GetContainerElementCount();
    if (containerElementCount > 1)
    {
        wing::MenuItem* alignTopMenuItem = new wing::MenuItem("Align top");
        contextMenu->AddMenuItem(alignTopMenuItem);
        contextMenu->AddAction(new AlignTopAction(diagram, alignTopMenuItem));
        wing::MenuItem* alignBottomMenuItem = new wing::MenuItem("Align bottom");
        contextMenu->AddMenuItem(alignBottomMenuItem);
        contextMenu->AddAction(new AlignBottomAction(diagram, alignBottomMenuItem));
        wing::MenuItem* alignVerticalCenterMenuItem = new wing::MenuItem("Align vertical center");
        contextMenu->AddMenuItem(alignVerticalCenterMenuItem);
        contextMenu->AddAction(new AlignVerticalCenterAction(diagram, alignVerticalCenterMenuItem));
        wing::MenuItem* alignLeftSideMenuItem = new wing::MenuItem("Align left side");
        contextMenu->AddMenuItem(alignLeftSideMenuItem);
        contextMenu->AddAction(new AlignLeftSideAction(diagram, alignLeftSideMenuItem));
        wing::MenuItem* alignRightSideMenuItem = new wing::MenuItem("Align right side");
        contextMenu->AddMenuItem(alignRightSideMenuItem);
        contextMenu->AddAction(new AlignRightSideAction(diagram, alignRightSideMenuItem));
        wing::MenuItem* alignHorizontalCenterMenuItem = new wing::MenuItem("Align horizontal center");
        contextMenu->AddMenuItem(alignHorizontalCenterMenuItem);
        contextMenu->AddAction(new AlignHorizontalCenterAction(diagram, alignHorizontalCenterMenuItem));
        if (containerElementCount > 2)
        {
            wing::MenuItem* spaceEvenlyVerticallyMenuItem = new wing::MenuItem("Space evenly vertically");
            contextMenu->AddMenuItem(spaceEvenlyVerticallyMenuItem);
            contextMenu->AddAction(new SpaceEvenlyVerticallyAction(diagram, spaceEvenlyVerticallyMenuItem));
            wing::MenuItem* spaceEvenlyHorizontallyMenuItem = new wing::MenuItem("Space evenly horizontally");
            contextMenu->AddMenuItem(spaceEvenlyHorizontallyMenuItem);
            contextMenu->AddAction(new SpaceEvenlyHorizontallyAction(diagram, spaceEvenlyHorizontallyMenuItem));
        }
    }
    else
    {
        bool inheritance = false;
        int n = indeces.size();
        if (n > 1)
        {
            for (int i = 0; i < n; ++i)
            {
                inheritance = false;
                int index = indeces[i];
                DiagramElement* element = diagram->GetElementByIndex(index);
                if (element->IsRelationshipElement())
                {
                    RelationshipElement* relationship = static_cast<RelationshipElement*>(element);
                    if (relationship->IsInheritance())
                    {
                        inheritance = true;
                    }
                }
                if (!inheritance)
                {
                    break;
                }
            }
            if (inheritance)
            {
                wing::MenuItem* combineMenuItem = new wing::MenuItem("Combine");
                contextMenu->AddMenuItem(combineMenuItem);
                contextMenu->AddAction(new CombineInheritancesAction(diagram, combineMenuItem));
            }
        }
        if (n > 0)
        {
            wing::MenuItem* deleteMenuItem = new wing::MenuItem("Delete");
            contextMenu->AddMenuItem(deleteMenuItem);
            contextMenu->AddAction(new DeleteSelectionAction(diagram, deleteMenuItem));
        }
    }
}

void ElementSelection::SortHorizontally()
{
    std::sort(indeces.begin(), indeces.end(), HorizontallyLess(GetDiagram()));
}

void ElementSelection::SortVertically()
{
    std::sort(indeces.begin(), indeces.end(), VerticallyLess(GetDiagram()));
}

void ElementSelection::AlignTop()
{
    if (indeces.size() < 2) return;
    SortHorizontally();
    Diagram* diagram = GetDiagram();
    std::unique_ptr<MoveCommand> moveCommand(new MoveCommand(diagram));
    for (int index : indeces)
    {
        DiagramElement* element = diagram->GetElementByIndex(index);
        moveCommand->AddOldLocation(index, element->GetCompoundLocation());
    }
    DiagramElement* firstElement = diagram->GetElementByIndex(indeces.front());
    float top = firstElement->Location().Y;
    int n = indeces.size();
    for (int i = 1; i < n; ++i)
    {
        DiagramElement* element = diagram->GetElementByIndex(indeces[i]);
        element->SetLocation(wing::PointF(element->Location().X, top));
    }
    for (int index : indeces)
    {
        DiagramElement* element = diagram->GetElementByIndex(index);
        moveCommand->AddNewLocation(element->GetCompoundLocation());
    }
    diagram->GetCommandList().AddCommand(moveCommand.release());
}

void ElementSelection::AlignBottom()
{
    if (indeces.size() < 2) return;
    SortHorizontally();
    Diagram* diagram = GetDiagram();
    std::unique_ptr<MoveCommand> moveCommand(new MoveCommand(diagram));
    for (int index : indeces)
    {
        DiagramElement* element = diagram->GetElementByIndex(index);
        moveCommand->AddOldLocation(index, element->GetCompoundLocation());
    }
    DiagramElement* firstElement = diagram->GetElementByIndex(indeces.front());
    float bottom = firstElement->Location().Y + firstElement->Size().Height;
    int n = indeces.size();
    for (int i = 1; i < n; ++i)
    {
        DiagramElement* element = diagram->GetElementByIndex(indeces[i]);
        float elementBottom = element->Location().Y + element->Size().Height;
        float dy = elementBottom - bottom;
        float top = element->Location().Y - dy;
        element->SetLocation(wing::PointF(element->Location().X, top));
    }
    for (int index : indeces)
    {
        DiagramElement* element = diagram->GetElementByIndex(index);
        moveCommand->AddNewLocation(element->GetCompoundLocation());
    }
    diagram->GetCommandList().AddCommand(moveCommand.release());
}

void ElementSelection::AlignVerticalCenter()
{
    if (indeces.size() < 2) return;
    SortHorizontally();
    Diagram* diagram = GetDiagram();
    std::unique_ptr<MoveCommand> moveCommand(new MoveCommand(diagram));
    for (int index : indeces)
    {
        DiagramElement* element = diagram->GetElementByIndex(index);
        moveCommand->AddOldLocation(index, element->GetCompoundLocation());
    }
    DiagramElement* firstElement = diagram->GetElementByIndex(indeces.front());
    float center = firstElement->Location().Y + firstElement->Size().Height / 2;
    int n = indeces.size();
    for (int i = 1; i < n; ++i)
    {
        DiagramElement* element = diagram->GetElementByIndex(indeces[i]);
        float elementCenter = element->Location().Y + element->Size().Height / 2;
        float dy = elementCenter - center;
        float top = element->Location().Y - dy;
        element->SetLocation(wing::PointF(element->Location().X, top));
    }
    for (int index : indeces)
    {
        DiagramElement* element = diagram->GetElementByIndex(index);
        moveCommand->AddNewLocation(element->GetCompoundLocation());
    }
    diagram->GetCommandList().AddCommand(moveCommand.release());
}

void ElementSelection::AlignLeftSide()
{
    if (indeces.size() < 2) return;
    SortVertically();
    Diagram* diagram = GetDiagram();
    std::unique_ptr<MoveCommand> moveCommand(new MoveCommand(diagram));
    for (int index : indeces)
    {
        DiagramElement* element = diagram->GetElementByIndex(index);
        moveCommand->AddOldLocation(index, element->GetCompoundLocation());
    }
    DiagramElement* firstElement = diagram->GetElementByIndex(indeces.front());
    float left = firstElement->Location().X;
    int n = indeces.size();
    for (int i = 1; i < n; ++i)
    {
        DiagramElement* element = diagram->GetElementByIndex(indeces[i]);
        element->SetLocation(wing::PointF(left, element->Location().Y));
    }
    for (int index : indeces)
    {
        DiagramElement* element = diagram->GetElementByIndex(index);
        moveCommand->AddNewLocation(element->GetCompoundLocation());
    }
    diagram->GetCommandList().AddCommand(moveCommand.release());
}

void ElementSelection::AlignRightSide()
{
    if (indeces.size() < 2) return;
    SortVertically();
    Diagram* diagram = GetDiagram();
    std::unique_ptr<MoveCommand> moveCommand(new MoveCommand(diagram));
    for (int index : indeces)
    {
        DiagramElement* element = diagram->GetElementByIndex(index);
        moveCommand->AddOldLocation(index, element->GetCompoundLocation());
    }
    DiagramElement* firstElement = diagram->GetElementByIndex(indeces.front());
    float right = firstElement->Location().X + firstElement->Size().Width;
    int n = indeces.size();
    for (int i = 1; i < n; ++i)
    {
        DiagramElement* element = diagram->GetElementByIndex(indeces[i]);
        float elementRight = element->Location().X + element->Size().Width;
        float dx = elementRight - right;
        float left = element->Location().X - dx;
        element->SetLocation(wing::PointF(left, element->Location().Y));
    }
    for (int index : indeces)
    {
        DiagramElement* element = diagram->GetElementByIndex(index);
        moveCommand->AddNewLocation(element->GetCompoundLocation());
    }
    diagram->GetCommandList().AddCommand(moveCommand.release());
}

void ElementSelection::AlignHorizontalCenter()
{
    if (indeces.size() < 2) return;
    SortVertically();
    Diagram* diagram = GetDiagram();
    std::unique_ptr<MoveCommand> moveCommand(new MoveCommand(diagram));
    for (int index : indeces)
    {
        DiagramElement* element = diagram->GetElementByIndex(index);
        moveCommand->AddOldLocation(index, element->GetCompoundLocation());
    }
    DiagramElement* firstElement = diagram->GetElementByIndex(indeces.front());
    float center = firstElement->Location().X + firstElement->Size().Width / 2;
    int n = indeces.size();
    for (int i = 1; i < n; ++i)
    {
        DiagramElement* element = diagram->GetElementByIndex(indeces[i]);
        float elementCenter = element->Location().X + element->Size().Width / 2;
        float dx = elementCenter - center;
        float left = element->Location().X - dx;
        element->SetLocation(wing::PointF(left, element->Location().Y));
    }
    for (int index : indeces)
    {
        DiagramElement* element = diagram->GetElementByIndex(index);
        moveCommand->AddNewLocation(element->GetCompoundLocation());
    }
    diagram->GetCommandList().AddCommand(moveCommand.release());
}

void ElementSelection::SpaceEvenlyVertically()
{
    if (indeces.size() < 3) return;
    SortVertically();
    Diagram* diagram = GetDiagram();
    std::unique_ptr<MoveCommand> moveCommand(new MoveCommand(diagram));
    for (int index : indeces)
    {
        DiagramElement* element = diagram->GetElementByIndex(index);
        moveCommand->AddOldLocation(index, element->GetCompoundLocation());
    }
    DiagramElement* firstElement = diagram->GetElementByIndex(indeces.front());
    DiagramElement* lastElement = diagram->GetElementByIndex(indeces.back());
    float firstElementBottom = firstElement->Location().Y + firstElement->Size().Height;
    float lastElementTop = lastElement->Location().Y;
    float space = lastElementTop - firstElementBottom;
    int n = indeces.size();
    for (int i = 1; i < n - 1; ++i)
    {
        DiagramElement* element = diagram->GetElementByIndex(indeces[i]);
        space -= element->Size().Height;
    }
    space /= n - 1;
    float prevElementBottom = firstElementBottom;
    for (int i = 1; i < n - 1; ++i)
    {
        DiagramElement* element = diagram->GetElementByIndex(indeces[i]);
        float top = prevElementBottom + space;
        element->SetLocation(wing::PointF(element->Location().X, top));
        prevElementBottom = top + element->Size().Height;
    }
    for (int index : indeces)
    {
        DiagramElement* element = diagram->GetElementByIndex(index);
        moveCommand->AddNewLocation(element->GetCompoundLocation());
    }
    diagram->GetCommandList().AddCommand(moveCommand.release());
}

void ElementSelection::SpaceEvenlyHorizontally()
{
    if (indeces.size() < 3) return;
    SortHorizontally();
    Diagram* diagram = GetDiagram();
    std::unique_ptr<MoveCommand> moveCommand(new MoveCommand(diagram));
    for (int index : indeces)
    {
        DiagramElement* element = diagram->GetElementByIndex(index);
        moveCommand->AddOldLocation(index, element->GetCompoundLocation());
    }
    DiagramElement* firstElement = diagram->GetElementByIndex(indeces.front());
    DiagramElement* lastElement = diagram->GetElementByIndex(indeces.back());
    float firstElementRight = firstElement->Location().X + firstElement->Size().Width;
    float lastElementLeft = lastElement->Location().X;
    float space = lastElementLeft - firstElementRight;
    int n = indeces.size();
    for (int i = 1; i < n - 1; ++i)
    {
        DiagramElement* element = diagram->GetElementByIndex(indeces[i]);
        space -= element->Size().Width;
    }
    space /= n - 1;
    float prevElementRightSide = firstElementRight;
    for (int i = 1; i < n - 1; ++i)
    {
        DiagramElement* element = diagram->GetElementByIndex(indeces[i]);
        float left = prevElementRightSide + space;
        element->SetLocation(wing::PointF(left, element->Location().Y));
        prevElementRightSide = left + element->Size().Width;
    }
    for (int index : indeces)
    {
        DiagramElement* element = diagram->GetElementByIndex(index);
        moveCommand->AddNewLocation(element->GetCompoundLocation());
    }
    diagram->GetCommandList().AddCommand(moveCommand.release());
}

void ElementSelection::CombineInheritanceRelationships()
{
    Diagram* diagram = GetDiagram();
    std::vector<RelationshipElement*> inheritanceRelationships;
    for (int index : indeces)
    {
        DiagramElement* element = diagram->GetElementByIndex(index);
        if (element->IsRelationshipElement())
        {
            RelationshipElement* relationshipElement = static_cast<RelationshipElement*>(element);
            if (relationshipElement->IsInheritance())
            {
                inheritanceRelationships.push_back(relationshipElement);
            }
        }
    }
    std::vector<RelationshipElement*> sourceRelationships;
    std::unique_ptr<RelationshipElement> combinedInheritanceRelationship = springpp::CombineInheritanceRelationships(inheritanceRelationships, sourceRelationships);
    if (combinedInheritanceRelationship)
    {
        std::vector<int> inheritanceRelationshipIndeces;
        for (RelationshipElement* sourceRelationship : sourceRelationships)
        {
            int index = diagram->GetIndexOfElement(sourceRelationship);
            if (index != -1)
            {
                inheritanceRelationshipIndeces.push_back(index);
            }
        }
        std::unique_ptr<DeleteElementsCommand> deleteElementsCommand(new DeleteElementsCommand(diagram));
        std::sort(inheritanceRelationshipIndeces.begin(), inheritanceRelationshipIndeces.end());
        int n = inheritanceRelationshipIndeces.size();
        for (int i = n - 1; i >= 0; --i)
        {
            int index = inheritanceRelationshipIndeces[i];
            std::unique_ptr<DiagramElement> element = diagram->RemoveElementByIndex(index);
            if (element->IsRelationshipElement())
            {
                RelationshipElement* relationship = static_cast<RelationshipElement*>(element.get());
                relationship->RemoveFromElements();
            }
            deleteElementsCommand->AddDeletedElement(element.release(), index);
            indeces.erase(std::remove(indeces.begin(), indeces.end(), index), indeces.end());
        }
        diagram->GetCommandList().AddCommand(deleteElementsCommand.release());
        int addIndex = diagram->NextIndex();
        std::unique_ptr<AddElementsCommand> addElementsCommand(new AddElementsCommand(diagram));
        addElementsCommand->AddIndex(addIndex);
        combinedInheritanceRelationship->AddToElements();
        diagram->AddElement(combinedInheritanceRelationship.release());
        diagram->GetCommandList().AddCommand(addElementsCommand.release());
    }
}

ResizeHandle::ResizeHandle(RectangleSelection* selection_, const wing::PointF& location_, const Connector& connector_) :
    selection(selection_), location(location_), startLocation(location), connector(connector_), width(0.0f)
{
    Layout* layout = Configuration::Instance().GetLayout();
    ResizeHandleLayoutElement* resizeHandleLayoutElement = layout->GetResizeHandleLayoutElement();
    width = resizeHandleLayoutElement->ResizeHandleWidth();
    switch (connector.ToInt())
    {
        case Connector::TopLeft():
        case Connector::BottomRight():
        {
            cursor = wing::LoadStandardCursor(wing::StandardCursorId::sizeNWSE);
            break;
        }
        case Connector::TopCenter():
        case Connector::BottomCenter():
        {
            cursor = wing::LoadStandardCursor(wing::StandardCursorId::sizeNS);
            break;
        }
        case Connector::TopRight():
        case Connector::BottomLeft():
        {
            cursor = wing::LoadStandardCursor(wing::StandardCursorId::sizeNewSW);
            break;
        }
        case Connector::RightCenter():
        case Connector::LeftCenter():
        {
            cursor = wing::LoadStandardCursor(wing::StandardCursorId::sizeWE);
            break;
        }
    }
}

bool ResizeHandle::Contains(const wing::PointF& loc) const
{
    return Distance(location, loc) <= 2 * width;
}

void ResizeHandle::Draw(wing::Graphics& graphics)
{
    Layout* layout = Configuration::Instance().GetLayout();
    ResizeHandleLayoutElement* resizeHandleLayoutElement = layout->GetResizeHandleLayoutElement();
    Vector towv = Vector(-1, 0) * (width / 2.0f);
    Line towl(location, towv);
    Vector tonv = Vector(0, -1) * (width / 2.0f);
    Line tonl(towl.end, tonv);
    wing::PointF nw(tonl.end);
    wing::RectF rect(nw, wing::SizeF(width, width));
    wing::Brush* brush = resizeHandleLayoutElement->InsideBrush();
    wing::Pen* pen = resizeHandleLayoutElement->LinePen();
    graphics.FillRectangle(brush, rect);
    graphics.DrawRectangle(pen, rect);
}

void ResizeHandle::Locate(const wing::RectF& rect)
{
    switch (connector.ToInt())
    {
        case Connector::TopLeft():
        {
            location = wing::PointF(rect.X, rect.Y);
            break;
        }
        case Connector::TopCenter():
        {
            location = wing::PointF(rect.X + rect.Width / 2.0f, rect.Y);
            break;
        }
        case Connector::TopRight():
        {
            location = wing::PointF(rect.X + rect.Width, rect.Y);
            break;
        }
        case Connector::RightCenter():
        {
            location = wing::PointF(rect.X + rect.Width, rect.Y + rect.Height / 2.0f);
            break;
        }
        case Connector::BottomRight():
        {
            location = wing::PointF(rect.X + rect.Width, rect.Y + rect.Height);
            break;
        }
        case Connector::BottomCenter():
        {
            location = wing::PointF(rect.X + rect.Width / 2.0f, rect.Y + rect.Height);
            break;
        }
        case Connector::BottomLeft():
        {
            location = wing::PointF(rect.X, rect.Y + rect.Height);
            break;
        }
        case Connector::LeftCenter():
        {
            location = wing::PointF(rect.X, rect.Y + rect.Height / 2.0f);
            break;
        }
    }
}

void ResizeHandle::Move(float dx, float dy)
{
    wing::RectF selectionRect = selection->StartRect();
    switch (connector.ToInt())
    {
        case Connector::TopLeft():
        {
            selectionRect.Offset(dx, dy);
            selectionRect.Width -= dx;
            selectionRect.Height -= dy;
            break;
        }
        case Connector::TopCenter():
        {
            selectionRect.Offset(0.0f, dy);
            selectionRect.Height -= dy;
            break;
        }
        case Connector::TopRight():
        {
            selectionRect.Offset(0.0f, dy);
            selectionRect.Width += dx;
            selectionRect.Height -= dy;
            break;
        }
        case Connector::RightCenter():
        {
            selectionRect.Width += dx;
            break;
        }
        case Connector::BottomRight():
        {
            selectionRect.Width += dx;
            selectionRect.Height += dy;
            break;
        }
        case Connector::BottomCenter():
        {
            selectionRect.Height += dy;
            break;
        }
        case Connector::BottomLeft():
        {
            selectionRect.Offset(dx, 0.0f);
            selectionRect.Width -= dx;
            selectionRect.Height += dy;
            break;
        }
        case Connector::LeftCenter():
        {
            selectionRect.Offset(dx, 0.0f);
            selectionRect.Width -= dx;
            break;
        }
    }
    selection->SetRect(selectionRect);
}

RectangleSelection::RectangleSelection(Diagram* diagram_, const wing::PointF& startLocation_) :
    Selection(SelectionKind::rectangleSelection, diagram_),
    startLocation(startLocation_),
    rect(startLocation, wing::SizeF()),
    startRect(),
    resizeHandles(),
    arrowCursor(wing::LoadStandardCursor(wing::StandardCursorId::arrow))
{
    ResizeHandle* nw = new ResizeHandle(this, startLocation, Connector(Connector::TopLeft()));
    resizeHandles.push_back(std::unique_ptr<ResizeHandle>(nw));
    ResizeHandle* n = new ResizeHandle(this, startLocation, Connector(Connector::TopCenter()));
    resizeHandles.push_back(std::unique_ptr<ResizeHandle>(n));
    ResizeHandle* ne = new ResizeHandle(this, startLocation, Connector(Connector::TopRight()));
    resizeHandles.push_back(std::unique_ptr<ResizeHandle>(ne));
    ResizeHandle* e = new ResizeHandle(this, startLocation, Connector(Connector::RightCenter()));
    resizeHandles.push_back(std::unique_ptr<ResizeHandle>(e));
    ResizeHandle* se = new ResizeHandle(this, startLocation, Connector(Connector::BottomRight()));
    resizeHandles.push_back(std::unique_ptr<ResizeHandle>(se));
    ResizeHandle* s = new ResizeHandle(this, startLocation, Connector(Connector::BottomCenter()));
    resizeHandles.push_back(std::unique_ptr<ResizeHandle>(s));
    ResizeHandle* sw = new ResizeHandle(this, startLocation, Connector(Connector::BottomLeft()));
    resizeHandles.push_back(std::unique_ptr<ResizeHandle>(sw));
    ResizeHandle* w = new ResizeHandle(this, startLocation, Connector(Connector::LeftCenter()));
    resizeHandles.push_back(std::unique_ptr<ResizeHandle>(w));
}

bool RectangleSelection::IsEmpty() const
{
    if (rect.IsEmptyArea()) return true;
    Diagram* diagram = GetDiagram();
    if (diagram->HasIntersectingContainerElements(rect)) return false;
    return true;
}

int RectangleSelection::Count() const
{
    Diagram* diagram = GetDiagram();
    return diagram->GetNumberOfIntersectingContainerElements(rect);
}

bool RectangleSelection::Contains(const wing::PointF& location) const
{
    return rect.Contains(location);
}

int RectangleSelection::GetContainerElementCount() const
{
    Diagram* diagram = GetDiagram();
    return diagram->GetNumberOfIntersectingContainerElements(rect);
}

std::vector<int> RectangleSelection::GetContainerElementIndeces() const
{
    Diagram* diagram = GetDiagram();
    return diagram->GetIntersectingContainerElementIndeces(rect);
}

void RectangleSelection::Clear()
{
    rect = wing::RectF();
}

void RectangleSelection::Delete()
{
    DeleteElements(GetDiagram(), GetContainerElementIndeces());
}

void RectangleSelection::SetStartLocation(const wing::PointF& startLocation_)
{
    startLocation = startLocation_;
    startRect = rect;
}

void RectangleSelection::Move(MouseArgs& args)
{
    float dx = args.location.X - startLocation.X;
    float dy = args.location.Y - startLocation.Y;
    rect.X = startRect.X + dx;
    rect.Y = startRect.Y + dy;
    LocateResizeHandles();
}

int RectangleSelection::GetFirstIndex() const
{
    std::vector<int> containerElementIndeces = GetContainerElementIndeces();
    if (!containerElementIndeces.empty())
    {
        return containerElementIndeces.front();
    }
    return -1;
}

std::vector<std::unique_ptr<DiagramElement>> RectangleSelection::Clone() const
{
    return CloneElements(GetDiagram(), GetContainerElementIndeces());
}

void RectangleSelection::AddActions(wing::ContextMenu* contextMenu)
{
    Selection::AddActions(contextMenu);
    wing::MenuItem* deleteMenuItem = new wing::MenuItem("Delete");
    contextMenu->AddMenuItem(deleteMenuItem);
    Diagram* diagram = GetDiagram();
    contextMenu->AddAction(new DeleteSelectionAction(diagram, deleteMenuItem));
    int containerElementCount = GetContainerElementCount();
    if (containerElementCount > 1)
    {
        wing::MenuItem* alignTopMenuItem = new wing::MenuItem("Align top");
        contextMenu->AddMenuItem(alignTopMenuItem);
        contextMenu->AddAction(new AlignTopAction(diagram, alignTopMenuItem));
        wing::MenuItem* alignBottomMenuItem = new wing::MenuItem("Align bottom");
        contextMenu->AddMenuItem(alignBottomMenuItem);
        contextMenu->AddAction(new AlignBottomAction(diagram, alignBottomMenuItem));
        wing::MenuItem* alignVerticalCenterMenuItem = new wing::MenuItem("Align vertical center");
        contextMenu->AddMenuItem(alignVerticalCenterMenuItem);
        contextMenu->AddAction(new AlignVerticalCenterAction(diagram, alignVerticalCenterMenuItem));
        wing::MenuItem* alignLeftSideMenuItem = new wing::MenuItem("Align left side");
        contextMenu->AddMenuItem(alignLeftSideMenuItem);
        contextMenu->AddAction(new AlignLeftSideAction(diagram, alignLeftSideMenuItem));
        wing::MenuItem* alignRightSideMenuItem = new wing::MenuItem("Align right side");
        contextMenu->AddMenuItem(alignRightSideMenuItem);
        contextMenu->AddAction(new AlignRightSideAction(diagram, alignRightSideMenuItem));
        wing::MenuItem* alignHorizontalCenterMenuItem = new wing::MenuItem("Align horizontal center");
        contextMenu->AddMenuItem(alignHorizontalCenterMenuItem);
        contextMenu->AddAction(new AlignHorizontalCenterAction(diagram, alignHorizontalCenterMenuItem));
        if (containerElementCount > 2)
        {
            wing::MenuItem* spaceEvenlyVerticallyMenuItem = new wing::MenuItem("Space evenly vertically");
            contextMenu->AddMenuItem(spaceEvenlyVerticallyMenuItem);
            contextMenu->AddAction(new SpaceEvenlyVerticallyAction(diagram, spaceEvenlyVerticallyMenuItem));
            wing::MenuItem* spaceEvenlyHorizontallyMenuItem = new wing::MenuItem("Space evenly horizontally");
            contextMenu->AddMenuItem(spaceEvenlyHorizontallyMenuItem);
            contextMenu->AddAction(new SpaceEvenlyHorizontallyAction(diagram, spaceEvenlyHorizontallyMenuItem));
        }
    }
}

void RectangleSelection::Draw(wing::Graphics& graphics)
{
    Layout* layout = Configuration::Instance().GetLayout();
    wing::Brush* brush = layout->GetSelectionColorElement()->GetBrush();
    graphics.FillRectangle(brush, rect);
    DrawResizeHandles(graphics);
}

void RectangleSelection::SetRect(const wing::RectF& rect_)
{
    rect = rect_;
    LocateResizeHandles();
}

void RectangleSelection::SetSize(const wing::SizeF& size)
{
    rect.Width = size.Width;
    rect.Height = size.Height;
    LocateResizeHandles();
}

wing::Cursor* RectangleSelection::GetCursor(const wing::PointF& location) const
{
    for (const auto& handle : resizeHandles)
    {
        if (handle->Contains(location))
        {
            return handle->GetCursor();
        }
    }
    return const_cast<wing::Cursor*>(&arrowCursor);
}

ResizeHandle* RectangleSelection::GetResizeHandle(const wing::PointF& location) const
{
    for (const auto& handle : resizeHandles)
    {
        if (handle->Contains(location))
        {
            return handle.get();
        }
    }
    return nullptr;
}

void RectangleSelection::LocateResizeHandles()
{
    for (auto& handle : resizeHandles)
    {
        handle->Locate(rect);
    }
}

void RectangleSelection::DrawResizeHandles(wing::Graphics& graphics)
{
    for (auto& handle : resizeHandles)
    {
        handle->Draw(graphics);
    }
}

} // namespace springpp
