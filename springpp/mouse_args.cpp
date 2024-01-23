// =================================
// Copyright (c) 2024 Seppo Laakko
// Distributed under the MIT license
// =================================

module springpp.mouse_args;

namespace springpp {

MouseArgs::MouseArgs() : time(), location(), elementIndex(-1), element(nullptr), shift(false), control(false), left(false), right(false)
{
}

MouseArgs::MouseArgs(const wing::PointF& location_, int elementIndex_, DiagramElement* element_, const wing::MouseEventArgs& mouseEventArgs) : 
    time(std::chrono::steady_clock::now()), location(location_), elementIndex(elementIndex_), element(element_), 
    shift((mouseEventArgs.buttons & wing::MouseButtons::shift) != wing::MouseButtons::none),
    control((mouseEventArgs.buttons& wing::MouseButtons::control) != wing::MouseButtons::none),
    left((mouseEventArgs.buttons& wing::MouseButtons::lbutton) != wing::MouseButtons::none),
    right((mouseEventArgs.buttons& wing::MouseButtons::rbutton) != wing::MouseButtons::none)
{
}

} // namespace springpp
