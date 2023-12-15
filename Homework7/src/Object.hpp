//
// Created by LEI XU on 5/13/19.
//
#pragma once
#ifndef RAYTRACING_OBJECT_H
#    define RAYTRACING_OBJECT_H

#    include "Bounds3.hpp"
#    include "Intersection.hpp"
#    include "Ray.hpp"
#    include "Vector.hpp"

class Object {
  public:
    Object()                                                                       = default;
    virtual ~Object()                                                              = default;
    virtual auto intersect(const Ray& ray) -> bool                                 = 0;
    virtual auto intersect(const Ray& ray, float&, uint32_t&) const -> bool        = 0;
    virtual auto getIntersection(Ray _ray) -> Intersection                         = 0;
    virtual void getSurfaceProperties(const Vector3f&, const Vector3f&, const uint32_t&,
                                      const Vector2f&, Vector3f&, Vector2f&) const = 0;
    virtual auto evalDiffuseColor(const Vector2f&) const -> Vector3f               = 0;
    virtual auto getBounds() -> Bounds3                                            = 0;
    virtual auto getArea() -> float                                                = 0;
    virtual void Sample(Intersection& pos, float& pdf)                             = 0;
    virtual auto hasEmit() -> bool                                                 = 0;
};

#endif // RAYTRACING_OBJECT_H
