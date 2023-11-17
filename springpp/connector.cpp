// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

module springpp.connector;

import util;

namespace springpp {

std::string ConnectorPointStr(ConnectorPoint connectorPoint)
{
    switch (connectorPoint)
    {
        case ConnectorPoint::none: 
        {
            return "none";
        }
        case ConnectorPoint::top:
        {
            return "top";
        }
        case ConnectorPoint::bottom:
        {
            return "bottom";
        }
        case ConnectorPoint::left:
        {
            return "left";
        }
        case ConnectorPoint::right:
        {
            return "right";
        }
        case ConnectorPoint::center:
        {
            return "center";
        }
        case ConnectorPoint::operation:
        {
            return "operation";
        }
        case ConnectorPoint::attribute:
        {
            return "attribute";
        }
    }
    return std::string();
}

const std::map<std::string, ConnectorPoint> connectorPointMap =
{
    {"none", ConnectorPoint::none}, 
    {"top", ConnectorPoint::top}, 
    {"bottom", ConnectorPoint::bottom}, 
    {"left", ConnectorPoint::left}, 
    {"right", ConnectorPoint::right}, 
    {"center", ConnectorPoint::center},
    {"operation", ConnectorPoint::operation},
    {"attribute", ConnectorPoint::attribute}
};

ConnectorPoint ParseConnectorPoint(const std::string& str)
{
    auto it = connectorPointMap.find(str);
    if (it != connectorPointMap.end())
    {
        return it->second;
    }
    return ConnectorPoint::none;
}

Connector::Connector() : primaryPoint(ConnectorPoint::none), secondaryPoint(ConnectorPoint::none)
{
}

Connector::Connector(ConnectorPoint primaryPoint_, ConnectorPoint secondaryPoint_) : primaryPoint(primaryPoint_), secondaryPoint(secondaryPoint_)
{
}

Connector::Connector(int x) : primaryPoint(ConnectorPoint((x >> 8) & 0xFF)), secondaryPoint(ConnectorPoint(x & 0xFF))
{
}

std::string Connector::ToString() const
{
    if (primaryPoint == ConnectorPoint::operation || primaryPoint == ConnectorPoint::attribute)
    {
        return ConnectorPointStr(primaryPoint) + "." + std::to_string(int(secondaryPoint));
    }
    else
    {
        return ConnectorPointStr(primaryPoint) + "." + ConnectorPointStr(secondaryPoint);
    }
}

wing::PointF Connector::Calculate(const wing::PointF& point, float w, int index, int count) const
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

Connector ParseConnector(const std::string& connectorStr)
{
    std::vector<std::string> components = util::Split(connectorStr, '.');
    if (components.size() == 2)
    {
        ConnectorPoint primary = ParseConnectorPoint(components[0]);
        if (primary == ConnectorPoint::operation || primary == ConnectorPoint::attribute)
        {
            Connector connector(primary, static_cast<ConnectorPoint>(std::stoi(components[1])));
            return connector;
        }
        else
        {
            ConnectorPoint secondary = ParseConnectorPoint(components[1]);
            Connector connector(primary, secondary);
            return connector;
        }
    }
    return Connector();
}

} // namespace springpp
