cmake_minimum_required(VERSION 3.10)

project(Voxel-Engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

find_package(OpenGL REQUIRED) # find openGL

add_subdirectory(include/glfw) # build glfw
# find_package(glfw REQUIRED) # use this if glfw is installed on your system

add_library(glad STATIC src/glad.c) # build glad

set(source_dir "${PROJECT_SOURCE_DIR}/src/")

add_executable("${CMAKE_PROJECT_NAME}" src/main.cpp)

# include_directories("${CMAKE_SOURCE_DIR}/include")
target_include_directories("${CMAKE_PROJECT_NAME}" PUBLIC "${CMAKE_SOURCE_DIR}/include") # specifies include folder

target_link_libraries(${PROJECT_NAME} glfw glad OpenGL::GL)

if(APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa -framework IOKit -framework CoreVideo")
endif()

if(WIN32)
    target_link_libraries(${PROJECT_NAME} opengl32)
endif()