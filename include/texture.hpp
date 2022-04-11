#pragma once

#include <span>
#include <utility>


#include <glad/gl.h>
#include <glm/glm.hpp>


namespace glu {



class Texture {
  public:
    struct Resolution {
        unsigned int x;
        unsigned int y;
    };

    enum class InternalFormat { RGBA32f, R8ui };
    enum class AccessType { Read, Write };



    Texture(Resolution, InternalFormat);

    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    Texture(Texture&& other) noexcept: id{std::exchange(other.id, 0)}, internal_format{other.internal_format} {}

    Texture& operator=(Texture&& other) noexcept {
        id = std::exchange(other.id, 0);
        internal_format = other.internal_format;

        return *this;
    }


    unsigned int get_id() const { return id; }


    void bind_to_image_unit(unsigned int unit, AccessType) const;
    void bind_to_texture_unit(unsigned int unit) const;


    template<typename T>
    void set_image(std::span<const T> data, Resolution res, InternalFormat format);


  private:
    unsigned int id;

    InternalFormat internal_format;
};

constexpr auto get_flag(Texture::InternalFormat format) {

    switch (format) {
        case Texture::InternalFormat::RGBA32f: return GL_RGBA;
        case Texture::InternalFormat::R8ui: return GL_RED_INTEGER;
    }
}

constexpr auto get_flag(Texture::AccessType type) {
    switch (type) {
        case Texture::AccessType::Read: return GL_READ_ONLY;
        case Texture::AccessType::Write: return GL_WRITE_ONLY;
    }
}

constexpr auto get_internal_flag(Texture::InternalFormat format) {

    switch (format) {
        case Texture::InternalFormat::RGBA32f: return GL_RGBA32F;
        case Texture::InternalFormat::R8ui: return GL_R8UI;
    }
}


constexpr auto get_data_type(Texture::InternalFormat format) {

    switch (format) {
        case Texture::InternalFormat::RGBA32f: return GL_FLOAT;
        case Texture::InternalFormat::R8ui: return GL_UNSIGNED_BYTE;
    }
}



template<typename T>
void Texture::set_image(std::span<const T> data, Resolution res, InternalFormat format) {
    internal_format = format;

    glBindTexture(GL_TEXTURE_2D, id);

    glTexImage2D(GL_TEXTURE_2D, 0, get_internal_flag(format), res.x, res.y, 0, get_flag(format), get_data_type(format), data.data());

    glBindTexture(GL_TEXTURE_2D, 0);
}


} // namespace glu
