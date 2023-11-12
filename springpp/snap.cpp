// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

module springpp.snap;

import util;

namespace springpp {

std::string SnapPointStr(SnapPoint snapPoint)
{
    switch (snapPoint)
    {
        case SnapPoint::none: 
        {
            return "none";
        }
        case SnapPoint::top:
        {
            return "top";
        }
        case SnapPoint::bottom:
        {
            return "bottom";
        }
        case SnapPoint::left:
        {
            return "left";
        }
        case SnapPoint::right:
        {
            return "right";
        }
        case SnapPoint::center:
        {
            return "center";
        }
        case SnapPoint::operation:
        {
            return "operation";
        }
        case SnapPoint::field:
        {
            return "field";
        }
    }
    return std::string();
}

const std::map<std::string, SnapPoint> snapPointMap =
{
    {"none", SnapPoint::none}, 
    {"top", SnapPoint::top}, 
    {"bottom", SnapPoint::bottom}, 
    {"left", SnapPoint::left}, 
    {"right", SnapPoint::right}, 
    {"center", SnapPoint::center},
    {"operation", SnapPoint::operation},
    {"field", SnapPoint::field}
};

SnapPoint ParseSnapPoint(const std::string& str)
{
    auto it = snapPointMap.find(str);
    if (it != snapPointMap.end())
    {
        return it->second;
    }
    return SnapPoint::none;
}

Snap::Snap() : primaryPoint(SnapPoint::none), secondaryPoint(SnapPoint::none)
{
}

Snap::Snap(SnapPoint primaryPoint_, SnapPoint secondaryPoint_) : primaryPoint(primaryPoint_), secondaryPoint(secondaryPoint_)
{
}

Snap::Snap(int x) : primaryPoint(SnapPoint((x >> 8) & 0xFF)), secondaryPoint(SnapPoint(x & 0xFF))
{
}

std::string Snap::ToString() const
{
    if (primaryPoint == SnapPoint::operation || primaryPoint == SnapPoint::field)
    {
        return SnapPointStr(primaryPoint) + "." + std::to_string(int(secondaryPoint));
    }
    else
    {
        return SnapPointStr(primaryPoint) + "." + SnapPointStr(secondaryPoint);
    }
}

wing::PointF Snap::Calculate(const wing::PointF& point, float w, int index, int count) const
{
    int s = ToInt();
    switch (s)
    {
        case TopLeft():
        case BottomLeft():
        {
            return wing::PointF(point.X + index * w, point.Y);
        }
        case TopRight():
        case BottomRight():
        {
            return wing::PointF(point.X - index * w, point.Y);
        }
        case LeftTop():
        case RightTop():
        {
            return wing::PointF(point.X, point.Y + index * w);
        }
        case LeftBottom():
        case RightBottom():
        {
            return wing::PointF(point.X, point.Y - index * w);
        }
        case LeftCenter():
        case RightCenter():
        {
            if ((count & 1) == 0)
            {
                return wing::PointF(point.X, point.Y - (count / 2) * w + index * w + w / 2);
            }
            else
            {
                return wing::PointF(point.X, point.Y - (count / 2) * w + index * w);
            }
        }
        case TopCenter():
        case BottomCenter():
        {
            if ((count & 1) == 0)
            {
                return wing::PointF(point.X - (count / 2) * w + index * w + w / 2, point.Y);
            }
            else
            {
                return wing::PointF(point.X - (count / 2) * w + index * w, point.Y);
            }
        }
    }
    return wing::PointF();
}

Snap ParseSnap(const std::string& snapStr)
{
    std::vector<std::string> components = util::Split(snapStr, '.');
    if (components.size() == 2)
    {
        SnapPoint primary = ParseSnapPoint(components[0]);
        if (primary == SnapPoint::operation || primary == SnapPoint::field)
        {
            Snap snap(primary, static_cast<SnapPoint>(std::stoi(components[1])));
            return snap;
        }
        else
        {
            SnapPoint secondary = ParseSnapPoint(components[1]);
            Snap snap(primary, secondary);
            return snap;
        }
    }
    return Snap();
}

} // namespace springpp
