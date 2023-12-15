//
// Created by LEI XU on 5/16/19.
//

#ifndef RAYTRACING_BVH_H
#define RAYTRACING_BVH_H

#include "Bounds3.hpp"
#include "Intersection.hpp"
#include "Object.hpp"
#include "Ray.hpp"
#include <atomic>
#include <ctime>
#include <memory>
#include <vector>

struct BVHBuildNode;
// BVHAccel Forward Declarations
struct BVHPrimitiveInfo;

// BVHAccel Declarations
inline int leafNodes, totalLeafNodes, totalPrimitives, interiorNodes;
class BVHAccel {

  public:
    // BVHAccel Public Types
    enum class SplitMethod { NAIVE, SAH };

    // BVHAccel Public Methods
    BVHAccel(std::vector<Object*> p, int maxPrimsInNode = 1,
             SplitMethod splitMethod = SplitMethod::NAIVE);
    auto WorldBound() const -> Bounds3;

    ~BVHAccel();
    BVHBuildNode* root;

    auto Intersect(const Ray& ray) const -> Intersection;
    auto getIntersection(BVHBuildNode* node, const Ray& ray) const -> Intersection;
    auto IntersectP(const Ray& ray) const -> bool;

    // BVHAccel Private Methods
    auto recursiveBuild(std::vector<Object*> objects) -> BVHBuildNode*;

    // BVHAccel Private Data
    const int            maxPrimsInNode;
    const SplitMethod    splitMethod;
    std::vector<Object*> primitives;
};

struct BVHBuildNode {
    Bounds3       bounds;
    BVHBuildNode* left;
    BVHBuildNode* right;
    Object*       object;

  public:
    int splitAxis = 0, firstPrimOffset = 0, nPrimitives = 0;
    // BVHBuildNode Public Methods
    BVHBuildNode() {
        bounds = Bounds3();
        left   = nullptr;
        right  = nullptr;
        object = nullptr;
    }
};

#endif // RAYTRACING_BVH_H
