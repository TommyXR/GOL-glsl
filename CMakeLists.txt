cmake_minimum_required(VERSION 3.21)

project(Slime LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_subdirectory("third_party")

add_executable(slime)

target_sources(slime
    PUBLIC  include/shader.hpp
            include/texture.hpp
            include/primitives.hpp
            include/glu.hpp
            include/simulation.hpp
    
            src/main.cpp
            src/shader.cpp
            src/texture.cpp
            src/primitives.cpp
)

            
target_compile_options(slime PRIVATE -Wall -Wextra -Wpedantic)

target_include_directories(slime PUBLIC include/)
target_link_libraries(slime OpenGL::GL glfw glad imgui)

