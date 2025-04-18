#include <array>

#include "buffer.h"
#include "shader.h"
#include "vao.h"
#include "voxel_parser.h"
#include "window.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/type_ptr.hpp>

glm::vec4 constexpr background_color{1, 1, 1, 1};

template <size_t N>
constexpr std::array<float, N>
operator+(std::array<float, N> const& lhs, std::array<float, N> const& rhs)
{
    std::array<float, N> result{};
    for (size_t i = 0; i < N; i++) {
        result[i] = lhs[i] + rhs[i];
    }
    return result;
}

void add_cube(
    std::vector<std::array<float, 3>>& positions,
    std::vector<std::array<float, 3>>& colors,
    std::vector<std::array<float, 2>>& uvs,
    std::array<float, 3> position,
    std::array<float, 3> color
)
{
    size_t constexpr vertex_count{36};
    std::array<std::array<float, 3>, 8> constexpr positions_gen{
        {{0, 0, 0},
         {1, 0, 0},
         {1, 1, 0},
         {0, 1, 0},
         {0, 0, 1},
         {1, 0, 1},
         {1, 1, 1},
         {0, 1, 1}}
    };
    std::array<std::array<float, 2>, 4> constexpr uvs_gen{
        {{0, 0}, {1, 0}, {1, 1}, {0, 1}}
    };
    std::array<size_t, vertex_count> constexpr position_indexes = {
        0, 1, 3, 3, 1, 2, 1, 5, 2, 2, 5, 6, 5, 4, 6, 6, 4, 7, 4,
        0, 7, 7, 0, 3, 3, 2, 7, 7, 2, 6, 4, 5, 0, 0, 5, 1

    };
    std::array<size_t, vertex_count / 6> constexpr uv_indexes =
        {0, 1, 3, 3, 1, 2};

    for (size_t i = 0; i < vertex_count; i++) {
        positions.push_back(positions_gen[position_indexes[i]] + position);
        colors.push_back(color);
        uvs.push_back(uvs_gen[uv_indexes[i % 6]]);
    }
}

int main(int argc, char** argv)
{
    Window window{};
    ShaderProgram basic_s{
        {{"res/shaders/basic.vert.glsl", GL_VERTEX_SHADER},
         {"res/shaders/basic.frag.glsl", GL_FRAGMENT_SHADER}}
    };

    auto const [obj_positions, obj_palette] =
        argc > 1 ? parse_vox_file(argv[1]) : parse_vox_file("res/teapot.vox");
    std::vector<std::array<float, 3>> position_v{};
    std::vector<std::array<float, 3>> color_v{};
    std::vector<std::array<float, 2>> uv_v{};
    for (auto const& p : obj_positions) {
        std::array<float, 3> position{
            static_cast<float>(p[0]),
            static_cast<float>(p[2]),
            static_cast<float>(p[1])
        };
        std::array<float, 3> color{
            static_cast<float>(obj_palette[p[3]][0]) / 255.0f,
            static_cast<float>(obj_palette[p[3]][1]) / 255.0f,
            static_cast<float>(obj_palette[p[3]][2]) / 255.0f
        };
        add_cube(position_v, color_v, uv_v, position, color);
    }

    StaticBuffer position_b{position_v, GL_ARRAY_BUFFER};
    StaticBuffer color_b{color_v, GL_ARRAY_BUFFER};
    StaticBuffer uv_b{uv_v, GL_ARRAY_BUFFER};

    VertexArrayObject vao{};
    vao.attach_shader(basic_s);
    vao.attach_buffer_object("v_position", position_b);
    vao.attach_buffer_object("v_color", color_b);
    vao.attach_buffer_object("v_uv", uv_b);

    while (!glfwWindowShouldClose(window.get_glfw_window())) {
        glfwPollEvents();
        if (Window::mouse_capture) {
            camera_update_per_frame(window.get_glfw_window());
            basic_s.update_uniform("view", Window::view);
            basic_s.update_uniform("proj", Window::proj);
        }

        // clear screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearBufferfv(GL_COLOR, 0, glm::value_ptr(background_color));

        vao.bind();
        glDrawArrays(GL_TRIANGLES, 0, position_v.size());

        // swap buffers
        glfwSwapBuffers(window.get_glfw_window());
    }
}
