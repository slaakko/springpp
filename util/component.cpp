// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

module util.component;

namespace util {

Component::Component() : container(nullptr), nextSibling(nullptr), prevSibling(nullptr)
{
}

Component::~Component()
{
}

Container::Container(Component* parent_) : parent(parent_), firstChild(nullptr), lastChild(nullptr)
{
}

Container::~Container()
{
    Component* child = firstChild;
    while (child)
    {
        Component* next = child->NextSibling();
        delete child;
        child = next;
    }
}

void Container::AddChild(Component* child)
{
    Container* container = child->GetContainer();
    if (container)
    {
        std::unique_ptr<Component> removedChild = container->RemoveChild(child);
        child = removedChild.release();
    }
    if (lastChild)
    {
        lastChild->LinkAfter(child);
    }
    if (!firstChild)
    {
        firstChild = child;
    }
    child->SetContainer(this);
    lastChild = child;
}

std::unique_ptr<Component> Container::RemoveChild(Component* child)
{
    child->Unlink();
    if (child == firstChild)
    {
        firstChild = child->NextSibling();
    }
    if (child == lastChild)
    {
        lastChild = child->PrevSibling();
    }
    child->SetContainer(nullptr);
    child->SetNextSibling(nullptr);
    child->SetPrevSibling(nullptr);
    return std::unique_ptr<Component>(child);
}

void Container::InsertBefore(Component* child, Component* before)
{
    if (!before)
    {
        AddChild(child);
    }
    else
    {
        Container* container = child->GetContainer();
        if (container)
        {
            std::unique_ptr<Component> removedChild = container->RemoveChild(child);
            child = removedChild.release();
        }
        child->SetContainer(this);
        if (firstChild == before)
        {
            firstChild = child;
        }
        before->LinkBefore(child);
    }
}

void Container::InsertAfter(Component* child, Component* after)
{
    if (!after)
    {
        AddChild(child);
    }
    else
    {
        Container* container = child->GetContainer();
        if (container)
        {
            std::unique_ptr<Component> removedChild = container->RemoveChild(child);
            child = removedChild.release();
        }
        child->SetContainer(this);
        after->LinkAfter(child);
        if (after == lastChild)
        {
            lastChild = child;
        }
    }
}

} // namespace util
