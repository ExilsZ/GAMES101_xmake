#include "OBJ_Loader.h"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Triangle.hpp"
#include "eigen3/Eigen/src/Core/Matrix.h"
#include "global.hpp"
#include "rasterizer.hpp"
#include <cmath>
#include <iostream>
#include <opencv2/opencv.hpp>

auto get_view_matrix(Eigen::Vector3f eye_pos) -> Eigen::Matrix4f {
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();
    Eigen::Vector3f g    = -eye_pos; // 视线方向
    g.normalize();
    Eigen::Vector3f t = {0, 1, 0}; // 视线向上方向
    Eigen::Vector3f r = g.cross(t);
    view << r.x(), r.y(), r.z(), 0, t.x(), t.y(), t.z(), 0, -g.x(), -g.y(), -g.z(), 0, 0, 0, 0, 1;

    Eigen::Matrix4f translate;
    translate << 1, 0, 0, -eye_pos[0], 0, 1, 0, -eye_pos[1], 0, 0, 1, -eye_pos[2], 0, 0, 0, 1;
    view = translate * view;
    return view;
}

auto get_model_matrix(float angle) -> Eigen::Matrix4f {
    Eigen::Matrix4f rotation;
    angle = angle * MY_PI / 180.F;
    rotation << cos(angle), 0, sin(angle), 0, 0, 1, 0, 0, -sin(angle), 0, cos(angle), 0, 0, 0, 0, 1;

    Eigen::Matrix4f scale;
    scale << 2.5, 0, 0, 0, 0, 2.5, 0, 0, 0, 0, 2.5, 0, 0, 0, 0, 1;

    Eigen::Matrix4f translate;
    translate << 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1;

    return translate * rotation * scale;
}

auto get_projection_matrix(float eye_fov, float aspect_ratio, float zNear,
                           float zFar) -> Eigen::Matrix4f {
    // MARK: Use the same projection matrix from the previous assignments

    // 1. 透视投影，将透视投影范围坐标转换到正交投影
    float           n = zNear;
    float           f = zFar;
    Eigen::Matrix4f M_persp2ortho;
    // clang-format off
    M_persp2ortho << n, 0,     0,      0,
                     0, n,     0,      0,
                     0, 0, n + f, -n * f,
                     0, 0,     1,      0;
    // clang-format on

    // 2. 正交投影，将坐标转换到规范立方体 (-1, 1)^3，先平移再缩放
    float fov = eye_fov * MY_PI / 180.0;

    float t = -n * tan(fov / 2.);
    float b = -t;
    float r = aspect_ratio * t;
    float l = -r;

    Eigen::Matrix4f M_ortho;
    Eigen::Matrix4f trans;
    Eigen::Matrix4f scale;
    // clang-format off
    trans << 1, 0, 0, -(r + l) / 2,
             0, 1, 0, -(t + b) / 2,
             0, 0, 1, -(n + f) / 2,
             0, 0, 0,            1;

    scale << 2 / (r - l),           0,           0, 0,
                       0, 2 / (t - b),           0, 0,
                       0,           0, 2 / (n - f), 0,
                       0,           0,           0, 1;
    // clang-format on
    M_ortho = scale * trans;

    Eigen::Matrix4f projection = M_ortho * M_persp2ortho;
    return projection;
}

auto vertex_shader(const VertexShaderPayload& payload) -> Eigen::Vector3f {
    return payload.position;
}

static auto reflect(const Eigen::Vector3f& vec, const Eigen::Vector3f& axis) -> Eigen::Vector3f {
    auto costheta = vec.dot(axis);
    return (2 * costheta * axis - vec).normalized();
}

struct light {
    Eigen::Vector3f position;  // 光源位置
    Eigen::Vector3f intensity; // 光源亮度
};

