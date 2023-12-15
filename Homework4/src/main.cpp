#include <iostream>
#include <opencv2/opencv.hpp>

std::vector<cv::Point2f> control_points;

void mouse_handler(int event, int x, int y, int flags, void* userdata) {
    if (event == cv::EVENT_LBUTTONDOWN && control_points.size() < 10) {
        std::cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")\n";
        control_points.emplace_back(x, y);
    }
}

void naive_bezier(const std::vector<cv::Point2f>& points, cv::Mat& window) {
    const auto& p_0 = points[0];
    const auto& p_1 = points[1];
    const auto& p_2 = points[2];
    const auto& p_3 = points[3];

    for (double t = 0.0; t <= 1.0; t += 0.001) {
        auto point = std::pow(1 - t, 3) * p_0 + 3 * t * std::pow(1 - t, 2) * p_1 +
                     3 * std::pow(t, 2) * (1 - t) * p_2 + std::pow(t, 3) * p_3;

        window.at<cv::Vec3b>(point.y, point.x)[2] = 255;
    }
}

// 计算阶乘
constexpr auto factorial(int n) -> int {
    if (n <= 1) { return 1; }
    return n * factorial(n - 1);
}

/**
 * 计算两个给定整数的二项式系数
 *
 * @param n 表示集合中的元素总数。
 * @param k 表示从集合中选择的元素的数量。
 *
 * @return n 和 k 的二项式系数
 */
auto binomial_coeff(int n, int k) -> int {
    return factorial(n) / (factorial(k) * factorial(n - k));
}

auto recursive_bezier(const std::vector<cv::Point2f>& control_points, float t) -> cv::Point2f {
    // TODO: Implement de Casteljau's algorithm
    int n{static_cast<int>(control_points.size() - 1)};

    cv::Point2f result = {0.0, 0.0};
    for (int i = 0; i <= n; i++) {
        float coeff  = binomial_coeff(n, i) * pow(t, i) * pow(1 - t, n - i);
        result.x    += control_points[i].x * coeff;
        result.y    += control_points[i].y * coeff;
    }
    return result;
}

void bezier(const std::vector<cv::Point2f>& control_points, cv::Mat& window) {
    // TODO: Iterate through all t = 0 to t = 1 with small steps, and call de Casteljau's
    // recursive Bezier algorithm.
    cv::Point2f p{0.0, 0.0};
    for (float t = 0.0; t <= 1.0;) {
        p = recursive_bezier(control_points, t);

        window.at<cv::Vec3b>(p.y, p.x)[1] = 255;

        t += 0.001;
    }
}

auto main() -> int {
    unsigned int n{};
    std::cout << "输入控制点个数，最大为 10:\n";
    std::cin >> n;

    cv::Mat window = cv::Mat(720, 720, CV_8UC3, cv::Scalar(0));
    cv::cvtColor(window, window, cv::COLOR_BGR2RGB);
    cv::namedWindow("Bezier Curve", cv::WINDOW_AUTOSIZE);

    cv::setMouseCallback("Bezier Curve", mouse_handler, nullptr);

    int key = -1;

    while (key != 27) {
        for (auto& point : control_points) { cv::circle(window, point, 3, {255, 255, 255}, 3); }

        if (control_points.size() == n) {
            // naive_bezier(control_points, window);
            bezier(control_points, window);

            cv::imshow("Bezier Curve", window);
            cv::imwrite("out/my_bezier_curve.png", window);
            key = cv::waitKey(0);

            return 0;
        }

        cv::imshow("Bezier Curve", window);
        key = cv::waitKey(20);
    }

    return 0;
}
