// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

module pic.primitive;

namespace pic {

float Distance(const PointF& start, const PointF& end)
{
    float dx = std::fabs(start.x - end.x);
    float dy = std::fabs(start.y - end.y);
    float distance = static_cast<float>(std::sqrtf(dx * dx + dy * dy));
    return distance;
}

float Vector::Length() const
{
    return std::sqrtf(x * x + y * y);
}

Vector Unit(const Vector& v)
{
    float length = v.Length();
    return v * (1 / length);
}

float ProjectionFactor(const Vector& v, const Vector& u)
{
    return Dot(v, u * (1 / u.Length()));
}

Vector Proj(const Vector& v, const Vector& u)
{
    return Unit(u) * ProjectionFactor(v, u);
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

Line::Line(const PointF& start_, const Vector& v) : start(start_), end(Vector(Vector(start) + v).ToPoint())
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

RectF Union(const RectF& a, const RectF& b)
{
    float right = std::max(a.Right(), b.Right());
    float bottom = std::max(a.Bottom(), b.Bottom());
    float left = std::min(a.Left(), b.Left());
    float top = std::min(a.Top(), b.Top());
    return RectF(left, top, right - left, bottom - top);
}

} // namespace pic
