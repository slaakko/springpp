// =================================
// Copyright (c) 2024 Seppo Laakko
// Distributed under the MIT license
// =================================

export module springpp.mouse_args;

import wing;
import std.core;

export namespace springpp {

class Diagram;
class DiagramElement;

struct MouseArgs
{
    MouseArgs();
    MouseArgs(const wing::PointF& location_, int elementIndex_, DiagramElement* element_, const wing::MouseEventArgs& mouseEventArgs);
    std::chrono::steady_clock::time_point time;
    wing::PointF location;
    int elementIndex;
    DiagramElement* element;
    bool shift;
    bool control;
    bool left;
    bool right;
};

} // namespace springpp
