// =================================
// Copyright (c) 2024 Seppo Laakko
// Distributed under the MIT license
// =================================

export module springpp.end_point;

import springpp.connector;
import soul.xml.dom;
import wing;
import std.core;

export namespace springpp {

class ContainerElement;
class Diagram;
class DiagramElement;
class TextElement;

class EndPoint
{
public:
    EndPoint();
    EndPoint(DiagramElement* element_, const Connector& connector_, const wing::PointF& point_);
    EndPoint(const EndPoint& that);
    EndPoint& operator=(const EndPoint& that);
    EndPoint(EndPoint&& that);
    EndPoint& operator=(EndPoint&& that);
    ~EndPoint();
    DiagramElement* Element() const { return element; }
    void SetElement(DiagramElement* element_) { element = element_; }
    const Connector& GetConnector() const { return connector; }
    void SetConnector(const Connector& connector_);
    const wing::PointF& Point() const { return point; }
    wing::PointF& Point() { return point; }
    void SetPoint(const wing::PointF& point_);
    TextElement* PrimaryTextElement() const { return primaryTextElement; }
    TextElement* SecondaryTextElement() const { return secondaryTextElement; }
    int Index() const { return index; }
    void SetIndex(const std::map<ContainerElement*, int>& containerElementIndexMap);
    void MapIndex(const std::map<int, int>& indexMap);
    void Resolve(Diagram* diagram);
    soul::xml::Element* ToXml(const std::string& elementName) const;
    void Parse(soul::xml::Element* xmlElement);
    void MeasureTexts(wing::Graphics& graphics);
private:
    DiagramElement* element;
    Connector connector;
    wing::PointF point;
    TextElement* primaryTextElement;
    TextElement* secondaryTextElement;
    int index;
};

class EndPointNearer
{
public:
    EndPointNearer(const wing::PointF& location_);
    bool operator()(const EndPoint* left, const EndPoint* right) const;
private:
    wing::PointF location;
};

} // namespace springpp
