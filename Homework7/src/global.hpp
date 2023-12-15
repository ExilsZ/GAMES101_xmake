#pragma once
#include <cmath>
#include <iostream>
#include <random>

#undef M_PI
#define M_PI 3.141592653589793F

extern const float EPSILON;
const float        kInfinity = std::numeric_limits<float>::max();

inline auto clamp(const float& lo, const float& hi, const float& v) -> float {
    return std::max(lo, std::min(hi, v));
}

inline auto solveQuadratic(const float& a, const float& b, const float& c, float& x0, float& x1)
    -> bool {
    float discr = b * b - 4 * a * c;
    if (discr < 0) { return false; }
    if (discr == 0) {
        x0 = x1 = -0.5 * b / a;
    } else {
        float q = (b > 0) ? -0.5 * (b + sqrt(discr)) : -0.5 * (b - sqrt(discr));
        x0      = q / a;
        x1      = c / q;
    }
    if (x0 > x1) { std::swap(x0, x1); }
    return true;
}

inline auto get_random_float() -> float {
    static std::random_device                    dev;
    static std::mt19937                          rng(dev());
    static std::uniform_real_distribution<float> dist(0.F, 1.F); // distribution in range [1, 6]

    return dist(rng);
}

inline void UpdateProgress(float progress) {
    int barWidth = 36;

    std::cout << "[";
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) {
            std::cout << "=";
        } else if (i == pos) {
            std::cout << ">";
        } else {
            std::cout << " ";
        }
    }
    std::cout << "] " << int(progress * 100.0) << " %\r";
    std::cout.flush();
};
