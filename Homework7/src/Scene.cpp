//
// Created by Göksu Güvendiren on 2019-05-14.
//

#include "Scene.hpp"

void Scene::buildBVH() {
    printf(" - Generating BVH...\n\n");
    this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

auto Scene::intersect(const Ray& ray) const -> Intersection { return this->bvh->Intersect(ray); }

/**
 * 该函数根据发光对象的面积对场景中的光源进行采样
 *
 * @param pos 表示光源上的采样位置
 * @param pdf 表示对场景中的光源进行采样的概率密度
 */
void Scene::sampleLight(Intersection& pos, float& pdf) const {
    float emit_area_sum = 0;
    for (auto* object : objects) {
        if (object->hasEmit()) { emit_area_sum += object->getArea(); }
    }
    float p       = get_random_float() * emit_area_sum;
    emit_area_sum = 0;
    for (auto* object : objects) {
        if (object->hasEmit()) {
            emit_area_sum += object->getArea();
            if (p <= emit_area_sum) {
                object->Sample(pos, pdf);
                break;
            }
        }
    }
}

auto Scene::trace(const Ray& ray, const std::vector<Object*>& objects, float& tNear,
                  uint32_t& index, Object** hitObject) -> bool {
    *hitObject = nullptr;
    for (auto* object : objects) {
        float    tNearK = kInfinity;
        uint32_t indexK;
        Vector2f uvK;
        if (object->intersect(ray, tNearK, indexK) && tNearK < tNear) {
            *hitObject = object;
            tNear      = tNearK;
            index      = indexK;
        }
    }

    return (*hitObject != nullptr);
}

// Implementation of Path Tracing
auto Scene::castRay(const Ray& ray, int depth) const -> Vector3f {
    // DONE Implement Path Tracing Algorithm here

    Vector3f L_dir(0.0);   // 直接光照之和
    Vector3f L_indir(0.0); // 间接光照之和

    // 使用 BVH 结构判断相交得到的交点
    Intersection x = Scene::intersect(ray);

    // 没有碰到物体，直接返回
    if (!x.happened) { return L_dir; }

    Vector3f  x_c = x.coords;              // 交点坐标
    Vector3f  x_n = x.normal.normalized(); // 交点法向量
    Material* x_m = x.m;                   // 交点材质

    // 随机对光源进行采样 (pdf_light = 1 / A)
    Intersection x_l;
    float        x_l_pdf = NAN;
    sampleLight(x_l, x_l_pdf);

    // 从 x_c 向光源点发射一条光线
    Vector3f dir_x2l = (x_l.coords - x_c).normalized();
    Ray      ray_x2l(x_c + EPSILON * x_n, dir_x2l);

    // 光线 ray_x2l 与场景中物体的交点
    Intersection hit_h2l = Scene::intersect(ray_x2l);

    // 直接光照
    // 若有交点且交点为发光材质，则光源点与 hit_pos 之间无遮挡
    if (hit_h2l.happened && hit_h2l.m->hasEmission()) {
        Vector3f light_n     = hit_h2l.normal;
        Vector3f light_int   = hit_h2l.m->m_emission;                  // 光强
        Vector3f fr          = x_m->eval(ray.direction, dir_x2l, x_n); // 材质 BRDF
        float    cos_theta   = dotProduct(dir_x2l, x_n);
        float    cos_theta_l = dotProduct(-dir_x2l, light_n);

        L_dir = light_int * fr * cos_theta * cos_theta_l / (hit_h2l.distance * x_l_pdf);
    }

    // 间接光照
    if (get_random_float() <= RussianRoulette) {
        // 根据 x 材质随机选取一个方向发射光线
        Vector3f     wi = x_m->sample(ray.direction, x_n).normalized();
        Ray          ray_x2wi(x_c, wi);
        Intersection hit_x2wi = Scene::intersect(ray_x2wi);

        if (hit_x2wi.happened && !hit_x2wi.m->hasEmission()) {
            float pdf = x_m->pdf(ray.direction, wi, x_n);
            // pdf 接近于 0 时，除以它计算得到的颜色会偏向极限值，也就是白色
            if (pdf > EPSILON) {
                L_indir = castRay(ray_x2wi, depth + 1) * x_m->eval(ray.direction, wi, x_n) *
                          dotProduct(wi, x_n) / (pdf * RussianRoulette);
            }
        }
    }
    // 自身发光 + 直接光照 + 间接光照
    return x_m->getEmission() + L_dir + L_indir;
}
