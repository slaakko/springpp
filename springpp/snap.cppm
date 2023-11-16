// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

export module springpp.snap;

import wing;
import std.core;

export namespace springpp {

enum class SnapPoint
{
    none, top, bottom, left, right, center, operation, attribute
};

std::string SnapPointStr(SnapPoint snapPoint);
SnapPoint ParseSnapPoint(const std::string& str);

class Snap
{
public:
    Snap();
    Snap(SnapPoint primaryPoint_, SnapPoint secondaryPoint_);
    explicit Snap(int x);
    int ToInt() const { return (int(primaryPoint) << 8) | int(secondaryPoint); }
    static consteval int TopLeft() { return (int(SnapPoint::top) << 8) | int(SnapPoint::left); }
    static consteval int TopCenter() { return (int(SnapPoint::top) << 8) | int(SnapPoint::center); }
    static consteval int TopRight() { return (int(SnapPoint::top) << 8) | int(SnapPoint::right); }
    static consteval int LeftTop() { return (int(SnapPoint::left) << 8) | int(SnapPoint::top); }
    static consteval int LeftCenter() { return (int(SnapPoint::left) << 8) | int(SnapPoint::center); }
    static consteval int LeftBottom() { return (int(SnapPoint::left) << 8) | int(SnapPoint::bottom); }
    static consteval int BottomLeft() { return (int(SnapPoint::bottom) << 8) | int(SnapPoint::left); }
    static consteval int BottomCenter() { return (int(SnapPoint::bottom) << 8) | int(SnapPoint::center); }
    static consteval int BottomRight() { return (int(SnapPoint::bottom) << 8) | int(SnapPoint::right); }
    static consteval int RightTop() { return (int(SnapPoint::right) << 8) | int(SnapPoint::top); }
    static consteval int RightCenter() { return (int(SnapPoint::right) << 8) | int(SnapPoint::center); }
    static consteval int RightBottom() { return (int(SnapPoint::right) << 8) | int(SnapPoint::bottom); }
    static int Operation(int index) { return (int(SnapPoint::operation) << 8) | index; }
    static int Attribute(int index) { return (int(SnapPoint::attribute) << 8) | index; }
    std::string ToString() const;
    SnapPoint PrimaryPoint() const { return primaryPoint; }
    SnapPoint SecondaryPoint() const { return secondaryPoint; }
    wing::PointF Calculate(const wing::PointF& point, float w, int index, int count) const;
private:
    SnapPoint primaryPoint;
    SnapPoint secondaryPoint;
};

Snap ParseSnap(const std::string& snapStr);

inline bool operator==(const Snap& left, const Snap& right)
{
    return left.PrimaryPoint() == right.PrimaryPoint() && left.SecondaryPoint() == right.SecondaryPoint();
}

inline bool operator!=(const Snap& left, const Snap& right)
{
    return !(left == right);
}

inline bool operator<(const Snap& left, const Snap& right)
{
    if (left.PrimaryPoint() < right.PrimaryPoint()) return true;
    if (left.PrimaryPoint() > right.PrimaryPoint()) return false;
    return left.SecondaryPoint() < right.SecondaryPoint();
}

} // namespace springpp
