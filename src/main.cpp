#include <array>
#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <memory>

#include <glad/gl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glm/glm.hpp>

#include "primitives.hpp"
#include "shader.hpp"
#include "texture.hpp"



static constexpr int OPENGL_VERSION_MAJOR = 4;
static constexpr int OPENGL_VERSION_MINOR = 6;

static constexpr std::size_t window_width = 1440;
static constexpr std::size_t window_height = 1080;
static constexpr std::string_view window_name = "Slime Simulation";

static constexpr int res_x = 400;
static constexpr int res_y = 400;


namespace detail {


struct GLFWwindowDeleter {
    void operator()(GLFWwindow* handle) { glfwDestroyWindow(handle); }
};


void glErrorCallback(
      unsigned int source, unsigned int type, unsigned int id, unsigned int severity, int /*length*/, const char* msg, const void*) {
    std::cerr << "OpenGL error no. " << id << " (source " << source << ", type " << type << ", severity " << severity << "): " << msg
              << '\n';
}


} // namespace detail

std::unique_ptr<GLFWwindow, detail::GLFWwindowDeleter> init_opengl() {

    glfwSetErrorCallback([](int code, const char* msg) { std::cerr << "GLFW error no. " << code << ": " << msg << '\n'; });
    glfwInit();


    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_VERSION_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);


    GLFWwindow* window = glfwCreateWindow(window_width, window_height, window_name.data(), nullptr, nullptr);

    if (window == nullptr) {
        throw std::runtime_error("Could not create window!");
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // vsync

    const auto gl_version = gladLoadGL(glfwGetProcAddress);

    if (gl_version == 0) {
        throw std::runtime_error("Could not load OpenGL functions!");
    }

    glDebugMessageCallback(&detail::glErrorCallback, nullptr);

    glViewport(0, 0, window_width, window_height);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");


    return std::unique_ptr<GLFWwindow, detail::GLFWwindowDeleter>(window);
}

void process_inputs(GLFWwindow& window) {
    if (glfwGetKey(&window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(&window, GLFW_TRUE);
    }
}


struct SimulationSettings {
    int iterations_per_seconds = 1;
};



int main() {
    using namespace glu;

    auto window = init_opengl();


    Shader vs{Shader::Type::Vertex, "shaders/shader.vert.glsl"};
    Shader fs{Shader::Type::Fragment, "shaders/shader.frag.glsl"};
    Shader cs{Shader::Type::Compute, "shaders/shader.comp.glsl"};

    Pipeline shader_pipeline;
    shader_pipeline.attach(vs);
    shader_pipeline.attach(fs);

    Pipeline compute_pipeline;
    compute_pipeline.attach(cs);

    Texture tex({res_x, res_y});
    Quad quad;

    SimulationSettings settings;


    using clock_t = std::chrono::high_resolution_clock;

    auto frame_begin = clock_t::now();

    while (glfwWindowShouldClose(window.get()) == 0) {

        const auto frame_end = clock_t::now();

        [[maybe_unused]] const auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(frame_end - frame_begin);
        frame_begin = frame_end;


        glfwPollEvents();
        process_inputs(*window);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Imgui
        {
            ImGui::Begin("Slime!!!");

            ImGui::InputInt("Iterations per second", &settings.iterations_per_seconds);

            ImGui::End();
        }

        tex.bind_to_image_unit(0);
        compute_pipeline.activate();
        glDispatchCompute(res_x, res_y, 1);
        compute_pipeline.deactivate();

        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        glClear(GL_COLOR_BUFFER_BIT);
        glViewport(0, 0, window_width, window_height);

        tex.bind_to_texture_unit(0);
        quad.draw(shader_pipeline);


        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        glfwSwapBuffers(window.get());
    }


    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();


    return EXIT_SUCCESS;
}
