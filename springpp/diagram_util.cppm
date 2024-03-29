// =================================
// Copyright (c) 2024 Seppo Laakko
// Distributed under the MIT license
// =================================

export module springpp.diagram_util;

import springpp.layout;
import springpp.canvas;
import springpp.diagram;
import springpp.diagram_element;
import springpp.relationship_element;
import springpp.primitive;
import wing;
import std.core;

export namespace springpp {

void DeleteRelationships(Diagram* diagram, const std::vector<RelationshipElement*>& relationships);

wing::RectF CalculateBounds(const std::vector<DiagramElement*>& diagramElements);

void SaveImage(const std::string& imageFileName, const Padding& margins, Canvas* canvas, const std::vector<DiagramElement*>& diagramElements, wing::Graphics* graphics,
    wing::ImageFormat imageFormat);

int MainDirection(DiagramElement* source, DiagramElement* target);

class HorizontallyLess
{
public:
    HorizontallyLess(Diagram* diagram_);
    bool operator()(int leftIndex, int rightIndex) const;
private:
    Diagram* diagram;
};

class VerticallyLess
{
public:
    VerticallyLess(Diagram* diagram_);
    bool operator()(int leftIndex, int rightIndex) const;
private:
    Diagram* diagram;
};

std::unique_ptr<RelationshipElement> CombineInheritanceRelationships(const std::vector<RelationshipElement*>& inheritanceRelationships,
    std::vector<RelationshipElement*>& sourceRelationships);

std::vector<std::unique_ptr<RelationshipElement>> SplitCombinedInheritanceRelationship(RelationshipElement* combinedRelationship);

bool LineContains(const wing::PointF& from, const wing::PointF& to, const wing::PointF& loc, float selectedLineWidth);

bool IsHorizontalLine(const wing::PointF& prev, const wing::PointF& next, Line& textLine);

} // namespace springpp

