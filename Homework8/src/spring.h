#pragma once

#include "CGL/CGL.h"
#include "mass.h"

using namespace std;

namespace CGL {

    struct Spring {
        Spring(Mass* a, Mass* b, float k)
            : m1(a), m2(b), k(k), rest_length((a->position - b->position).norm()) {}

        float  k;           // 弹簧的弹性系数
        double rest_length; // 弹簧的自然长度

        Mass* m1;
        Mass* m2;
    }; // struct Spring
} // namespace CGL
