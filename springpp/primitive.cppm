// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

export module springpp.primitive;

import wing;
import std.core;

export namespace springpp {

inline bool operator==(const wing::PointF& left, const wing::PointF& right)
{
    return left.X == right.X && left.Y == right.Y;
}

inline bool operator!=(const wing::PointF& left, const wing::PointF& right)
{
    return !(left == right);
}

float Distance(const wing::PointF& start, const wing::PointF& end);

inline void Offset(wing::PointF& p, float dx, float dy)
{
    p.X += dx;
    p.Y += dy;
}

struct Vector
{
    Vector() : x(0.0f), y(0.0f) {}
    Vector(float x_, float y_) : x(x_), y(y_) {}
    explicit Vector(const wing::PointF& p) : x(p.X), y(p.Y) {}
    float Length() const;
    wing::PointF ToPoint() const { return wing::PointF(x, y); }
    float x;
    float y;
};

inline bool operator==(const Vector& v, const Vector& u)
{
    return v.x == u.x && v.y == u.y;
}

inline bool operator!=(const Vector& v, const Vector& u)
{
    return !(v == u);
}

inline Vector operator*(const Vector& v, float a)
{
    return Vector(v.x * a, v.y * a);
}

inline Vector operator+(const Vector& v, const Vector& u)
{
    return Vector(v.x + u.x, v.y + u.y);
}

inline Vector operator-(const Vector& v, const Vector& u)
{
    return Vector(v.x - u.x, v.y - u.y);
}

Vector UnitVector(const Vector& v);

inline float Dot(const Vector& u, const Vector& v)
{
    return u.x * v.x + u.y * v.y;
}

float ProjectionFactor(const Vector& v, const Vector& u);

Vector Projection(const Vector& v, const Vector& u);

// int MainDirection(const Vector& v):
//      0, if angle between vector v and positive x axis is between 315 and 45 degrees clockwise
//      90, if angle between vector v and positive x axis is between 45 and 135 degrees clockwise
//      180, if angle between vector v and positive x axis is between 135 and 225 degrees clockwise
//      270, if angle between vector v and positive x axis is between 225 and 315 degrees clockwise
//      In this coordinate system x axis points to the right and y axis points downwards.

int MainDirection(const Vector& v);

Vector Rotate(const Vector& v, double angleRad);

struct Line
{
    Line() : start(), end() {}
    Line(const wing::PointF& start_, const wing::PointF& end_) : start(start_), end(end_) {}
    Line(const wing::PointF& start_, const Vector& v);
    Vector ToVector() const { return Vector(end.X - start.X, end.Y - start.Y); }
    float Length() const { return Distance(start, end); }
    wing::PointF start;
    wing::PointF end;
};

inline bool operator==(const Line& left, const Line& right)
{
    return left.start == right.start && left.end == right.end;
}

inline bool operator!=(const Line& left, const Line& right)
{
    return !(left == right);
}

Line Rotate(const Line& line, float angleDeg);

class CompoundLocation
{
public:
    CompoundLocation(const wing::PointF& location_);
    CompoundLocation(const wing::PointF& location_, const std::vector<wing::PointF>& points_);
    const wing::PointF& Location() const { return location; }
    const std::vector<wing::PointF>& Points() const { return points; }
    bool HasTargetPoint() const { return hasTargetPoint; }
    void SetTargetPoint(const wing::PointF& targetPoint_);
    const wing::PointF& TargetPoint() const { return targetPoint; }
    void Offset(float dx, float dy);
private:
    wing::PointF location;
    std::vector<wing::PointF> points;
    bool hasTargetPoint;
    wing::PointF targetPoint;
};

bool Contains(const wing::PointF& from, const wing::PointF& to, const wing::PointF& loc, float selectedLineWidth);

} // namespace springpp
