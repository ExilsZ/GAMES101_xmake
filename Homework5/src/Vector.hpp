#pragma once

#include <cmath>
#include <iostream>

class Vector3f {
  public:
    Vector3f() : x(0), y(0), z(0) {}
    Vector3f(float xx) : x(xx), y(xx), z(xx) {}
    Vector3f(float xx, float yy, float zz) : x(xx), y(yy), z(zz) {}
    auto operator*(const float& r) const -> Vector3f { return {x * r, y * r, z * r}; }
    auto operator/(const float& r) const -> Vector3f { return {x / r, y / r, z / r}; }

    auto operator*(const Vector3f& v) const -> Vector3f { return {x * v.x, y * v.y, z * v.z}; }
    auto operator-(const Vector3f& v) const -> Vector3f { return {x - v.x, y - v.y, z - v.z}; }
    auto operator+(const Vector3f& v) const -> Vector3f { return {x + v.x, y + v.y, z + v.z}; }
    auto operator-() const -> Vector3f { return {-x, -y, -z}; }
    auto operator+=(const Vector3f& v) -> Vector3f& {
        x += v.x, y += v.y, z += v.z;
        return *this;
    }
    friend auto operator*(const float& r, const Vector3f& v) -> Vector3f {
        return {v.x * r, v.y * r, v.z * r};
    }
    friend auto operator<<(std::ostream& os, const Vector3f& v) -> std::ostream& {
        return os << v.x << ", " << v.y << ", " << v.z;
    }
    float x, y, z;
};

class Vector2f {
  public:
    Vector2f() : x(0), y(0) {}
    Vector2f(float xx) : x(xx), y(xx) {}
    Vector2f(float xx, float yy) : x(xx), y(yy) {}
    auto  operator*(const float& r) const -> Vector2f { return {x * r, y * r}; }
    auto  operator+(const Vector2f& v) const -> Vector2f { return {x + v.x, y + v.y}; }
    float x, y;
};

inline auto lerp(const Vector3f& a, const Vector3f& b, const float& t) -> Vector3f {
    return a * (1 - t) + b * t;
}

inline auto normalize(const Vector3f& v) -> Vector3f {
    float mag2 = v.x * v.x + v.y * v.y + v.z * v.z;
    if (mag2 > 0) {
        float invMag = 1 / sqrtf(mag2);
        return {v.x * invMag, v.y * invMag, v.z * invMag};
    }

    return v;
}

inline auto dotProduct(const Vector3f& a, const Vector3f& b) -> float {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline auto crossProduct(const Vector3f& a, const Vector3f& b) -> Vector3f {
    return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}
