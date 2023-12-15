//
// Created by LEI XU on 4/27/19.
//

#ifndef RASTERIZER_SHADER_H
#define RASTERIZER_SHADER_H
#include "Texture.hpp"
#include <eigen3/Eigen/Eigen>
#include <utility>

struct FragmentShaderPayload {
    FragmentShaderPayload() = default;

    FragmentShaderPayload(Eigen::Vector3f col, Eigen::Vector3f nor, Eigen::Vector2f tc,
                          Texture* tex)
        : color(std::move(col)), normal(std::move(nor)), tex_coords(std::move(tc)), texture(tex) {}

    Eigen::Vector3f view_pos;
    Eigen::Vector3f color;
    Eigen::Vector3f normal;
    Eigen::Vector2f tex_coords;
    Texture*        texture{nullptr};
};

struct VertexShaderPayload {
    Eigen::Vector3f position;
};

#endif // RASTERIZER_SHADER_H
