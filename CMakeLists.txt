cmake_minimum_required(VERSION 3.23)
project(UCScraft)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_executable(UCScraft
        include/camera.h
        include/Shader.h
        include/shader_m.h
        include/stb_image.cpp
        include/stb_image.h
        main.cpp
        )

target_link_libraries(UCScraft -lglfw -lGLEW -lGL)
