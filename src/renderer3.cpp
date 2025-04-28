#include <array>

#include "buffer.h"
#include "shader.h"
#include "vao.h"
#include "voxel_parser.h"
#include "window.h"

#include <cstdlib>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/type_ptr.hpp>

glm::vec4 constexpr background_color{1, 1, 1, 1};

template <size_t N>
constexpr std::array<float, N> operator+(std::array<float, N> const& lhs, std::array<float, N> const& rhs)
{
    std::array<float, N> result{};
    for (size_t i = 0; i < N; i++) {
        result[i] = lhs[i] + rhs[i];
    }
    return result;
}

uint32_t pack_float(std::array<float, 3> in)
{
    uint8_t px = static_cast<uint8_t>(in[0]);
    uint8_t py = static_cast<uint8_t>(in[1]);
    uint8_t pz = static_cast<uint8_t>(in[2]);
    uint32_t p = (((uint32_t)pz) << 16) | (((uint32_t)py) << 8) | ((uint32_t)px);
    return p;
}

void add_cube(
    std::vector<uint32_t>& positions,
    std::vector<uint32_t>& colors,
    std::array<float, 3> position,
    std::array<float, 3> color
)
{
    size_t constexpr vertex_count{36};
    std::array<std::array<float, 3>, 8> constexpr positions_gen{
        {{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}, {0, 0, 1}, {1, 0, 1}, {1, 1, 1}, {0, 1, 1}}
    };
    // std::array<std::array<float, 2>, 4> constexpr uvs_gen{{{0, 0}, {1, 0}, {1, 1}, {0, 1}}};
    std::array<size_t, vertex_count> constexpr position_indexes = {
        0, 1, 3, 3, 1, 2, 1, 5, 2, 2, 5, 6, 5, 4, 6, 6, 4, 7, 4,
        0, 7, 7, 0, 3, 3, 2, 7, 7, 2, 6, 4, 5, 0, 0, 5, 1

    };
    // std::array<size_t, vertex_count / 6> constexpr uv_indexes = {0, 1, 3, 3, 1, 2};

    for (size_t i = 0; i < vertex_count; i++) {
        positions.push_back(pack_float(positions_gen[position_indexes[i]] + position));
        colors.push_back(pack_float(color));
    }
}

int main(int argc, char** argv)
{
    auto const [obj_positions, obj_palette] = argc > 1 ? parse_vox_file_2(argv[1]) : parse_vox_file_2("res/teapot.vox");

    Window window{};
    ShaderProgram basic_s{
        {{"res/shaders/3.vert.glsl", GL_VERTEX_SHADER}, {"res/shaders/1.frag.glsl", GL_FRAGMENT_SHADER}}
    };

    std::vector<VertexArrayObject> chunks{};
    std::vector<glm::vec3> chunk_offsets{};
    std::vector<size_t> chunk_draw_sizes{};
    int64_t bytes{};
    for (size_t z = 0; z < 1; z++) {
        for (size_t y = 0; y < 4; y++) {
            for (size_t x = 0; x < 4; x++) {
                std::vector<uint32_t> position_v{};
                std::vector<uint32_t> color_v{};
                for (auto const& p : obj_positions) {
                    std::array<float, 3> position{
                        static_cast<float>(p[0]),
                        static_cast<float>(p[2]),
                        static_cast<float>(p[1]),
                    };
                    std::array<float, 3> color{
                        static_cast<float>(obj_palette[p[3]][0]),
                        static_cast<float>(obj_palette[p[3]][1]),
                        static_cast<float>(obj_palette[p[3]][2])
                    };
                    add_cube(position_v, color_v, position, color);
                }

                StaticBuffer position_b{position_v, GL_ARRAY_BUFFER};
                StaticBuffer color_b{color_v, GL_ARRAY_BUFFER};

                VertexArrayObject vao{};
                vao.attach_shader(basic_s);
                vao.attach_buffer_object("v_position", std::move(position_b));
                vao.attach_buffer_object("v_color", std::move(color_b));

                chunks.push_back(std::move(vao));
                chunk_offsets.push_back(
                    {static_cast<float>(x) * 256.0, static_cast<float>(y) * 256.0, static_cast<float>(z) * 256.0}
                );
                chunk_draw_sizes.push_back(position_v.size());

                bytes += position_v.size() * sizeof(position_v[0]);
                bytes += color_v.size() * sizeof(color_v[0]);
            }
        }
    }
    std::cout << bytes / 1073741824.0f << " Gb of VRAM\n";

    int frame_count = 0;
    double total_time = 0.0;
    while (!glfwWindowShouldClose(window.get_glfw_window())) {
        double t1 = glfwGetTime();
        glfwPollEvents();
        if (Window::mouse_capture) {
            camera_update_per_frame(window.get_glfw_window());
            basic_s.update_uniform("view", Window::view);
            basic_s.update_uniform("proj", Window::proj);
        }

        // clear screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearBufferfv(GL_COLOR, 0, glm::value_ptr(background_color));

        for (size_t i = 0; i < chunks.size(); i++) {
            chunks[i].bind();
            basic_s.bind();
            basic_s.update_uniform("offset", chunk_offsets[i]);
            glDrawArrays(GL_TRIANGLES, 0, chunk_draw_sizes[i]);
        }

        // swap buffers
        glfwSwapBuffers(window.get_glfw_window());
        double t2 = glfwGetTime();
        frame_count++;
        total_time += (t2 - t1);
    }
    std::cout << "FPS over " << frame_count << " frames: " << frame_count / total_time << "\n";
}
