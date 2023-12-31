//
// Created by LEI XU on 4/11/19.
//

#ifndef RASTERIZER_TRIANGLE_H
#define RASTERIZER_TRIANGLE_H

#include "Texture.hpp"
#include <eigen3/Eigen/Eigen>

using namespace Eigen;
class Triangle {

  public:
    Vector4f v[3];
    /*the original coordinates of the triangle, v0, v1, v2 in counter clockwise order*/
    /*Per vertex values*/
    Vector3f color[3];      // color at each vertex;
    Vector2f tex_coords[3]; // texture u,v
    Vector3f normal[3];     // normal vector for each vertex

    Texture* tex = nullptr;
    Triangle();

    auto a() const -> Eigen::Vector4f { return v[0]; }
    auto b() const -> Eigen::Vector4f { return v[1]; }
    auto c() const -> Eigen::Vector4f { return v[2]; }

    void setVertex(int ind, Vector4f ver);             /*set i-th vertex coordinates */
    void setNormal(int ind, Vector3f n);               /*set i-th vertex normal vector*/
    void setColor(int ind, float r, float g, float b); /*set i-th vertex color*/

    void setNormals(const std::array<Vector3f, 3>& normals);
    void setColors(const std::array<Vector3f, 3>& colors);
    void setTexCoord(int ind, Vector2f uv); /*set i-th vertex texture coordinate*/
    auto toVector4() const -> std::array<Vector4f, 3>;

    // 返回第i个点的法向量
    auto getNormal(int ind) const -> Vector3f { return normal[ind]; }
    // 返回第i个点的像素值
    auto getColor(int ind) const -> Vector3f { return color[ind]; }
    // 返回第i个点的纹理坐标
    auto getTexCoord(int ind) const -> Vector2f { return tex_coords[ind]; }
};

#endif // RASTERIZER_TRIANGLE_H
