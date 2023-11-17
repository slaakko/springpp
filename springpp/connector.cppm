// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

export module springpp.connector;

import wing;
import std.core;

export namespace springpp {

enum class ConnectorPoint
{
    none, top, bottom, left, right, center, operation, attribute
};

std::string ConnectorPointStr(ConnectorPoint connectorPoint);
ConnectorPoint ParseConnectorPoint(const std::string& str);

class Connector
{
public:
    Connector();
    Connector(ConnectorPoint primaryPoint_, ConnectorPoint secondaryPoint_);
    explicit Connector(int x);
    int ToInt() const { return (int(primaryPoint) << 8) | int(secondaryPoint); }
    static consteval int TopLeft() { return (int(ConnectorPoint::top) << 8) | int(ConnectorPoint::left); }
    static consteval int TopCenter() { return (int(ConnectorPoint::top) << 8) | int(ConnectorPoint::center); }
    static consteval int TopRight() { return (int(ConnectorPoint::top) << 8) | int(ConnectorPoint::right); }
    static consteval int LeftTop() { return (int(ConnectorPoint::left) << 8) | int(ConnectorPoint::top); }
    static consteval int LeftCenter() { return (int(ConnectorPoint::left) << 8) | int(ConnectorPoint::center); }
    static consteval int LeftBottom() { return (int(ConnectorPoint::left) << 8) | int(ConnectorPoint::bottom); }
    static consteval int BottomLeft() { return (int(ConnectorPoint::bottom) << 8) | int(ConnectorPoint::left); }
    static consteval int BottomCenter() { return (int(ConnectorPoint::bottom) << 8) | int(ConnectorPoint::center); }
    static consteval int BottomRight() { return (int(ConnectorPoint::bottom) << 8) | int(ConnectorPoint::right); }
    static consteval int RightTop() { return (int(ConnectorPoint::right) << 8) | int(ConnectorPoint::top); }
    static consteval int RightCenter() { return (int(ConnectorPoint::right) << 8) | int(ConnectorPoint::center); }
    static consteval int RightBottom() { return (int(ConnectorPoint::right) << 8) | int(ConnectorPoint::bottom); }
    static int Operation(int index) { return (int(ConnectorPoint::operation) << 8) | index; }
    static int Attribute(int index) { return (int(ConnectorPoint::attribute) << 8) | index; }
    std::string ToString() const;
    ConnectorPoint PrimaryPoint() const { return primaryPoint; }
    ConnectorPoint SecondaryPoint() const { return secondaryPoint; }
    wing::PointF Calculate(const wing::PointF& point, float w, int index, int count) const;
private:
    ConnectorPoint primaryPoint;
    ConnectorPoint secondaryPoint;
};

Connector ParseConnector(const std::string& connectorStr);

inline bool operator==(const Connector& left, const Connector& right)
{
    return left.PrimaryPoint() == right.PrimaryPoint() && left.SecondaryPoint() == right.SecondaryPoint();
}

inline bool operator!=(const Connector& left, const Connector& right)
{
    return !(left == right);
}

inline bool operator<(const Connector& left, const Connector& right)
{
    if (left.PrimaryPoint() < right.PrimaryPoint()) return true;
    if (left.PrimaryPoint() > right.PrimaryPoint()) return false;
    return left.SecondaryPoint() < right.SecondaryPoint();
}

} // namespace springpp
