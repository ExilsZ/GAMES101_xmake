//
// Created by LEI XU on 5/16/19.
//

#ifndef RAYTRACING_INTERSECTION_H
#define RAYTRACING_INTERSECTION_H
#include "Material.hpp"
#include "Vector.hpp"
class Object;
class Sphere;

struct Intersection {
    Intersection() = default;

    bool      happened{false};                              // 是否相交
    Vector3f  coords{Vector3f()};                           // 交点坐标
    Vector3f  normal{Vector3f()};                           // 交点法向量
    double    distance{std::numeric_limits<double>::max()}; // 距离
    Object*   obj{nullptr};                                 // 对象指针
    Material* m{nullptr};                                   // 材质指针
};
#endif // RAYTRACING_INTERSECTION_H
