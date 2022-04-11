#include "texture.hpp"

#include <glad/gl.h>


namespace glu {


Texture::Texture() {

    glCreateTextures(GL_TEXTURE_2D, 1, &id);

    glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

Texture::Texture(Resolution res): Texture() {
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, res.x, res.y, 0, GL_RGBA, GL_FLOAT, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
}


Texture::~Texture() {
    if (id != 0) {
        glDeleteTextures(1, &id);
    }
}


void Texture::bind_to_image_unit(unsigned int unit) const {
    glBindImageTexture(unit, id, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
}

void Texture::bind_to_texture_unit(unsigned int unit) const {
    glBindTextureUnit(unit, id);
}


} // namespace glu