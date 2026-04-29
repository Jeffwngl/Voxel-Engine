/**
 * Handles main application rendering using glfw.
 */

#include <iostream>
#include <stdio.h>
#include <unordered_set>
#include <glad/glad.h>
#include <glfw/include/GLFW/glfw3.h>

// math libraries
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// file loading
#include <fstream>
#include <sstream>
#include <string>

// shader helpers
#include "../render/shader.h"

// chunk generation
#include "../world/chunk_gen.hpp"

// camera helpers
#include "../input/camera.hpp"

#include "../render/skybox.hpp" // TODO: maybe move this later

/**
 * Standard light block definition
 */
constexpr float lightCubeVertices[] = { // TODO: move somewhere else
    // pos (xyz)            normal (xyz)         uv
    // Back
    -0.5f,-0.5f,-0.5f,   0.0f, 0.0f,-1.0f,   0.0f,0.0f,
     0.5f,-0.5f,-0.5f,   0.0f, 0.0f,-1.0f,   1.0f,0.0f,
     0.5f, 0.5f,-0.5f,   0.0f, 0.0f,-1.0f,   1.0f,1.0f,
     0.5f, 0.5f,-0.5f,   0.0f, 0.0f,-1.0f,   1.0f,1.0f,
    -0.5f, 0.5f,-0.5f,   0.0f, 0.0f,-1.0f,   0.0f,1.0f,
    -0.5f,-0.5f,-0.5f,   0.0f, 0.0f,-1.0f,   0.0f,0.0f,
    // Front
    -0.5f,-0.5f, 0.5f,   0.0f, 0.0f, 1.0f,   0.0f,0.0f,
     0.5f,-0.5f, 0.5f,   0.0f, 0.0f, 1.0f,   1.0f,0.0f,
     0.5f, 0.5f, 0.5f,   0.0f, 0.0f, 1.0f,   1.0f,1.0f,
     0.5f, 0.5f, 0.5f,   0.0f, 0.0f, 1.0f,   1.0f,1.0f,
    -0.5f, 0.5f, 0.5f,   0.0f, 0.0f, 1.0f,   0.0f,1.0f,
    -0.5f,-0.5f, 0.5f,   0.0f, 0.0f, 1.0f,   0.0f,0.0f,
    // Left
    -0.5f, 0.5f, 0.5f,  -1.0f, 0.0f, 0.0f,   1.0f,0.0f,
    -0.5f, 0.5f,-0.5f,  -1.0f, 0.0f, 0.0f,   1.0f,1.0f,
    -0.5f,-0.5f,-0.5f,  -1.0f, 0.0f, 0.0f,   0.0f,1.0f,
    -0.5f,-0.5f,-0.5f,  -1.0f, 0.0f, 0.0f,   0.0f,1.0f,
    -0.5f,-0.5f, 0.5f,  -1.0f, 0.0f, 0.0f,   0.0f,0.0f,
    -0.5f, 0.5f, 0.5f,  -1.0f, 0.0f, 0.0f,   1.0f,0.0f,
    // Right
     0.5f, 0.5f, 0.5f,   1.0f, 0.0f, 0.0f,   1.0f,0.0f,
     0.5f, 0.5f,-0.5f,   1.0f, 0.0f, 0.0f,   1.0f,1.0f,
     0.5f,-0.5f,-0.5f,   1.0f, 0.0f, 0.0f,   0.0f,1.0f,
     0.5f,-0.5f,-0.5f,   1.0f, 0.0f, 0.0f,   0.0f,1.0f,
     0.5f,-0.5f, 0.5f,   1.0f, 0.0f, 0.0f,   0.0f,0.0f,
     0.5f, 0.5f, 0.5f,   1.0f, 0.0f, 0.0f,   1.0f,0.0f,
    // Bottom
    -0.5f,-0.5f,-0.5f,   0.0f,-1.0f, 0.0f,   0.0f,1.0f,
     0.5f,-0.5f,-0.5f,   0.0f,-1.0f, 0.0f,   1.0f,1.0f,
     0.5f,-0.5f, 0.5f,   0.0f,-1.0f, 0.0f,   1.0f,0.0f,
     0.5f,-0.5f, 0.5f,   0.0f,-1.0f, 0.0f,   1.0f,0.0f,
    -0.5f,-0.5f, 0.5f,   0.0f,-1.0f, 0.0f,   0.0f,0.0f,
    -0.5f,-0.5f,-0.5f,   0.0f,-1.0f, 0.0f,   0.0f,1.0f,
    // Top
    -0.5f, 0.5f,-0.5f,   0.0f, 1.0f, 0.0f,   0.0f,1.0f,
     0.5f, 0.5f,-0.5f,   0.0f, 1.0f, 0.0f,   1.0f,1.0f,
     0.5f, 0.5f, 0.5f,   0.0f, 1.0f, 0.0f,   1.0f,0.0f,
     0.5f, 0.5f, 0.5f,   0.0f, 1.0f, 0.0f,   1.0f,0.0f,
    -0.5f, 0.5f, 0.5f,   0.0f, 1.0f, 0.0f,   0.0f,0.0f,
    -0.5f, 0.5f,-0.5f,   0.0f, 1.0f, 0.0f,   0.0f,1.0f,
};

namespace Engine {

inline constexpr unsigned int SCREEN_WIDTH = 800;
inline constexpr unsigned int SCREEN_HEIGHT = 600;
inline constexpr int TEXTURE_SIZE = 128;
inline constexpr int CHUNK_SIZE = 16;
inline constexpr int RENDER_DISTANCE = 5;

class Game {
    private:
        /* Frame timing */
        float deltaTime = 0.0f;
        float lastTime = 0.0f;
        float lastFrame = 0.0f;
        float fps = 0.0f;
        float frameCount = 0.0f;

        Camera camera;

        float yaw;
        float pitch;
        float lastMouseX;
        float lastMouseY;

        bool firstLoad;

        /* World */
        ChunkManager chunkManager;

        /* Loop functions */
        bool init();
        void loadTextures();
        void configureShaders();
        void setupLight();
        void setupSkyBox();
        void mainLoop();
        void finish();

        void update();
        void render();
        void calculateFPS();
        
        static void onFrameBufferResize(GLFWwindow* window, int width, int height);
        static void onMouseMove(GLFWwindow *window, double xPos, double yPos);

        Shader* terrainShader;
        Shader* lightShader;
        Shader* skyBoxShader;

        SkyBox* skyBox;

        unsigned int textureArray = 0;

        /* Light geometry */
        unsigned int lightVAO = 0;
        unsigned int lightVBO = 0;

        /* Light parameters */
        float ambientStrength = 0.5f;
        float diffuseStrength = 1.0f;
        float specularStrength = 0.6f;
        float shininess = 32.0f;
        // glm::vec3 lightPos { 64.0f, 64.0f, 64.0f };
        glm::vec3 sunDir { glm::normalize(glm::vec3(0.5f, 1.0f, 0.3f)) };
        glm::vec3 lightColor { 1.0f, 1.0f, 1.0f };

        glm::mat4 projection { 1.0f };

        /* Fog parameters */
        float fogStart = 70.0f;
        float fogEnd = 100.0f;
        glm::vec3 fogColor { 1.0f, 1.0f, 1.0f };

    public:
        GLFWwindow* window;

        Game();
        ~Game();

        void run();
};
}