auto phong_fragment_shader(const FragmentShaderPayload& payload) -> Eigen::Vector3f {
    Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
    Eigen::Vector3f kd = payload.color;
    Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

    auto l1 = light{
        { 20,  20,  20},
        {500, 500, 500}
    };
    auto l2 = light{
        {-20,  20,   0},
        {500, 500, 500}
    };

    std::vector<light> lights = {l1, l2};
    Eigen::Vector3f    amb_light_intensity{10, 10, 10};
    Eigen::Vector3f    eye_pos{0, 0, 10};

    float p = 150;

    // Eigen::Vector3f color  = payload.color;
    Eigen::Vector3f point  = payload.view_pos;
    Eigen::Vector3f normal = payload.normal;

    Eigen::Vector3f result_color = {0, 0, 0};
    for (auto& light : lights) {
        // MARK: For each light source in the code, calculate what the *ambient*,
        // *diffuse*, and *specular* components are. Then, accumulate that result on the
        // *result_color* object.
        Eigen::Vector3f light_dir   = (light.position - point).normalized();
        Eigen::Vector3f view_dir    = (eye_pos - point).normalized();
        Eigen::Vector3f bisector    = (light_dir + view_dir).normalized();
        float light_distance_square = (light.position - point).dot(light.position - point);

        Eigen::Vector3f diffuse = kd.cwiseProduct(light.intensity / light_distance_square) *
                                  std::max(0.0F, light_dir.dot(normal));

        Eigen::Vector3f specular = ks.cwiseProduct(light.intensity / light_distance_square) *
                                   std::pow(std::max(0.0F, bisector.dot(normal)), p);

        result_color += (diffuse + specular);
    }
    Eigen::Vector3f ambient  = ka.cwiseProduct(amb_light_intensity);
    result_color            += ambient;

    return result_color * 255.F;
}

auto texture_fragment_shader(const FragmentShaderPayload& payload) -> Eigen::Vector3f {
    Eigen::Vector3f return_color = {0, 0, 0};
    if (payload.texture != nullptr) {
        // MARK: Get the texture value at the texture coordinates of the current
        // fragment
        return_color = payload.texture->getColor(payload.tex_coords.x(), payload.tex_coords.y());
    }

    Eigen::Vector3f texture_color;
    texture_color << return_color.x(), return_color.y(), return_color.z();

    Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
    Eigen::Vector3f kd = texture_color / 255.F;
    Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

    auto l1 = light{
        { 20,  20,  20},
        {500, 500, 500}
    };
    auto l2 = light{
        {-20,  20,   0},
        {500, 500, 500}
    };

    std::vector<light> lights = {l1, l2};
    Eigen::Vector3f    amb_light_intensity{10, 10, 10};
    Eigen::Vector3f    eye_pos{0, 0, 10};

    float p = 200; // 控制高光可视角度

    // Eigen::Vector3f color  = texture_color;
    Eigen::Vector3f point  = payload.view_pos;
    Eigen::Vector3f normal = payload.normal;

    Eigen::Vector3f result_color = {0, 0, 0};

    for (auto& light : lights) {
        // MARK: For each light source in the code, calculate what the *ambient*,
        // *diffuse*, and *specular* components are. Then, accumulate that result on the
        // *result_color* object.
        Eigen::Vector3f light_dir   = (light.position - point).normalized();
        Eigen::Vector3f view_dir    = (eye_pos - point).normalized();
        Eigen::Vector3f bisector    = (light_dir + view_dir).normalized();
        float light_distance_square = (light.position - point).dot(light.position - point);

        Eigen::Vector3f specular = ks.cwiseProduct(light.intensity / light_distance_square) *
                                   std::pow(std::max(0.0F, bisector.dot(normal)), p);
        Eigen::Vector3f diffuse = kd.cwiseProduct(light.intensity / light_distance_square) *
                                  std::max(0.0F, light_dir.dot(normal));

        result_color += (diffuse + specular);
    }
    Eigen::Vector3f ambient  = ka.cwiseProduct(amb_light_intensity);
    result_color            += ambient;

    return result_color * 255.F;
}

// 法线贴图
auto normal_fragment_shader(const FragmentShaderPayload& payload) -> Eigen::Vector3f {
    Eigen::Vector3f return_color =
        (payload.normal.head<3>().normalized() + Eigen::Vector3f(1.0, 1.0, 1.0)) / 2.;
    Eigen::Vector3f result;
    result << return_color.x() * 255, return_color.y() * 255, return_color.z() * 255;
    return result;
}

