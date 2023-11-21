// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

module springpp.diagram;

import springpp.canvas;
import springpp.operation;
import springpp.tool;
import springpp.diagram_element;
import springpp.container_element;
import springpp.relationship_element;
import springpp.configuration;
import springpp.diagram_util;
import soul.xml.dom;
import soul.xml.dom.parser;
import soul.xml.xpath;
import util;

namespace springpp {

Diagram::Diagram() : flags(DiagramFlags::none), canvas(nullptr), operation(new EmptyOperation(this)), selection(new EmptySelection(this))
{
}

void Diagram::SetCanvas(Canvas* canvas_)
{
    canvas = canvas_;
}

void Diagram::Save(const std::string& filePath)
{
    std::ofstream file(filePath);
    util::CodeFormatter formatter(file);
    formatter.SetIndentSize(1);
    soul::xml::Document diagramDoc;
    soul::xml::Element* diagramElement = soul::xml::MakeElement("diagram");
    std::vector<ContainerElement*> containerElements;
    std::vector<RelationshipElement*> relationshipElements;
    std::map<ContainerElement*, int> containerElementIndexMap;
    for (const auto& element : elements)
    {
        if (element->IsContainerElement())
        {
            containerElements.push_back(static_cast<ContainerElement*>(element.get()));
        }
        else if (element->IsRelationshipElement())
        {
            relationshipElements.push_back(static_cast<RelationshipElement*>(element.get()));
        }
    }
    int nc = containerElements.size();
    for (int i = 0; i < nc; ++i)
    {
        ContainerElement* containerElement = containerElements[i];
        containerElementIndexMap[containerElement] = i;
        diagramElement->AppendChild(containerElement->ToXml());
    }
    int nr = relationshipElements.size();
    for (int i = 0; i < nr; ++i)
    {
        RelationshipElement* relationshipElement = relationshipElements[i];
        relationshipElement->SetContainerElementIndeces(containerElementIndexMap);
        diagramElement->AppendChild(relationshipElement->ToXml());
    }
    diagramDoc.AppendChild(diagramElement);
    diagramDoc.Write(formatter);
}

void Diagram::Load(const std::string& filePath)
{
    std::vector<RelationshipElement*> relationshipElements;
    std::unique_ptr<soul::xml::Document> diagramDoc = soul::xml::ParseXmlFile(filePath);
    std::unique_ptr<soul::xml::xpath::NodeSet> nodeSet = soul::xml::xpath::EvaluateToNodeSet("/diagram/*", diagramDoc.get());
    int n = nodeSet->Count();
    for (int i = 0; i < n; ++i)
    {
        soul::xml::Node* node = nodeSet->GetNode(i);
        if (node->IsElementNode())
        {
            soul::xml::Element* element = static_cast<soul::xml::Element*>(node);
            DiagramElement* diagramElement = CreateDiagramElement(element->Name());
            diagramElement->Parse(element);
            if (diagramElement->IsRelationshipElement())
            {
                relationshipElements.push_back(static_cast<RelationshipElement*>(diagramElement));
            }
            AddElement(diagramElement);
        }
    }
    for (auto relationshipElement : relationshipElements)
    {
        relationshipElement->Resolve(this);
    }
}

void Diagram::Draw(wing::Graphics& graphics)
{
    if (Changed())
    {
        ResetChanged();
        Configuration::Instance().GetLayout()->SetGraphics(&graphics);
        for (const auto& element : elements)
        {
            element->Measure(graphics);
        }
    }
    if (Invalidated())
    {
        ResetInvalidated();
    }
    std::vector<DiagramElement*> relationshipElements;
    for (const auto& element : elements)
    {
        if (element->IsContainerElement())
        {
            element->Draw(graphics);
        }
        else if (element->IsRelationshipElement())
        {
            relationshipElements.push_back(element.get());
        }
    }
    for (auto relationshipElement : relationshipElements)
    {
        relationshipElement->Draw(graphics);
    }
    operation->Draw(graphics);
    selection->Draw(graphics);
}

void Diagram::SetMenuItems(wing::MenuItem* undoMenuItem, wing::MenuItem* redoMenuItem)
{
    commandList.SetMenuItems(undoMenuItem, redoMenuItem);
}

void Diagram::SetSelection(Selection* selection_)
{
    selection->Clear();
    selection.reset(selection_);
    if (!selection->IsEmpty())
    {
        EnableCopy();
        EnableCut();
    }
    else
    {
        DisableCopy();
        DisableCut();
    }
}

void Diagram::ResetSelection()
{
    selection->Clear();
    DisposeContextMenu();
    selection.reset(new EmptySelection(this));
    DisableCopy();
    DisableCut();
    SetChanged();
    Invalidate();
}

void Diagram::DeleteSelection()
{
    if (!selection->IsEmpty())
    {
        selection->Delete();
        ResetSelection();
    }
}

void Diagram::SetOperation(Operation* operation_)
{
    operation.reset(operation_);
}

void Diagram::ResetOperation()
{
    operation.reset(new EmptyOperation(this));
}

void Diagram::EnableCopy()
{
    Canvas* canvas = GetCanvas();
    if (canvas)
    {
        canvas->EnableCopy();
    }
}

void Diagram::EnableCut()
{
    Canvas* canvas = GetCanvas();
    if (canvas)
    {
        canvas->EnableCut();
    }
}

void Diagram::DisableCopy()
{
    Canvas* canvas = GetCanvas();
    if (canvas)
    {
        canvas->DisableCopy();
    }
}

void Diagram::DisableCut()
{
    Canvas* canvas = GetCanvas();
    if (canvas)
    {
        canvas->DisableCut();
    }
}

void Diagram::AdvanceOperation(MouseArgs& mouseArgs)
{
    if (operation->IsEmptyOperation()) return;
    operation->AdvanceTo(mouseArgs);
    SetChanged();
    Invalidate();
}

void Diagram::CommitOperation(MouseArgs& mouseArgs)
{
    operation->Commit(mouseArgs);
    ResetOperation();
    SetChanged();
    Invalidate();
}

void Diagram::CancelOperation()
{
    operation->Cancel();
    ResetOperation();
    Tools::Instance().SetCurrent(Tools::Instance().GetSelectTool());
    SetChanged();
    Invalidate();
}

void Diagram::DisposeContextMenu()
{
    wing::Window* window = canvas->GetWindow();
    window->RemoveContextMenu();
}

void Diagram::HideContextMenu()
{
    wing::Window* window = canvas->GetWindow();
    window->HideContextMenu();
}

void Diagram::SetChanged()
{
    if (!Changed())
    {
        flags = flags | DiagramFlags::changed;
        SetModified();
    }
}

void Diagram::ResetChanged()
{
    flags = flags & ~DiagramFlags::changed;
}

void Diagram::SetModified()
{
    flags = flags | DiagramFlags::modified;
    if (canvas)
    {
        wing::StatusBarItem* modifiedStatusBarItem = canvas->ModifiedStatusBarItem();
        if (modifiedStatusBarItem)
        {
            modifiedStatusBarItem->SetText("*");
        }
    }
}

void Diagram::ResetModified()
{
    if (IsModified())
    {
        flags = flags & ~DiagramFlags::modified;
        if (canvas)
        {
            wing::StatusBarItem* modifiedStatusBarItem = canvas->ModifiedStatusBarItem();
            modifiedStatusBarItem->SetText(" ");
        }
    }
}

void Diagram::Invalidate()
{
    if (!Invalidated())
    {
        flags = flags | DiagramFlags::invalidated;
        if (canvas)
        {
            canvas->Invalidate();
        }
    }
}

void Diagram::ResetInvalidated()
{
    flags = flags & ~DiagramFlags::invalidated;
}

void Diagram::Undo()
{
    if (commandList.CanUndo())
    {
        commandList.Undo();
        SetChanged();
        Invalidate();
    }
}

void Diagram::Redo()
{
    if (commandList.CanRedo())
    {
        commandList.Redo();
        SetChanged();
        Invalidate();
    }
}

DiagramElement* Diagram::GetElementByIndex(int index) const
{
    return elements.Get(index);
}

void Diagram::SetElementByIndex(DiagramElement* element, int index)
{
    elements.Set(index, element);
}

int Diagram::GetIndexOfElement(DiagramElement* element) const
{
    int n = elements.Count();
    for (int i = 0; i < n; ++i)
    {
        if (elements.Get(i) == element)
        {
            return i;
        }
    }
    return -1;
}

void Diagram::AddElement(DiagramElement* element)
{
    elements.Add(element);
    element->SetDiagram(this);
    SetChanged();
    Invalidate();
}

std::unique_ptr<DiagramElement> Diagram::RemoveElementByIndex(int index)
{
    std::unique_ptr<DiagramElement> element(elements.Remove(index));
    return element;
}

void Diagram::InsertElement(DiagramElement* element, int index)
{
    elements.Insert(index, element);
}

DiagramElement* Diagram::ReleaseElementByIndex(int index)
{
    return elements.Release(index);
}

int Diagram::GetIndexOfElementAt(const wing::PointF& location) const
{
    springpp::Contains<DiagramElement> pred(location);
    return IndexOf(elements, pred);
}

bool Diagram::HasIntersectingContainerElements(const wing::RectF& rect) const
{
    for (const auto& element : elements)
    {
        if (element->IsContainerElement())
        {
            ContainerElement* containerElement = static_cast<ContainerElement*>(element.get());
            if (containerElement->IntersectsWith(rect)) return true;
        }
    }
    return false;
}

int Diagram::GetNumberOfIntersectingContainerElements(const wing::RectF& rect) const
{
    int count = 0;
    for (const auto& element : elements)
    {
        if (element->IsContainerElement())
        {
            ContainerElement* containerElement = static_cast<ContainerElement*>(element.get());
            if (containerElement->IntersectsWith(rect))
            {
                ++count;
            }
        }
    }
    return count;
}

std::vector<int> Diagram::GetIntersectingContainerElementIndeces(const wing::RectF& rect) const
{
    std::vector<int> indeces;
    int n = elements.Count();
    for (int i = 0; i < n; ++i)
    {
        DiagramElement* element = GetElementByIndex(i);
        if (element->IsContainerElement())
        {
            ContainerElement* containerElement = static_cast<ContainerElement*>(element);
            if (containerElement->IntersectsWith(rect))
            {
                indeces.push_back(i);
            }
        }
    }
    return indeces;
}

void Diagram::Offset(float dx, float dy)
{
    for (auto& element : elements)
    {
        element->Offset(dx, dy);
    }
}

wing::RectF Diagram::CalculateBounds() const
{
    std::vector<DiagramElement*> diagramElements;
    for (const auto& element : elements)
    {
        diagramElements.push_back(element.get());
    }
    return springpp::CalculateBounds(diagramElements);
}

void Diagram::SaveImage(const std::string& fileName, const Padding& margins, wing::ImageFormat format)
{
    std::vector<DiagramElement*> diagramElements;
    for (const auto& element : elements)
    {
        diagramElements.push_back(element.get());
    }
    wing::Graphics* graphics = wing::Graphics::FromHWND(canvas->Handle());
    springpp::SaveImage(fileName, margins, canvas, diagramElements, graphics, format);
}

void Diagram::Paste(const std::u32string& clipboardData)
{
    std::unique_ptr<AddElementsCommand> addElementsCommand(new AddElementsCommand(this));
    std::unique_ptr<soul::xml::Document> diagramElementDoc(soul::xml::ParseXmlContent(clipboardData, "clipboard"));
    std::unique_ptr<soul::xml::xpath::NodeSet> nodeSet = soul::xml::xpath::EvaluateToNodeSet("/springpp.diagram.elements/*", diagramElementDoc.get());
    std::unique_ptr<ElementSelection> elementSelection(new ElementSelection(this));
    std::vector<RelationshipElement*> relationshipElements;
    std::map<int, int> indexMap;
    int n = nodeSet->Count();
    for (int i = 0; i < n; ++i)
    {
        soul::xml::Node* node = nodeSet->GetNode(i);
        if (node->IsElementNode())
        {
            soul::xml::Element* xmlElement = static_cast<soul::xml::Element*>(node);
            std::string xmlElementName = xmlElement->Name();
            std::unique_ptr<DiagramElement> diagramElement(CreateDiagramElement(xmlElementName));
            diagramElement->Parse(xmlElement);
            int index = elements.Count();
            if (diagramElement->IsRelationshipElement())
            {
                relationshipElements.push_back(static_cast<RelationshipElement*>(diagramElement.get()));
            }
            AddElement(diagramElement.release());
            indexMap[i] = index;
            elementSelection->Add(index);
            addElementsCommand->AddIndex(index);
        }
    }
    for (RelationshipElement* relationship : relationshipElements)
    {
        relationship->MapIndeces(indexMap);
        relationship->Resolve(this);
    }
    SetSelection(elementSelection.release());
    commandList.AddCommand(addElementsCommand.release());
}

} // namespace springpp
