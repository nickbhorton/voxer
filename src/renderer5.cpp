#include <array>

#include "buffer.h"
#include "glad/gl.h"
#include "shader.h"
#include "vao.h"
#include "voxel_parser.h"
#include "window.h"

#include <bitset>
#include <cstdlib>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/type_ptr.hpp>

glm::vec4 constexpr background_color{1, 1, 1, 1};
std::vector<std::array<float, 3>> const face_position_geometry{{{0, 0, 0}, {1, 0, 0}, {0, 0, 1}, {1, 0, 1}}};

template <size_t N>
constexpr std::array<float, N> operator+(std::array<float, N> const& lhs, std::array<float, N> const& rhs)
{
    std::array<float, N> result{};
    for (size_t i = 0; i < N; i++) {
        result[i] = lhs[i] + rhs[i];
    }
    return result;
}

uint32_t pack_float(std::array<float, 3> in, uint8_t cidx)
{
    uint8_t px = static_cast<uint8_t>(in[0]);
    uint8_t py = static_cast<uint8_t>(in[1]);
    uint8_t pz = static_cast<uint8_t>(in[2]);
    uint32_t p = (((uint32_t)cidx) << 24) | (((uint32_t)pz) << 16) | (((uint32_t)py) << 8) | ((uint32_t)px);
    // std::bitset<32> bs(p);
    // std::cout << bs << "\n";
    return p;
}

void add_cube(std::vector<uint32_t>& positions, std::array<float, 3> position, uint8_t cidx)
{
    positions.push_back(pack_float(position, cidx));
}

int main(int argc, char** argv)
{
    auto const [obj_positions, obj_palette] = argc > 1 ? parse_vox_file_2(argv[1]) : parse_vox_file_2("res/teapot.vox");
    std::array<std::array<float, 4>, 256> palette_float{};
    for (size_t i = 0; i < 256; i++) {
        std::array<float, 4> cc{};
        cc[0] = static_cast<float>(obj_palette[i][0]) / 255;
        cc[1] = static_cast<float>(obj_palette[i][1]) / 255;
        cc[2] = static_cast<float>(obj_palette[i][2]) / 255;
        cc[3] = static_cast<float>(obj_palette[i][3]) / 255;
        palette_float[i] = cc;
    }

    Window window{};
    ShaderProgram basic_s{
        {{"res/shaders/5.vert.glsl", GL_VERTEX_SHADER}, {"res/shaders/2.frag.glsl", GL_FRAGMENT_SHADER}}
    };
    basic_s.update_uniform_array("colors", palette_float);

    // geometry for instance rendering
    StaticBuffer face_position_geometry_b{face_position_geometry, GL_ARRAY_BUFFER};

    std::vector<VertexArrayObject> chunks{};
    std::vector<glm::vec3> chunk_offsets{};
    std::vector<size_t> chunk_draw_sizes{};
    int64_t bytes{};
    for (size_t z = 0; z < 1; z++) {
        for (size_t y = 0; y < 4; y++) {
            for (size_t x = 0; x < 4; x++) {
                std::vector<uint32_t> data_v{};
                for (auto const& p : obj_positions) {
                    std::array<float, 3> position{
                        static_cast<float>(p[0]),
                        static_cast<float>(p[2]),
                        static_cast<float>(p[1]),
                    };
                    add_cube(data_v, position, p[3]);
                }

                StaticBuffer data_b{data_v, GL_ARRAY_BUFFER};

                VertexArrayObject vao{};
                vao.attach_shader(basic_s);
                // divisor of 6 for each face
                vao.attach_buffer_object("v_data", std::move(data_b), 6);
                vao.attach_buffer_object("v_position", face_position_geometry_b);

                chunks.push_back(std::move(vao));
                chunk_offsets.push_back(
                    {static_cast<float>(x) * 256.0, static_cast<float>(y) * 256.0, static_cast<float>(z) * 256.0}
                );
                chunk_draw_sizes.push_back(data_v.size());

                bytes += data_v.size() * sizeof(data_v[0]);
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
            glDrawArraysInstanced(
                GL_TRIANGLE_STRIP,
                0,
                static_cast<GLsizei>(face_position_geometry.size()),
                chunk_draw_sizes[i] * 6
            );
        }

        // swap buffers
        glfwSwapBuffers(window.get_glfw_window());
        double t2 = glfwGetTime();
        frame_count++;
        total_time += (t2 - t1);
    }
    std::cout << "FPS over " << frame_count << " frames: " << frame_count / total_time << "\n";
}
