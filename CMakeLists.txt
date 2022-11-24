cmake_minimum_required(VERSION 3.23)
project(UCScraft)

set(CMAKE_CXX_STANDARD 20)

include_directories(include include/glm)

add_executable(UCScraft
        include/camera.h
        main.cpp
        include/shader_m.h
        include/stb_image.cpp
        include/stb_image.h
        )

target_link_libraries(UCScraft -lglfw -lGLEW -lGL)
