// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

export module springpp.diagram_util;

import springpp.layout;
import springpp.canvas;
import springpp.diagram_element;
import wing;
import std.core;

export namespace springpp {

wing::RectF CalculateBoundingRect(const std::vector<DiagramElement*>& diagramElements);

void SaveImage(const std::string& imageFileName, const Padding& margins, Canvas* canvas, const std::vector<DiagramElement*>& diagramElements, wing::Graphics* graphics,
    wing::ImageFormat imageFormat);

int MainDirection(DiagramElement* source, DiagramElement* target);

} // namespace springpp

