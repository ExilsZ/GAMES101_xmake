#include "rasterizer.hpp"
#include <iostream>
#include <opencv2/opencv.hpp>

constexpr double MY_PI = 3.1415926;

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

auto get_model_matrix(float rotation_angle) -> Eigen::Matrix4f {
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

auto get_projection_matrix(float eye_fov, float aspect_ratio, float zNear,
                           float zFar) -> Eigen::Matrix4f {
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

    if (argc == 2) {
        command_line = true;
        filename     = std::string(argv[1]);
    }

    rst::rasterizer r(700, 700);

    Eigen::Vector3f eye_pos = {0, 0, 5};

    std::vector<Eigen::Vector3f> pos{
        {  2,   0, -2},
        {  0,   2, -2},
        { -2,   0, -2},
        {3.5,  -1, -5},
        {2.5, 1.5, -5},
        { -1, 0.5, -5}
    };

    std::vector<Eigen::Vector3i> ind{
        {0, 1, 2},
        {3, 4, 5}
    };

    std::vector<Eigen::Vector3f> cols{
        {217.0, 238.0, 185.0},
        {217.0, 238.0, 185.0},
        {217.0, 238.0, 185.0},
        {185.0, 217.0, 238.0},
        {185.0, 217.0, 238.0},
        {185.0, 217.0, 238.0}
    };

    auto pos_id = r.load_positions(pos);
    auto ind_id = r.load_indices(ind);
    auto col_id = r.load_colors(cols);

    int key         = 0;
    int frame_count = 0;

    if (command_line) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, col_id, rst::Primitive::Triangle);
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
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, col_id, rst::Primitive::Triangle);

        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0F);
        cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
        cv::imshow("image", image);
        key = cv::waitKey(10);

        std::cout << "frame count: " << frame_count++ << '\n';
        if (key == 'a') {
            angle += 6;
        } else if (key == 'd') {
            angle -= 6;
        }
    }

    return 0;
}