// 凹凸贴图
auto bump_fragment_shader(const FragmentShaderPayload& payload) -> Eigen::Vector3f {
    Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
    Eigen::Vector3f kd = payload.color;
    Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

    auto l1 = light{
        { 20,  20,  20},
        {500, 500, 500}
    };
    auto l2 = light{
        {-20,  20,   0},
        {500, 500, 500}
    };

    std::vector<light> lights = {l1, l2};
    Eigen::Vector3f    amb_light_intensity{10, 10, 10};
    Eigen::Vector3f    eye_pos{0, 0, 10};

    float p = 150;

    // Eigen::Vector3f color  = payload.color;
    Eigen::Vector3f point  = payload.view_pos;
    Eigen::Vector3f normal = payload.normal;

    // TODO: Implement bump mapping here
    // Let n = normal = (x, y, z)
    // Vector t = (x*y/sqrt(x*x+z*z),sqrt(x*x+z*z),z*y/sqrt(x*x+z*z))
    // Vector b = n cross product t
    // Matrix TBN = [t b n]
    // dU = kh * kn * (h(u+1/w,v)-h(u,v))
    // dV = kh * kn * (h(u,v+1/h)-h(u,v))
    // Vector ln = (-dU, -dV, 1)
    // Normal n = normalize(TBN * ln)

    float kh = 0.2;
    float kn = 0.1;

    Matrix3f TBN{Matrix3f::Identity()}; // TBN 矩阵;
    // T：正切向量 Tangent
    // B：副切向量 Bitangent
    // N：法向量 Normal

    Vector3f n{normal}; // N 向量已知
    float    nx{normal.x()};
    float    ny{normal.y()};
    float    nz{normal.z()};
    Vector3f t{nx * ny / sqrt(nx * nx + nz * nz), sqrt(nx * nx + nz * nz),
               nz * ny / sqrt(nx * nx + nz * nz)};
    Vector3f b{n.cross(t)};

    TBN << t.x(), b.x(), normal.x(), t.y(), b.y(), normal.y(), t.z(), b.z(), normal.z();

    float u = payload.tex_coords.x();  // 点在贴图的 u 坐标;
    float v = payload.tex_coords.y();  // 点在贴图的 v 坐标;
    int   w = payload.texture->width;  // 纹理宽度;
    int   h = payload.texture->height; // 纹理高度;

    float dU = kh * kn *
               (payload.texture->getColor(u + 1.0F / w, v).norm() -
                payload.texture->getColor(u, v).norm());
    float dV = kh * kn *
               (payload.texture->getColor(u, v + 1.0F / h).norm() -
                payload.texture->getColor(u, v).norm());

    Vector3f ln           = {-dU, -dV, 1.0};
    normal                = TBN * ln;
    Vector3f result_color = {0, 0, 0};
    result_color          = normal.normalized();

    return result_color * 255.F;
}

// 移位贴图，改变几何体
auto displacement_fragment_shader(const FragmentShaderPayload& payload) -> Eigen::Vector3f {
    Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
    Eigen::Vector3f kd = payload.color;
    Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

    auto l1 = light{
        { 20,  20,  20},
        {500, 500, 500}
    };
    auto l2 = light{
        {-20,  20,   0},
        {500, 500, 500}
    };

    std::vector<light> lights = {l1, l2};
    Eigen::Vector3f    amb_light_intensity{10, 10, 10};
    Eigen::Vector3f    eye_pos{0, 0, 10};

    float p = 150;

    Eigen::Vector3f color  = payload.color;
    Eigen::Vector3f point  = payload.view_pos;
    Eigen::Vector3f normal = payload.normal;

    // TODO: Implement displacement mapping here
    // Let n = normal = (x, y, z)
    // Vector t = (x*y/sqrt(x*x+z*z),sqrt(x*x+z*z),z*y/sqrt(x*x+z*z))
    // Vector b = n cross product t
    // Matrix TBN = [t b n]
    // dU = kh * kn * (h(u+1/w,v)-h(u,v))
    // dV = kh * kn * (h(u,v+1/h)-h(u,v))
    // Vector ln = (-dU, -dV, 1)
    // Position p = p + kn * n * h(u,v)
    // Normal n = normalize(TBN * ln)
    float kh = 0.2;
    float kn = 0.1;

    Matrix3f TBN{Matrix3f::Identity()}; // TBN 矩阵;

    Vector3f n{normal};
    float    nx{normal.x()};
    float    ny{normal.y()};
    float    nz{normal.z()};
    Vector3f t{nx * ny / sqrt(nx * nx + nz * nz), sqrt(nx * nx + nz * nz),
               nz * ny / sqrt(nx * nx + nz * nz)};
    Vector3f b{n.cross(t)};

    TBN << t.x(), b.x(), normal.x(), t.y(), b.y(), normal.y(), t.z(), b.z(), normal.z();

    float u = payload.tex_coords.x();  // 点在贴图的 u 坐标;
    float v = payload.tex_coords.y();  // 点在贴图的 v 坐标;
    int   w = payload.texture->width;  // 纹理宽度;
    int   h = payload.texture->height; // 纹理高度;

    float dU = kh * kn *
               (payload.texture->getColor(u + 1.0F / w, v).norm() -
                payload.texture->getColor(u, v).norm());
    float dV = kh * kn *
               (payload.texture->getColor(u, v + 1.0F / h).norm() -
                payload.texture->getColor(u, v).norm());

    Vector3f ln = {-dU, -dV, 1.0};
    normal      = (TBN * ln).normalized();

    point += (kn * normal * payload.texture->getColor(u, v).norm()); // 改变顶点坐标的高度

    Eigen::Vector3f result_color = {0, 0, 0};

    for (auto& light : lights) {
        // MARK: For each light source in the code, calculate what the *ambient*,
        // *diffuse*, and *specular* components are. Then, accumulate that result on the
        // *result_color* object.
        Eigen::Vector3f light_dir   = (light.position - point).normalized();
        Eigen::Vector3f view_dir    = (eye_pos - point).normalized();
        Eigen::Vector3f bisector    = (light_dir + view_dir).normalized();
        float light_distance_square = (light.position - point).dot(light.position - point);

        Eigen::Vector3f specular = ks.cwiseProduct(light.intensity / light_distance_square) *
                                   std::pow(std::max(0.0F, bisector.dot(normal)), p);
        Eigen::Vector3f diffuse = kd.cwiseProduct(light.intensity / light_distance_square) *
                                  std::max(0.0F, light_dir.dot(normal));

        result_color += (diffuse + specular);
    }
    Vector3f ambient  = ka.cwiseProduct(amb_light_intensity);
    result_color     += ambient;

    return result_color * 255.F;
}

