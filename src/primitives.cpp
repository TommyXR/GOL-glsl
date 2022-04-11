#include "primitives.hpp"

#include "shader.hpp"


namespace glu {



Buffer::Buffer() {
    glCreateBuffers(1, &id);
}

Buffer::~Buffer() {
    if (id != 0) {
        glDeleteBuffers(1, &id);
    }
}


VAO::VAO() {
    glCreateVertexArrays(1, &id);
}

VAO::~VAO() {
    if (id != 0) {
        glDeleteVertexArrays(1, &id);
    }
}


void VAO::bind_v(const Buffer& buffer, unsigned int binding, GLintptr offset, int stride) const {
    glVertexArrayVertexBuffer(id, binding, buffer.get_id(), offset, stride);
}


void VAO::bind_e(const Buffer& buffer) const {
    glVertexArrayElementBuffer(id, buffer.get_id());
}


void VAO::activate() const {
    glBindVertexArray(id);
}

void VAO::deactivate() const {
    glBindVertexArray(0);
}


void VAO::activate_vertex_attrib(unsigned int binding, unsigned int attrib, int size, unsigned int type, unsigned int offset) const {
    glEnableVertexArrayAttrib(id, attrib);
    glVertexArrayAttribBinding(id, attrib, binding);
    glVertexArrayAttribFormat(id, attrib, size, type, GL_FALSE, offset);
}


Quad::Quad() {

    vbo.set_data<float>(vertices);
    ebo.set_data<unsigned int>(indices);

    vao.bind_v(vbo, 0, 0, 5 * sizeof(float));
    vao.bind_e(ebo);

    // position
    vao.activate_vertex_attrib(0, 0, 3, GL_FLOAT, 0);


    // tex coords
    vao.activate_vertex_attrib(0, 1, 2, GL_FLOAT, 3 * sizeof(float));
}

void Quad::draw(const Pipeline& pipeline) {
    pipeline.activate();
    vao.activate();

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    vao.deactivate();
    pipeline.deactivate();
}



} // namespace glu