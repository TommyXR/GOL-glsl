#include "shader.hpp"

#include <fstream>
#include <iostream>
#include <string_view>
#include <utility>
#include <vector>


#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>



namespace glu {


namespace {

constexpr auto get_type_flag(Shader::Type type) {
    switch (type) {
        case Shader::Type::Vertex: return GL_VERTEX_SHADER;
        case Shader::Type::Fragment: return GL_FRAGMENT_SHADER;
        case Shader::Type::Geometry: return GL_GEOMETRY_SHADER;
        case Shader::Type::Compute: return GL_COMPUTE_SHADER;
    }
}

constexpr auto get_bit(Shader::Type type) {
    switch (type) {
        case Shader::Type::Vertex: return GL_VERTEX_SHADER_BIT;
        case Shader::Type::Fragment: return GL_FRAGMENT_SHADER_BIT;
        case Shader::Type::Geometry: return GL_GEOMETRY_SHADER_BIT;
        case Shader::Type::Compute: return GL_COMPUTE_SHADER_BIT;
    }
}

} // namespace



Shader::Shader(Type type, const std::filesystem::path& path): type{type} {

    if (!std::filesystem::exists(path)) {
        throw std::runtime_error("Could not open file.");
    }

    std::ifstream file{path};

    std::string file_content(std::istreambuf_iterator(file), std::istreambuf_iterator<char>{});

    const char* raw_file = file_content.c_str();

    id = glCreateShaderProgramv(get_type_flag(type), 1, &raw_file);


    int success = 0;
    glGetProgramiv(id, GL_LINK_STATUS, &success);

    if (success == GL_FALSE) {

        GLint log_size = 0;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &log_size);

        std::vector<char> log_data;
        log_data.reserve(log_size);

        glGetProgramInfoLog(id, log_size, &log_size, &log_data[0]);
        std::string_view log(&log_data[0], log_size);

        std::cerr << "Could not compile shader: " << log << '\n';
        std::exit(EXIT_FAILURE);
    }
}


Shader::~Shader() {
    if (id != 0) {
        glDeleteProgram(id);
    }
}

Shader::Shader(Shader&& other): type{other.type}, id{std::exchange(other.id, 0)} {}

Shader& Shader::operator=(Shader&& other) {
    type = other.type;
    id = std::exchange(other.id, 0);

    return *this;
}


void Shader::set_uniform(std::string_view name, bool value) const {
    const auto location = glGetUniformLocation(id, name.data());
    glProgramUniform1i(id, location, GLint{value});
}

void Shader::set_uniform(std::string_view name, int value) const {
    const auto location = glGetUniformLocation(id, name.data());
    glProgramUniform1i(id, location, value);
}

void Shader::set_uniform(std::string_view name, unsigned int value) const {
    const auto location = glGetUniformLocation(id, name.data());
    glProgramUniform1ui(id, location, value);
}

void Shader::set_uniform(std::string_view name, float value) const {
    const auto location = glGetUniformLocation(id, name.data());
    glProgramUniform1f(id, location, value);
}

void Shader::set_uniform(std::string_view name, const glm::vec2& value) const {
    const auto location = glGetUniformLocation(id, name.data());
    glProgramUniform2fv(id, location, 1, glm::value_ptr(value));
}

void Shader::set_uniform(std::string_view name, const glm::vec3& value) const {
    const auto location = glGetUniformLocation(id, name.data());
    glProgramUniform3fv(id, location, 1, glm::value_ptr(value));
}

void Shader::set_uniform(std::string_view name, const glm::mat4& value) const {
    const auto location = glGetUniformLocation(id, name.data());
    glProgramUniformMatrix4fv(id, location, 1, GL_FALSE, glm::value_ptr(value));
}


// Pipeline


Pipeline::Pipeline() {
    glCreateProgramPipelines(1, &id);
}

Pipeline::~Pipeline() {
    if (id != 0) {
        glDeleteProgramPipelines(1, &id);
    }
}

Pipeline::Pipeline(Pipeline&& other): id{std::exchange(other.id, 0)} {}

Pipeline& Pipeline::operator=(Pipeline&& other) {
    id = std::exchange(other.id, 0);

    return *this;
}


void Pipeline::attach(const Shader& shader) const {
    glUseProgramStages(id, get_bit(shader.get_type()), shader.get_id());
}

void Pipeline::activate() const {
    glBindProgramPipeline(id);
}

void Pipeline::deactivate() const {
    glBindProgramPipeline(0);
}



} // namespace glu
