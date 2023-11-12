// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

module springpp.primitive;

namespace springpp {

float Distance(const wing::PointF& start, const wing::PointF& end)
{
    float dx = std::fabs(start.X - end.X);
    float dy = std::fabs(start.Y - end.Y);
    float distance = static_cast<float>(std::sqrtf(dx * dx + dy * dy));
    return distance;
}

float Vector::Length() const
{
    return std::sqrtf(x * x + y * y);
}

Vector UnitVector(const Vector& v)
{
    float length = v.Length();
    return v * (1 / length);
}

float ProjectionFactor(const Vector& v, const Vector& u)
{
    return Dot(v, u * (1 / u.Length()));
}

Vector Projection(const Vector& v, const Vector& u)
{
    return UnitVector(u) * ProjectionFactor(v, u);
}

int MainDirection(const Vector& v)
{
    if (v.x >= 0)
    {
        if (v.y >= 0)
        {
            Vector u(1, -1);
            if (Dot(v, u) >= 0)
            {
                return 0;
            }
            else
            {
                return 90;
            }
        }
        else
        {
            Vector u(-1, -1);
            if (Dot(v, u) >= 0)
            {
                return 270;
            }
            else
            {
                return 0;
            }
        }
    }
    else
    {
        if (v.y >= 0)
        {
            Vector u(1, 1);
            if (Dot(v, u) >= 0)
            {
                return 90;
            }
            else
            {
                return 180;
            }
        }
        else
        {
            Vector u(-1, 1);
            if (Dot(v, u) >= 0)
            {
                return 180;
            }
            else
            {
                return 270;
            }
        }
    }
}

Vector Rotate(const Vector& v, double angleRad)
{
    double cosTheta = std::cos(angleRad);
    double sinTheta = std::sin(angleRad);
    Vector r(static_cast<float>(v.x * cosTheta - v.y * sinTheta), static_cast<float>(v.x * sinTheta + v.y * cosTheta));
    return r;
}

Line::Line(const wing::PointF& start_, const Vector& v) : start(start_), end(Vector(Vector(start) + v).ToPoint())
{
}

Line Rotate(const Line& line, float angleDeg)
{
    double angleRad = std::numbers::pi_v<double> / 180.0 * angleDeg;
    Vector v = line.ToVector();
    Vector r = Rotate(v, angleRad);
    Line result(line.start, r);
    return result;
}

CompoundLocation::CompoundLocation(const wing::PointF& location_) : location(location_), points(), hasTargetPoint(false)
{
}

CompoundLocation::CompoundLocation(const wing::PointF& location_, const std::vector<wing::PointF>& points_) : location(location_), points(points_), hasTargetPoint(false)
{
}

void CompoundLocation::SetTargetPoint(const wing::PointF& targetPoint_)
{
    hasTargetPoint = true;
    targetPoint = targetPoint_;
}

void CompoundLocation::Offset(float dx, float dy)
{
    springpp::Offset(location, dx, dy);
    for (auto& point : points)
    {
        springpp::Offset(point, dx, dy);
    }
    if (hasTargetPoint)
    {
        springpp::Offset(targetPoint, dx, dy);
    }
}

bool Contains(const wing::PointF& from, const wing::PointF& to, const wing::PointF& loc, float selectedLineWidth)
{
    Line line(from, to);
    Line lineToLoc(from, loc);
    Vector a(line.ToVector());
    Vector b(lineToLoc.ToVector());
    float f = ProjectionFactor(b, a);
    if (f >= 0 && f <= a.Length())
    {
        Vector c = Projection(b, a);
        Vector d = b - c;
        if (d.Length() <= selectedLineWidth / 2)
        {
            return true;
        }
    }
    return false;
}

} // namespace springpp
