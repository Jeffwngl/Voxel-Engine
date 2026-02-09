cmake_minimum_required(VERSION 3.14)
project(Voxel-Engine)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(include/glad)
include_directories(include/glfw)
include_directories(include)