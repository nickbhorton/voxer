#include <array>

#include "buffer.h"
#include "glad/gl.h"
#include "shader.h"
#include "vao.h"
#include "voxel_parser.h"
#include "window.h"

#include <array>
#include <cstdint>
#include <cstdlib>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <vector>

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

uint32_t constexpr g_chunk_size = 256;
uint32_t constexpr g_max_voxels_in_chunk = g_chunk_size * g_chunk_size * g_chunk_size;

static uint32_t calculate_index(uint32_t x, uint32_t y, uint32_t z)
{
    return x + (y * g_chunk_size * g_chunk_size) + (z * g_chunk_size);
}

void set_mask(std::array<uint32_t, g_max_voxels_in_chunk>* mask, uint32_t x, uint32_t y, uint32_t z, uint32_t val)
{
    uint32_t const index = calculate_index(x, y, z);
    mask->at(index) = val;
}

uint32_t test_mask(std::array<uint32_t, g_max_voxels_in_chunk> const* mask, int32_t x, int32_t y, int32_t z)
{
    // if testing outside of chunk give false
    // this is a stop gap before introducing multiple chunks
    if (x > (int)g_chunk_size - 1 || y > (int)g_chunk_size - 1 || z > (int)g_chunk_size - 1) {
        return 0;
    }
    if (x < 0 || y < 0 || z < 0) {
        return 0;
    }

    uint32_t const index = calculate_index(x, y, z);
    return mask->at(index);
}

std::vector<std::vector<uint32_t>> generate_mesh(std::array<uint32_t, g_max_voxels_in_chunk> const* mask)
{
    std::vector<std::vector<uint32_t>> faces{};
    for (size_t i = 0; i < 6; i++) {
        faces.push_back({});
    }
    for (int64_t xi = 0; xi < g_chunk_size; xi++) {
        for (int64_t zi = 0; zi < g_chunk_size; zi++) {
            for (int64_t yi = 0; yi < g_chunk_size; yi++) {
                int64_t x = xi;
                int64_t y = yi;
                int64_t z = zi;

                uint32_t result = test_mask(mask, x, y, z);
                if (result) {
                    // down
                    if (!test_mask(mask, x, y - 1, z)) {
                        faces[0].push_back(test_mask(mask, xi, yi, zi));
                    }
                    // up
                    if (!test_mask(mask, x, y + 1, z)) {
                        faces[1].push_back(test_mask(mask, xi, yi, zi));
                    }
                    // north
                    if (!test_mask(mask, x, y, z + 1)) {
                        faces[2].push_back(test_mask(mask, xi, yi, zi));
                    }
                    // east
                    if (!test_mask(mask, x - 1, y, z)) {
                        faces[3].push_back(test_mask(mask, xi, yi, zi));
                    }
                    // south
                    if (!test_mask(mask, x, y, z - 1)) {
                        faces[4].push_back(test_mask(mask, xi, yi, zi));
                    }
                    // west
                    if (!test_mask(mask, x + 1, y, z)) {
                        faces[5].push_back(test_mask(mask, xi, yi, zi));
                    }
                }
            }
        }
    }
    /*
    for (size_t i = 0; i < 6; i++) {
        std::cout << faces[i].size() << "\n";
    }
         */
    return faces;
}

uint32_t pack_float(std::array<float, 3> in, uint8_t cidx)
{
    uint8_t px = static_cast<uint8_t>(in[0]);
    uint8_t py = static_cast<uint8_t>(in[1]);
    uint8_t pz = static_cast<uint8_t>(in[2]);
    uint32_t p = (((uint32_t)cidx) << 24) | (((uint32_t)pz) << 16) | (((uint32_t)py) << 8) | ((uint32_t)px);
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
        {{"res/shaders/6.vert.glsl", GL_VERTEX_SHADER}, {"res/shaders/2.frag.glsl", GL_FRAGMENT_SHADER}}
    };
    basic_s.update_uniform_array("colors", palette_float);

    // geometry for instance rendering
    StaticBuffer face_position_geometry_b{face_position_geometry, GL_ARRAY_BUFFER};

    std::vector<VertexArrayObject> chunks{};
    std::vector<glm::vec3> chunk_offsets{};
    std::vector<size_t> chunk_draw_sizes{};
    int64_t bytes{};
    float repeat_size{128.0};
    for (size_t z = 0; z < 1; z++) {
        for (size_t y = 0; y < 4; y++) {
            for (size_t x = 0; x < 4; x++) {
                std::unique_ptr<std::array<uint32_t, g_max_voxels_in_chunk>> voxel_mask{};
                voxel_mask = std::make_unique<std::array<uint32_t, g_max_voxels_in_chunk>>();
                for (auto const& p : obj_positions) {
                    uint32_t val = (((uint32_t)p[3]) << 24) | (((uint32_t)p[1]) << 16) | (((uint32_t)p[2]) << 8) |
                                   ((uint32_t)p[0]);
                    set_mask(voxel_mask.get(), p[0], p[2], p[1], val);
                }
                // down, up, north, east, south, west
                auto faces = generate_mesh(voxel_mask.get());
                std::vector<uint32_t> data_v{};
                std::vector<uint32_t> dir_v{};

                for (size_t i = 0; i < 6; i++) {
                    for (size_t j = 0; j < faces[i].size(); j++) {
                        data_v.push_back(faces[i][j]);
                        dir_v.push_back(i);
                    }
                }
                // std::cout << "total: " << data_v.size() << "\n";

                StaticBuffer data_b{data_v, GL_ARRAY_BUFFER};
                StaticBuffer dir_b{dir_v, GL_ARRAY_BUFFER};

                VertexArrayObject vao{};
                vao.attach_shader(basic_s);
                // divisor of 6 for each face
                vao.attach_buffer_object("v_data", std::move(data_b), 1);
                vao.attach_buffer_object("direction", std::move(dir_b), 1);
                vao.attach_buffer_object("v_position", face_position_geometry_b);
                chunks.push_back(std::move(vao));
                chunk_offsets.push_back(
                    {static_cast<float>(x) * repeat_size,
                     static_cast<float>(y) * repeat_size,
                     static_cast<float>(z) * repeat_size}
                );
                chunk_draw_sizes.push_back(data_v.size());

                bytes += data_v.size() * sizeof(data_v[0]);
                bytes += dir_v.size() * sizeof(dir_v[0]);
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
                chunk_draw_sizes[i]
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
