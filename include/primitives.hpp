#pragma once

#include <array>
#include <span>

#include <glad/gl.h>


namespace glu {



class Pipeline;


class Buffer {

  public:
    Buffer();
    ~Buffer();

    Buffer(const Buffer&) = delete;

    unsigned int get_id() const { return id; }

    template<typename T>
    void set_data(std::span<const T> data) {
        glNamedBufferData(id, data.size_bytes(), data.data(), GL_STATIC_DRAW);
        // size = data.size();
    }


  private:
    unsigned int id = 0;
    // std::size_t size = 0;
};



class VAO {

  public:
    VAO();
    ~VAO();

    VAO(const VAO&) = delete;
    VAO& operator=(const VAO&) = delete;

    void bind_v(const Buffer&, unsigned int binding, GLintptr offset, int stride) const;
    void bind_e(const Buffer&) const;

    void activate() const;
    void deactivate() const;

    void activate_vertex_attrib(unsigned int binding, unsigned int attrib, int size, unsigned int type, unsigned int offset) const;

  private:
    unsigned int id = 0;
};



class Quad {
  public:
    static constexpr std::array<float, 20> vertices{
          -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f};

    static constexpr std::array<unsigned int, 6> indices{0, 1, 2, 2, 3, 0};

    Quad();

    void draw(const Pipeline&);

  private:
    VAO vao;
    Buffer vbo;
    Buffer ebo;
};



} // namespace glu