auto main(int argc, const char** argv) -> int {
    std::vector<Triangle*> TriangleList;

    float angle        = 140.0;
    bool  command_line = false;

    std::string  filename = "out/output.png";
    objl::Loader Loader;
    std::string  obj_path = "res/models/spot/";

    // Load .obj File
    bool loadout = Loader.LoadFile("res/models/spot/spot_triangulated_good.obj");
    for (auto mesh : Loader.LoadedMeshes) {
        for (int i = 0; i < mesh.Vertices.size(); i += 3) {
            auto* t = new Triangle();
            for (int j = 0; j < 3; j++) {
                t->setVertex(j, Vector4f(mesh.Vertices[i + j].Position.X,
                                         mesh.Vertices[i + j].Position.Y,
                                         mesh.Vertices[i + j].Position.Z, 1.0));
                t->setNormal(j,
                             Vector3f(mesh.Vertices[i + j].Normal.X, mesh.Vertices[i + j].Normal.Y,
                                      mesh.Vertices[i + j].Normal.Z));
                t->setTexCoord(j, Vector2f(mesh.Vertices[i + j].TextureCoordinate.X,
                                           mesh.Vertices[i + j].TextureCoordinate.Y));
            }
            TriangleList.push_back(t);
        }
    }

    rst::rasterizer r(700, 700);

    const auto* texture_path = "hmap.jpg";
    r.set_texture(Texture(obj_path + texture_path));

    std::function<Eigen::Vector3f(FragmentShaderPayload)> active_shader = phong_fragment_shader;

    if (argc >= 2) {
        command_line = true;
        filename     = std::string(argv[1]);

        if (argc == 3 && std::string(argv[2]) == "texture") {
            std::cout << "Rasterizing using the texture shader\n";
            active_shader = texture_fragment_shader;
            texture_path  = "spot_texture.png";
            r.set_texture(Texture(obj_path + texture_path));
        } else if (argc == 3 && std::string(argv[2]) == "normal") {
            std::cout << "Rasterizing using the normal shader\n";
            active_shader = normal_fragment_shader;
        } else if (argc == 3 && std::string(argv[2]) == "phong") {
            std::cout << "Rasterizing using the phong shader\n";
            active_shader = phong_fragment_shader;
        } else if (argc == 3 && std::string(argv[2]) == "bump") {
            std::cout << "Rasterizing using the bump shader\n";
            active_shader = bump_fragment_shader;
        } else if (argc == 3 && std::string(argv[2]) == "displacement") {
            std::cout << "Rasterizing using the displacement shader\n";
            active_shader = displacement_fragment_shader;
        }
    }

    Eigen::Vector3f eye_pos = {0, 0, 10};

    r.set_vertex_shader(vertex_shader);
    r.set_fragment_shader(active_shader);

    int key         = 0;
    int frame_count = 0;

    if (command_line) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);
        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45.0, 1, 0.1, 50));

        r.draw(TriangleList);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0F);
        cv::cvtColor(image, image, cv::COLOR_RGB2BGR);

        cv::imwrite(filename, image);

        return 0;
    }

    while (key != 27) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45.0, 1, 0.1, 50));

        // r.draw(pos_id, ind_id, col_id, rst::Primitive::Triangle);
        r.draw(TriangleList);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0F);
        cv::cvtColor(image, image, cv::COLOR_RGB2BGR);

        cv::imshow("image", image);
        cv::imwrite(filename, image);
        key = cv::waitKey(10);

        if (key == 'a') {
            angle -= 0.1;
        } else if (key == 'd') {
            angle += 0.1;
        }
    }
    return 0;
}
