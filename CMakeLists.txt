cmake_minimum_required(VERSION 3.23)
project(UCScraft)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_executable(UCScraft
        main.cpp
        include/camera.hpp
        include/Shader.hpp
        include/shader_m.hpp
        include/stb_image.cpp
        include/stb_image.h
        include/Block.hpp)

target_link_libraries(UCScraft -lglfw -lGLEW -lGL)
