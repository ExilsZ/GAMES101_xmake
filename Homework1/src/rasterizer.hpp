//
// Created by goksu on 4/6/19.
//

#pragma once

#include "Triangle.hpp"
#include <algorithm>
#include <eigen3/Eigen/Eigen>
using namespace Eigen;

namespace rst {
    enum class Buffers { Color = 1, Depth = 2 };

    inline auto operator|(Buffers a, Buffers b) -> Buffers { return Buffers((int)a | (int)b); }

    inline auto operator&(Buffers a, Buffers b) -> Buffers { return Buffers((int)a & (int)b); }

    enum class Primitive { Line, Triangle };

    /*
     * For the curious : The draw function takes two buffer id's as its arguments.
     * These two structs make sure that if you mix up with their orders, the
     * compiler won't compile it. Aka : Type safety
     * */
    struct pos_buf_id {
        int pos_id = 0;
    };

    struct ind_buf_id {
        int ind_id = 0;
    };

    class rasterizer {
      public:
        rasterizer(int w, int h);
        auto load_positions(const std::vector<Eigen::Vector3f>& positions) -> pos_buf_id;
        auto load_indices(const std::vector<Eigen::Vector3i>& indices) -> ind_buf_id;

        void set_model(const Eigen::Matrix4f& m);
        void set_view(const Eigen::Matrix4f& v);
        void set_projection(const Eigen::Matrix4f& p);

        void set_pixel(const Eigen::Vector3f& point, const Eigen::Vector3f& color);

        void clear(Buffers buff);

        void draw(pos_buf_id pos_buffer, ind_buf_id ind_buffer, Primitive type);

        auto frame_buffer() -> std::vector<Eigen::Vector3f>& { return frame_buf; }

      private:
        void draw_line(Eigen::Vector3f begin, Eigen::Vector3f end);
        void rasterize_wireframe(const Triangle& t);

        Eigen::Matrix4f model;
        Eigen::Matrix4f view;
        Eigen::Matrix4f projection;

        std::map<int, std::vector<Eigen::Vector3f>> pos_buf;
        std::map<int, std::vector<Eigen::Vector3i>> ind_buf;

        std::vector<Eigen::Vector3f> frame_buf;
        std::vector<float>           depth_buf;

        int width, height;
        int next_id = 0;

        auto get_index(int x, int y) -> int;

        auto get_next_id() -> int { return next_id++; }
    };
} // namespace rst
