# Voxel-Engine

> A 3D voxel world engine built in C++ with OpenGL — v0.0.1 Alpha

This is a project I have been using to learn more about OpenGL and C++ as well as graphics programming.

![C++](https://img.shields.io/badge/C++-17-blue?style=flat-square)
![OpenGL](https://img.shields.io/badge/OpenGL-3.3-green?style=flat-square)
![Platform](https://img.shields.io/badge/platform-macOS-lightgrey?style=flat-square)
![Status](https://img.shields.io/badge/status-alpha-orange?style=flat-square)

## Features

- Procedural terrain generation using 3D Perlin noise with a height gradient
- Chunk-based world — terrain is generated, uploaded, and culled dynamically based on player position
- Face culling — only visible block faces are added to the mesh
- Texture array support — dirt, grass sides, grass top, and flower textures
- Phong lighting model — ambient, diffuse, and specular lighting
- First-person camera with mouse look and keyboard movement
- Render distance configuration

## Project Structure
```
VoxelEngine/
├── src/
│   ├── core/           # Application class — window, render loop, lifecycle
│   ├── input/          # Camera — movement and mouse look
│   ├── world/          # Chunk manager — generation, upload, culling
│   ├── noise/          # Perlin noise terrain generation
│   ├── render/         # Shader loading and uniform helpers
│   ├── shaders/        # GLSL vertex and fragment shaders
│   └── assets/         # Textures
├── include/            # Third-party headers (GLFW, GLM, GLAD, stb)
└── CMakeLists.txt
```

## Dependencies
 
| Library | Version | Purpose |
|---|---|---|
| [GLFW](https://www.glfw.org/) | 3.x | Window creation and input |
| [GLAD](https://glad.dav1d.de/) | — | OpenGL function loader |
| [GLM](https://github.com/g-truc/glm) | 0.9.x | Math (vectors, matrices) |
| [stb_image](https://github.com/nothings/stb) | — | Texture loading |
| [stb_perlin](https://github.com/nothings/stb) | — | Perlin noise generation |

## Installation
Requires CMake 3.16+ and a C++17 compiler (gcc on macOS).

```bash
git clone https://github.com/Jeffwngl/voxel-engine.git
cd Voxel-Engine
mkdir build && cd build
cmake ..
cmake --build .
./Voxel-Engine
```

### For Windows Users

To enable support for `GLFW_CURSOR_DISABLED` which does not work on the WSLg compatibility layer, you need to compile and run the program natively on windows as an `.exe`, you can use any C++ windows toolchain e.g. Install MSYS2:

```
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja
```
- Then open the `MSYS2 MINGW64` terminal and build the project as normal:
```
mkdir build && cd build
cmake .. -G "Ninja"
ninja

./Voxel-Engine.exe
```

For those who wish to still run this via WSL, you can install the dependencies using `ubuntu-deps.sh`.

## Controls

- `WASD` Move around.
- `Q` Toggle on and off cursor control.
- `SPACE` Move up.
- `Shift` Move down.

## Screenshots

![picture1](/public/picture1.png)
![picture1](/public/picture1.png)

### Development

![perlin](/public/Perlin.png?raw=true)
![lighting 1](public/diffuse.png?raw=true)
![lighting 1](public/Lighting.png?raw=true)
