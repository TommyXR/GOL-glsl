#include <array>
#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <memory>
#include <random>
#include <utility>

#include <glad/gl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glm/glm.hpp>

#include "glu.hpp"
#include "simulation.hpp"



static constexpr int OPENGL_VERSION_MAJOR = 4;
static constexpr int OPENGL_VERSION_MINOR = 6;

static constexpr std::size_t window_width = 1440;
static constexpr std::size_t window_height = 1440;
static constexpr std::string_view window_name = "Slime Simulation";

static constexpr Texture::Resolution res{2048, 2048};


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

    // glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);


    GLFWwindow* window = glfwCreateWindow(window_width, window_height, window_name.data(), nullptr, nullptr);

    if (window == nullptr) {
        throw std::runtime_error("Could not create window!");
    }

    glfwMakeContextCurrent(window);
    // glfwSwapInterval(1); // vsync

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



void randomize(Texture& tex, float density) {
    std::mt19937 rng{std::random_device{}()};

    std::uniform_real_distribution<float> dist(0, 1);

    std::array<std::uint8_t, res.x * res.y> data;

    for (auto& n: data) {
        n = dist(rng) < density ? 1 : 0;
    }

    tex.set_image<std::uint8_t>(data, res, Texture::InternalFormat::R8ui);
}



int main() {
    using namespace glu;
    using namespace std::chrono_literals;

    auto window = init_opengl();


    Shader vs{Shader::Type::Vertex, "shaders/shader.vert.glsl"};
    Shader fs{Shader::Type::Fragment, "shaders/shader.frag.glsl"};
    Shader cs{Shader::Type::Compute, "shaders/shader.comp.glsl"};

    Pipeline render_pipeline;
    render_pipeline.attach(vs);
    render_pipeline.attach(fs);

    Pipeline compute_pipeline;
    compute_pipeline.attach(cs);

    Texture input_texture(res, Texture::InternalFormat::R8ui);
    Texture output_texture(res, Texture::InternalFormat::R8ui);


    Quad quad;
    Simulation::Parameters settings;


    randomize(input_texture, settings.randomize_density);


    using clock_t = std::chrono::high_resolution_clock;

    auto elapsed = 0ns;
    int iteration = 0;

    auto frame_begin = clock_t::now();

    while (glfwWindowShouldClose(window.get()) == 0) {

        const auto frame_end = clock_t::now();

        [[maybe_unused]] const auto dt = std::chrono::duration_cast<std::chrono::nanoseconds>(frame_end - frame_begin);
        frame_begin = frame_end;

        elapsed += dt;

        glfwPollEvents();
        process_inputs(*window);


        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Imgui
        ImGui::Begin("Slime!!!");
        {
            ImGui::Text("FPS: %f", ImGui::GetIO().Framerate);

            ImGui::SliderInt("Iterations per second", &settings.iterations_per_second, 1, 500);

            ImGui::Separator();

            ImGui::SliderFloat("Randomize density", &settings.randomize_density, 0, 1);

            if (ImGui::Button("Randomize!")) {
                randomize(input_texture, settings.randomize_density);
            }
        }
        ImGui::End();



        glClear(GL_COLOR_BUFFER_BIT);

        // RENDERING!!
        //
        //
        //
        //
        //


        // Draw the texture
        input_texture.bind_to_image_unit(0, Texture::AccessType::Read);
        quad.draw(render_pipeline);


        // Update cells

        if (elapsed.count() >= 1.0f / settings.iterations_per_second) {
            while (elapsed.count() >= 1.0f / settings.iterations_per_second) {

                ++iteration;
                fs.set_uniform("iteration", iteration);

                elapsed -= 1000ms / settings.iterations_per_second;

                input_texture.bind_to_image_unit(0, Texture::AccessType::Read);
                output_texture.bind_to_image_unit(1, Texture::AccessType::Write);


                compute_pipeline.activate();
                glDispatchCompute(res.x, res.y, 1);
                compute_pipeline.deactivate();

                // Swap input and output textures...
                std::swap(input_texture, output_texture);
                //
                //
                //
                //
                //
                //
                //
                //
                // /RENDERING!!
            }
        }


        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        // glFlush();
        glfwSwapBuffers(window.get());
    }


    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();


    return EXIT_SUCCESS;
}
