#pragma once

#include <glm/glm.hpp>


namespace glu {



class Texture {
  public:
    struct Resolution {
        unsigned int x;
        unsigned int y;
    };

    Texture();
    Texture(Resolution);

    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    unsigned int get_id() const { return id; }


    void bind_to_image_unit(unsigned int unit) const;
    void bind_to_texture_unit(unsigned int unit) const;



  private:
    unsigned int id;
};



} // namespace glu
