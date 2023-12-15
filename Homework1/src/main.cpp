#include "rasterizer.hpp"
#include <eigen3/Eigen/Eigen>
#include <iostream>
#include <opencv2/opencv.hpp>

constexpr double MY_PI = 3.1415926;

// View，平移变换
auto get_view_matrix(Eigen::Vector3f eye_pos) -> Eigen::Matrix4f {
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();
    Eigen::Matrix4f translate;
    // clang-format off
    translate << 1, 0, 0, -eye_pos[0],
                 0, 1, 0, -eye_pos[1],
                 0, 0, 1, -eye_pos[2],
                 0, 0, 0, 1;
    // clang-format on
    view = translate * view;
    return view;
}

// Model，绕 Z 轴的旋转矩阵
auto get_model_matrix(float rotation_angle) -> Eigen::Matrix4f {
    // DONE: Implement this function
    // Create the model matrix for rotating the triangle around the Z axis.
    // Then return it.

    Eigen::Matrix4f rotate;
    float           rad = rotation_angle / 180.0F * MY_PI;
    // clang-format off
    rotate << cos(rad), -sin(rad), 0, 0,
              sin(rad),  cos(rad), 0, 0,
                     0,         0, 1, 0,
                     0,         0, 0, 1;
    // clang-format on
    return rotate;
}

// Projection，eye_fov：垂直视场角，aspect_ratio：宽高比，zNear：近平面，zFar：远平面
auto get_projection_matrix(float eye_fov, float aspect_ratio, float zNear,
                           float zFar) -> Eigen::Matrix4f {

    // DONE: Implement this function
    // Create the projection matrix for the given parameters.
    // Then return it.

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

auto main(int argc, const char** argv) -> int {
    float       angle        = 0;
    bool        command_line = false;
    std::string filename     = "output.png";

    if (argc >= 3) {
        command_line = true;
        angle        = std::stof(argv[2]); // -r by default
        if (argc == 4) { filename = std::string(argv[3]); }
    }

    rst::rasterizer r(700, 700);

    Eigen::Vector3f eye_pos = {0, 0, 5};

    std::vector<Eigen::Vector3f> pos{
        { 2, 0, -2},
        { 0, 2, -2},
        {-2, 0, -2}
    };

    std::vector<Eigen::Vector3i> ind{
        {0, 1, 2}
    };

    auto pos_id = r.load_positions(pos);
    auto ind_id = r.load_indices(ind);

    int key         = 0;
    int frame_count = 0;

    if (command_line) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0F);

        cv::imwrite(filename, image);

        return 0;
    }

    while (key != 27) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);

        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0F);
        cv::imshow("image", image);
        key = cv::waitKey(10);

        std::cout << "frame count: " << frame_count++ << '\n';

        if (key == 'a') {
            angle += 10;
        } else if (key == 'd') {
            angle -= 10;
        }
    }

    return 0;
}
