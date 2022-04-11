#pragma once

#include <filesystem>
#include <string_view>

#include <glm/glm.hpp>


namespace glu {

class Shader {
  public:
    enum class Type { Vertex, Fragment, Geometry, Compute };

    Shader(Type, const std::filesystem::path&);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& other);
    Shader& operator=(Shader&&);


    unsigned int get_id() const { return id; }

    Type get_type() const { return type; }


    void set_uniform(std::string_view name, bool) const;
    void set_uniform(std::string_view name, int) const;
    void set_uniform(std::string_view name, unsigned int) const;
    void set_uniform(std::string_view name, float) const;
    void set_uniform(std::string_view name, const glm::vec2&) const;
    void set_uniform(std::string_view name, const glm::vec3&) const;
    void set_uniform(std::string_view name, const glm::mat4&) const;


  private:
    Type type;
    unsigned int id = 0;
};


class Pipeline {
  public:
    Pipeline();
    ~Pipeline();

    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;

    Pipeline(Pipeline&&);
    Pipeline& operator=(Pipeline&&);


    void attach(const Shader&) const;

    void activate() const;
    void deactivate() const;


  private:
    unsigned int id = 0;
};

} // namespace glu
