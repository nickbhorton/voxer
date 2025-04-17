#include <array>

#include "buffer.h"
#include "shader.h"
#include "vao.h"
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
    std::array<size_t, vertex_count> constexpr position_indexes = {
        0, 1, 3, 3, 1, 2, 1, 5, 2, 2, 5, 6, 5, 4, 6, 6, 4, 7, 4,
        0, 7, 7, 0, 3, 3, 2, 7, 7, 2, 6, 4, 5, 0, 0, 5, 1

    };
    for (size_t i = 0; i < vertex_count; i++) {
        positions.push_back(positions_gen[position_indexes[i]] + position);
        colors.push_back(color);
    }
}

int main()
{
    Window window{};
    ShaderProgram basic_s{
        {{"res/shaders/basic.vert.glsl", GL_VERTEX_SHADER},
         {"res/shaders/basic.frag.glsl", GL_FRAGMENT_SHADER}}
    };

    std::vector<std::array<float, 3>> position_v{};
    std::vector<std::array<float, 3>> color_v{};
    add_cube(position_v, color_v, {0, 0, 0}, {0, 0, 0});
    add_cube(position_v, color_v, {1, 0, 0}, {1, 0, 0});
    add_cube(position_v, color_v, {1, 1, 0}, {1, 1, 0});
    add_cube(position_v, color_v, {0, 1, 1}, {0, 1, 1});

    StaticBuffer position_b{position_v, GL_ARRAY_BUFFER};
    StaticBuffer color_b{color_v, GL_ARRAY_BUFFER};
    VertexArrayObject vao{};
    vao.attach_shader(basic_s);
    vao.attach_buffer_object("v_position", position_b);
    vao.attach_buffer_object("v_color", color_b);

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
