// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

export module pic.primitive;

import std.core;

export namespace pic {

struct PointF
{
    PointF() : x(0), y(0) {}
    PointF(float x_, float y_) : x(x_), y(y_) {}
    void Offset(float dx, float dy) { x += dx; y += dy; }
    float x;
    float y;
};

inline bool operator==(const PointF& left, const PointF& right)
{
    return left.x == right.x && left.y == right.y;
}

inline bool operator!=(const PointF& left, const PointF& right)
{
    return !(left == right);
}

float Distance(const PointF& start, const PointF& end);

struct Vector
{
    Vector() : x(0.0f), y(0.0f) {}
    Vector(float x_, float y_) : x(x_), y(y_) {}
    explicit Vector(const PointF& p) : x(p.x), y(p.y) {}
    float Length() const;
    PointF ToPoint() const { return PointF(x, y); }
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

inline Vector operator*(float a, const Vector& v)
{
    return Vector(a * v.x, a * v.y);
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

Vector Unit(const Vector& v);

inline float Dot(const Vector& u, const Vector& v)
{
    return u.x * v.x + u.y * v.y;
}

float ProjectionFactor(const Vector& v, const Vector& u);

Vector Proj(const Vector& v, const Vector& u);

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
    Line(const PointF& start_, const PointF& end_) : start(start_), end(end_) {}
    Line(const PointF& start_, const Vector& v);
    Vector ToVector() const { return Vector(end.x - start.x, end.y - start.y); }
    float Length() const { return Distance(start, end); }
    PointF start;
    PointF end;
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

struct SizeF
{
    SizeF() : w(0), h(0) {}
    SizeF(float w_, float h_) : w(w_), h(h_) {}
    float w;
    float h;
};

inline bool operator==(const SizeF& left, const SizeF& right)
{
    return left.w == right.w && left.h == right.h;
}

inline bool operator!=(const SizeF& left, const SizeF& right)
{
    return !(left == right);
}

struct RectF
{
    RectF() : location(), size() {}
    RectF(const PointF& location_, const SizeF& size_) : location(location_), size(size_) {}
    RectF(float x, float y, float w, float h) : location(PointF(x, y)), size(SizeF(w, h)) {}
    float Left() const { return location.x; }
    float Top() const { return location.y; }
    float Right() const { return location.x + size.w; }
    float Bottom() const { return location.y + size.h; }
    bool IsEmpty() const { return size.w == 0 && size.h == 0; }
    bool Contains(const PointF& p) const { return p.x >= Left() && p.x < Right() && p.y >= Top() && p.y < Bottom(); }
    bool IntersectsWith(const RectF& that) const { return Left() < that.Right() && Top() < that.Bottom() && Right() > that.Left() && Bottom() > that.Top(); }
    void Offset(float dx, float dy) { location.Offset(dx, dy); }
    void Inflate(float dx, float dy) { location.Offset(-dx, -dy); size.w += 2 * dx; size.h += 2 * dy; }
    PointF location;
    SizeF size;
};

RectF Union(const RectF& a, const RectF& b);

} // namespace pic
