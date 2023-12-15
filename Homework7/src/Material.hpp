//
// Created by LEI XU on 5/16/19.
//

#ifndef RAYTRACING_MATERIAL_H
#define RAYTRACING_MATERIAL_H

#include "Vector.hpp"
#include "global.hpp"

enum MaterialType { DIFFUSE };

class Material {
  private:
    // Compute reflection direction
    static auto reflect(const Vector3f& I, const Vector3f& N) -> Vector3f {
        return I - 2 * dotProduct(I, N) * N;
    }

    // Compute refraction direction using Snell's law
    //
    // We need to handle with care the two possible situations:
    //
    //    - When the ray is inside the object
    //
    //    - When the ray is outside.
    //
    // If the ray is outside, you need to make cosi positive cosi = -N.I
    //
    // If the ray is inside, you need to invert the refractive indices and negate the normal N
    static auto refract(const Vector3f& I, const Vector3f& N, const float& ior) -> Vector3f {
        float    cosi = std::clamp(-1.0F, 1.0F, dotProduct(I, N));
        float    etai = 1;
        float    etat = ior;
        Vector3f n    = N;
        if (cosi < 0) {
            cosi = -cosi;
        } else {
            std::swap(etai, etat);
            n = -N;
        }
        float eta = etai / etat;
        float k   = 1 - eta * eta * (1 - cosi * cosi);
        return k < 0 ? 0 : eta * I + (eta * cosi - sqrtf(k)) * n;
    }

    // Compute Fresnel equation
    //
    // \param I is the incident view direction
    //
    // \param N is the normal at the intersection point
    //
    // \param ior is the material refractive index
    //
    // \param[out] kr is the amount of light reflected
    static void fresnel(const Vector3f& I, const Vector3f& N, const float& ior, float& kr) {
        float cosi = std::clamp(-1.F, 1.F, dotProduct(I, N));
        float etai = 1;
        float etat = ior;
        if (cosi > 0) { std::swap(etai, etat); }
        // Compute sini using Snell's law
        float sint = etai / etat * sqrtf(std::max(0.F, 1 - cosi * cosi));
        // Total internal reflection
        if (sint >= 1) {
            kr = 1;
        } else {
            float cost = sqrtf(std::max(0.F, 1 - sint * sint));
            cosi       = fabsf(cosi);
            float Rs   = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
            float Rp   = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
            kr         = (Rs * Rs + Rp * Rp) / 2;
        }
        // As a consequence of the conservation of energy, transmittance is given by:
        // kt = 1 - kr;
    }

    auto toWorld(const Vector3f& a, const Vector3f& N) -> Vector3f {
        Vector3f B;
        Vector3f C;
        if (std::fabs(N.x) > std::fabs(N.y)) {
            float invLen = 1.0F / std::sqrt(N.x * N.x + N.z * N.z);
            C            = Vector3f(N.z * invLen, 0.0F, -N.x * invLen);
        } else {
            float invLen = 1.0F / std::sqrt(N.y * N.y + N.z * N.z);
            C            = Vector3f(0.0F, N.z * invLen, -N.y * invLen);
        }
        B = crossProduct(C, N);
        return a.x * B + a.y * C + a.z * N;
    }

  public:
    MaterialType m_type;
    // Vector3f m_color;
    Vector3f m_emission;
    float    ior;
    Vector3f Kd, Ks;
    float    specularExponent;
    // Texture tex;

    inline Material(MaterialType t = DIFFUSE, Vector3f e = Vector3f(0, 0, 0));
    inline auto getType() const -> MaterialType;
    // inline Vector3f getColor();
    static inline auto getColorAt(double u, double v) -> Vector3f;
    inline auto        getEmission() const -> Vector3f;
    inline auto        hasEmission() -> bool;

    // sample a ray by Material properties
    inline auto sample(const Vector3f& wi, const Vector3f& N) -> Vector3f;
    // given a ray, calculate the PdF of this ray
    inline auto pdf(const Vector3f& wi, const Vector3f& wo, const Vector3f& N) const -> float;
    // given a ray, calculate the contribution of this ray
    inline auto eval(const Vector3f& wi, const Vector3f& wo, const Vector3f& N) -> Vector3f;
};

Material::Material(MaterialType t, Vector3f e) : m_type(t), m_emission(e) {

    // m_color = c;
}

auto Material::getType() const -> MaterialType { return m_type; }
/// Vector3f Material::getColor(){return m_color;}
auto Material::getEmission() const -> Vector3f { return m_emission; }
auto Material::hasEmission() -> bool { return m_emission.norm() > EPSILON; }

auto Material::getColorAt(double u, double v) -> Vector3f { return {}; }

auto Material::sample(const Vector3f& wi, const Vector3f& N) -> Vector3f {
    switch (m_type) {
        case DIFFUSE: {
            // uniform sample on the hemisphere
            float    x_1 = get_random_float();
            float    x_2 = get_random_float();
            float    z   = std::fabs(1.0F - 2.0F * x_1);
            float    r   = std::sqrt(1.0F - z * z);
            float    phi = 2 * M_PI * x_2;
            Vector3f localRay(r * std::cos(phi), r * std::sin(phi), z);
            return toWorld(localRay, N);

            break;
        }
    }
}

// 计算在给定传入光线方向“wi”和表面法线“N”的情况下对给定出射光线方向“wo”进行采样的概率密度函数
auto Material::pdf(const Vector3f& wi, const Vector3f& wo, const Vector3f& N) const -> float {
    switch (m_type) {
        case DIFFUSE: {
            // uniform sample probability 1 / (2 * PI)
            if (dotProduct(wo, N) > 0.0F) { return 0.5F / M_PI; }
            return 0.0F;
            break;
        }
    }
}

auto Material::eval(const Vector3f& wi, const Vector3f& wo, const Vector3f& N) -> Vector3f {
    switch (m_type) {
        case DIFFUSE: {
            // calculate the contribution of diffuse model
            float cosalpha = dotProduct(N, wo);
            if (cosalpha > 0.0F) {
                Vector3f diffuse = Kd / M_PI;
                return diffuse;
            }
            return {0.0F};
            break;
        }
    }
}

#endif // RAYTRACING_MATERIAL_H
