//
// Created by LEI XU on 5/16/19.
//

#ifndef RAYTRACING_RAY_H
#define RAYTRACING_RAY_H
#include "Vector.hpp"
struct Ray {
    // Destination = origin + t*direction

    Vector3f origin;
    Vector3f direction;     // 光的方向
    Vector3f direction_inv; // 光的方向坐标的倒数
    double   t;             // 传播时间
    double   t_min{0.0};
    double   t_max{std::numeric_limits<double>::max()};

    Ray(const Vector3f& ori, const Vector3f& dir, const double _t = 0.0)
        : origin(ori), direction(dir), t(_t) {
        direction_inv = Vector3f(1.F / direction.x, 1.F / direction.y, 1.F / direction.z);
    }

    auto operator()(double t) const -> Vector3f { return origin + direction * t; }

    friend auto operator<<(std::ostream& os, const Ray& r) -> std::ostream& {
        os << "[origin:=" << r.origin << ", direction=" << r.direction << ", time=" << r.t << "]\n";
        return os;
    }
};
#endif // RAYTRACING_RAY_H
