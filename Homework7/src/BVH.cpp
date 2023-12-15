#include "BVH.hpp"
#include <algorithm>
#include <cassert>

BVHAccel::BVHAccel(std::vector<Object*> p, int maxPrimsInNode, SplitMethod splitMethod)
    : maxPrimsInNode(std::min(255, maxPrimsInNode)), splitMethod(splitMethod),
      primitives(std::move(p)) {
    time_t start;
    time_t stop;
    time(&start);
    if (primitives.empty()) { return; }

    root = recursiveBuild(primitives);

    time(&stop);
    double diff = difftime(stop, start);
    int    hrs  = (int)diff / 3600;
    int    mins = ((int)diff / 60) - (hrs * 60);
    int    secs = (int)diff - (hrs * 3600) - (mins * 60);

    printf("\rBVH Generation complete: \nTime Taken: %i hrs, %i mins, %i secs\n\n", hrs, mins,
           secs);
}

auto BVHAccel::recursiveBuild(std::vector<Object*> objects) -> BVHBuildNode* {
    BVHBuildNode* node = new BVHBuildNode();

    // Compute bounds of all primitives in BVH node
    Bounds3 bounds;
    for (auto& object : objects) { bounds = Union(bounds, object->getBounds()); }
    if (objects.size() == 1) {
        // Create leaf _BVHBuildNode_
        node->bounds = objects[0]->getBounds();
        node->object = objects[0];
        node->left   = nullptr;
        node->right  = nullptr;
        node->area   = objects[0]->getArea();
        return node;
    }
    if (objects.size() == 2) {
        node->left  = recursiveBuild(std::vector{objects[0]});
        node->right = recursiveBuild(std::vector{objects[1]});

        node->bounds = Union(node->left->bounds, node->right->bounds);
        node->area   = node->left->area + node->right->area;
        return node;
    }

    Bounds3 centroidBounds;
    for (auto& object : objects) {
        centroidBounds = Union(centroidBounds, object->getBounds().Centroid());
    }
    int dim = centroidBounds.maxExtent();
    switch (dim) {
        case 0:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().x < f2->getBounds().Centroid().x;
            });
            break;
        case 1:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().y < f2->getBounds().Centroid().y;
            });
            break;
        case 2:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().z < f2->getBounds().Centroid().z;
            });
            break;
    }

    auto beginning = objects.begin();
    auto middling  = objects.begin() + (objects.size() / 2);
    auto ending    = objects.end();

    auto leftshapes  = std::vector<Object*>(beginning, middling);
    auto rightshapes = std::vector<Object*>(middling, ending);

    assert(objects.size() == (leftshapes.size() + rightshapes.size()));

    node->left  = recursiveBuild(leftshapes);
    node->right = recursiveBuild(rightshapes);

    node->bounds = Union(node->left->bounds, node->right->bounds);
    node->area   = node->left->area + node->right->area;

    return node;
}

auto BVHAccel::Intersect(const Ray& ray) const -> Intersection {
    Intersection isect;
    if (root == nullptr) { return isect; }
    isect = BVHAccel::getIntersection(root, ray);
    return isect;
}

auto BVHAccel::getIntersection(BVHBuildNode* node, const Ray& ray) const -> Intersection {
    // DONE Traverse the BVH to find intersection
    Intersection isect;

    if (!node->bounds.IntersectP(
            ray, ray.direction_inv,
            std::array<int, 3>{ray.direction.x > 0, ray.direction.y > 0, ray.direction.z > 0})) {
        return isect;
    }

    if (node->object != nullptr) { return node->object->getIntersection(ray); }

    Intersection hit_left;
    Intersection hit_right;

    hit_left  = getIntersection(node->left, ray);
    hit_right = getIntersection(node->right, ray);

    return hit_left.distance <= hit_right.distance ? hit_left : hit_right;
}

void BVHAccel::getSample(BVHBuildNode* node, float p, Intersection& pos, float& pdf) {
    if (node->left == nullptr || node->right == nullptr) {
        node->object->Sample(pos, pdf);
        pdf *= node->area;
        return;
    }
    if (p < node->left->area) {
        getSample(node->left, p, pos, pdf);
    } else {
        getSample(node->right, p - node->left->area, pos, pdf);
    }
}

void BVHAccel::Sample(Intersection& pos, float& pdf) {
    float p = std::sqrt(get_random_float()) * root->area;
    getSample(root, p, pos, pdf);
    pdf /= root->area;
}
