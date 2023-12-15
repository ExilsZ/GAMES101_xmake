//
// Created by LEI XU on 4/11/19.
//

#ifndef RASTERIZER_TRIANGLE_H
#define RASTERIZER_TRIANGLE_H

#include <eigen3/Eigen/Eigen>

using namespace Eigen;
class Triangle {

  public:
    /*the original coordinates of the triangle, v0, v1, v2 in counter clockwise order*/
    /*Per vertex values*/

    Vector3f v[3];          // 顶点坐标数组
    Vector3f color[3];      // color at each vertex;
    Vector2f tex_coords[3]; // texture u,v
    Vector3f normal[3];     // normal vector for each vertex

    // Texture *tex;
    Triangle();

    void setVertex(int ind, Vector3f ver);                       /*set i-th vertex coordinates */
    void setNormal(int ind, Vector3f n);                         /*set i-th vertex normal vector*/
    void setColor(int ind, float r, float g, float b);           /*set i-th vertex color*/
    auto getColor() const -> Vector3f { return color[0] * 255; } // Only one color per triangle.
    void setTexCoord(int ind, float s, float t); /*set i-th vertex texture coordinate*/

    /**
     * 该函数将三角形的顶点转换为 Eigen::Vector4f 对象的 std::array。
     *
     * @return Vector4f 对象的 std::array。
     */
    auto toVector4() const -> std::array<Vector4f, 3>;
};

#endif // RASTERIZER_TRIANGLE_H
