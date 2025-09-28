cmake_minimum_required(VERSION 3.0.0)
project(Voxel-Engine VERSION 1.0.0)
cmake_policy(SET CMP0072 NEW)


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# set files to be compiled and linked
add_executable("${CMAKE_PROJECT_NAME}" src/config.h src/main.cpp src/glad.c)

# add glfw dependencies
target_include_directories("${CMAKE_PROJECT_NAME}" PRIVATE dependencies)

# link to glfw libraries
target_link_libraries("${CMAKE_PROJECT_NAME}" ${GLFW3_LIBRARY} OpenGL::GL)