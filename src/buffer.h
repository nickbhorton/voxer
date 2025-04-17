#pragma once

#include <glad/gl.h>
#include <iostream>
#include <vector>

class StaticBuffer
{
public:
    StaticBuffer() = delete;
    template <typename T> StaticBuffer(T buffer_data, GLenum bind_target);
    template <typename T>
    StaticBuffer(std::vector<T> buffer_data, GLenum bind_target);

    StaticBuffer(StaticBuffer const& other) = delete;
    StaticBuffer& operator=(StaticBuffer const& other) = delete;

    StaticBuffer(StaticBuffer&& other) noexcept;
    StaticBuffer& operator=(StaticBuffer&& other) noexcept;

    ~StaticBuffer();

    void bind();
    size_t byte_count() const;
    GLenum get_bind_target() const;
    GLuint get_name() const;
    GLenum get_type() const;
    GLint get_attrib_size() const;

private:
    GLuint name;
    GLenum bind_target;

    GLenum data_type;
    GLint attrib_size;

    size_t data_size;

    bool moved;
};

template <typename T>
StaticBuffer::StaticBuffer(T buffer_data, GLenum bind_target)
{
    std::cerr << "StatisBuffer type not implemented\n";
}

template <>
StaticBuffer::StaticBuffer(
    std::vector<std::array<float, 3>> buffer_data,
    GLenum bind_target
);
template <>
StaticBuffer::StaticBuffer(
    std::vector<uint32_t> buffer_data,
    GLenum bind_target
);
