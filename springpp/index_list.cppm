// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

export module springpp.index_list;

import wing;
import std.core;

export namespace springpp {

template<typename T>
concept Cloneable = requires(T x)
{
    x.Clone();
};

template<Cloneable T>
class IndexList
{
public:
    std::vector<std::unique_ptr<T>>::const_iterator begin() const
    {
        return elements.begin();
    }
    std::vector<std::unique_ptr<T>>::const_iterator end() const
    {
        return elements.end();
    }
    IndexList<T> Clone()
    {
        IndexList<T> clone;
        for (auto& element : elements)
        {
            clone.Add(static_cast<T*>(element->Clone()));
        }
        return clone;
    }
    T* Get(int index) const
    {
        if (index >= 0 && index < elements.size())
        {
            return elements[index].get();
        }
        else
        {
            throw std::runtime_error("IndexList<T>::Get: invalid index " + std::to_string(index));
        }
    }
    T* TryGet(int index) const
    {
        if (index >= 0 && index < elements.size())
        {
            return elements[index].get();
        }
        else
        {
            return nullptr;
        }
    }
    void Set(int index, T* element)
    {
        if (index >= 0 && index < elements.size())
        {
            elements[index].reset(element);
        }
        else
        {
            throw std::runtime_error("IndexList<T>::Set: invalid index " + std::to_string(index));
        }
    }
    void Add(T* element)
    {
        elements.push_back(std::unique_ptr<T>(element));
    }
    T* Remove(int index)
    {
        if (index >= 0 && index < elements.size())
        {
            std::unique_ptr<T> element = std::move(elements[index]);
            elements.erase(elements.begin() + index);
            return element.release();
        }
        else
        {
            throw std::runtime_error("IndexList<T>::Remove: invalid index " + std::to_string(index));
        }
    }
    void Insert(int index, T* element)
    {
        if (index >= 0 && index < elements.size())
        {
            elements.insert(elements.begin() + index, std::unique_ptr<T>(element));
        }
        else if (index >= elements.size())
        {
            Add(element);
        }
        else
        {
            throw std::runtime_error("IndexList<T>::Insert: invalid index " + std::to_string(index));
        }
    }
    T* Release(int index)
    {
        if (index >= 0 && index < elements.size())
        {
            return elements[index].release();
        }
        else
        {
            throw std::runtime_error("IndexList<T>::Release: invalid index " + std::to_string(index));
        }
    }
    void Clear()
    {
        elements.clear();
    }
    bool IsEmpty() const
    {
        return elements.empty();
    }
    int Count() const
    {
        return elements.size();
    }
    void MoveUp(int index)
    {
        if (index >= 0 && index < elements.size())
        {
            T* element = Remove(index);
            --index;
            Insert(index, element);
        }
        else
        {
            throw std::runtime_error("IndexList<T>::MoveUp: invalid index " + std::to_string(index));
        }
    }
    void MoveDown(int index)
    {
        if (index >= 0 && index < elements.size())
        {
            T* element = Remove(index);
            ++index;
            Insert(index, element);
        }
        else
        {
            throw std::runtime_error("IndexList<T>::MoveDown: invalid index " + std::to_string(index));
        }
    }
private:
    std::vector<std::unique_ptr<T>> elements;
};

template<Cloneable T, typename P>
int IndexOf(const IndexList<T>& list, const P& pred)
{
    int n = list.Count();
    for (int i = 0; i < n; ++i)
    {
        T* element = list.Get(i);
        if (pred(element))
        {
            return i;
        }
    }
    return -1;
}

template<typename T>
class Contains
{
public:
    Contains(const wing::PointF& location_) : location(location_)
    {
    }
    bool operator()(T* element) const
    {
        return element->Contains(location);
    }
private:
    wing::PointF location;
};

} // namespace springpp
