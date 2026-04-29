cmake_minimum_required(VERSION 3.10)

project(Voxel-Engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

find_package(OpenGL REQUIRED) # find openGL

add_subdirectory(include/glfw) # build glfw

add_library(glad STATIC src/external/glad.c) # build glad

set(source_dir "${PROJECT_SOURCE_DIR}/src/")

add_executable(${CMAKE_PROJECT_NAME} src/main.cpp)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    src/core/application.cpp
    src/world/chunk_gen.cpp
    src/noise/perlin_gen.cpp
    src/input/camera.cpp
    src/external/stb_image.cpp
    src/render/skybox.cpp
    src/external/imgui/imgui_draw.cpp
    src/external/imgui/imgui_impl_glfw.cpp 
    src/external/imgui/imgui_impl_opengl3.cpp
    src/external/imgui/imgui_tables.cpp
    src/external/imgui/imgui_widgets.cpp
    src/external/imgui/imgui.cpp
)

target_include_directories("${CMAKE_PROJECT_NAME}" PUBLIC 
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/include/glfw/include"
    "${CMAKE_SOURCE_DIR}/src"
    "${CMAKE_SOURCE_DIR}/src/external"
)

target_link_libraries(${PROJECT_NAME} glfw glad OpenGL::GL)

if(APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa -framework IOKit -framework CoreVideo")
endif()

if(WIN32)
    target_link_libraries(${PROJECT_NAME} opengl32)
endif()