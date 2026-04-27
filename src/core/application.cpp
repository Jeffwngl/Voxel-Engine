#include "application.hpp"

// stb
// #define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// file loading
#include <iomanip>

/**
 * Standard light block definition
 */
constexpr float lightCubeVertices[] = {
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

void loadTextureLayer(const char* path, GLint layer) {
    int w, h, channels;
    unsigned char* data = stbi_load(path, &w, &h, &channels, 0);
    if (data) {
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0,
                        0, 0, layer,
                        Engine::TEXTURE_SIZE, Engine::TEXTURE_SIZE, 1,
                        GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    } else {
        std::cerr << "[Texture] Failed to load: " << path << '\n';
    }
}

namespace Engine {

/* Constructor / destructor */
Game::Game() : camera(glm::vec3(0.0f, 32.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)) {}

Game::~Game() {
    finish();
}

/* Entry point */

void Game::run() {
    if (!init()) return;
    loadTextures();
    setupLight();
    configureShaders();
    mainLoop();
}

bool Game::init() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return false;
    }

    // Configure OpenGL version 3.3 Core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Voxel Engine v0.0.1", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return false;
    }

    glfwSetWindowUserPointer(window, this); // Set user pointer before mouse events
    glfwMakeContextCurrent(window);  // Make context current
    glfwSetFramebufferSizeCallback(window, onFrameBufferResize);
    glfwSetCursorPosCallback(window, onMouseMove);
    // if (glfwRawMouseMotionSupported()) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); // Wslg doesnt support GLFW_CURSOR_DISABLED
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return false;
    }

    glEnable(GL_DEPTH_TEST);
    
    return true;
}

void Game::loadTextures() {
    glGenTextures(1, &textureArray);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);

    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, TEXTURE_SIZE, TEXTURE_SIZE, 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Access from build folder
    loadTextureLayer("../src/assets/textures/dirt.png", 0);
    loadTextureLayer("../src/assets/textures/grass-side.png", 1);
    loadTextureLayer("../src/assets/textures/grass-top.png", 2);
    loadTextureLayer("../src/assets/textures/grass-flowers.png", 3);

    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
}

void Game::configureShaders() {
    terrainShader = new Shader("../src/shaders/vertex.glsl", "../src/shaders/fragment.glsl");
    lightShader = new Shader("../src/shaders/light_vertex.glsl", "../src/shaders/light_fragment.glsl");

    projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT), 0.1f, 100.0f);

    terrainShader->useShader();
    terrainShader->setInt("textureIDs", 0);
    terrainShader->setVec3("light.position", lightPos);
    terrainShader->setVec3("light.color", lightColor);
    terrainShader->setFloat("material.ambient", ambientStrength);
    terrainShader->setFloat("material.diffuse", diffuseStrength);
    terrainShader->setFloat("material.specular", specularStrength);
    terrainShader->setFloat("material.shininess", shininess);
    terrainShader->setFloat("fog.fogStart", fogStart);
    terrainShader->setFloat("fog.fogEnd", fogEnd);
    terrainShader->setVec3("fog.fogColor", fogColor);
    // scattering testing
    // terrainShader->setVec3("betaRayleigh", glm::vec3(1.0e-3f, 2.0e-3f, 4.0e-3f));
    // terrainShader->setVec3("betaMie", glm::vec3(5e-4f, 5e-4f, 5e-4f));
    // terrainShader->setFloat("g", -0.75f);
    // terrainShader->setFloat("Esun", 15.0f);
terrainShader->setVec3("betaRayleigh", glm::vec3(1.16e-3f, 2.7e-3f, 6.62e-3f));
terrainShader->setVec3("betaMie",      glm::vec3(4e-4f, 4e-4f, 4e-4f));
terrainShader->setFloat("g",           -0.75f);
terrainShader->setFloat("Esun",        20.0f);
terrainShader->setVec3("sunDir",       glm::normalize(glm::vec3(0.5f, 1.0f, 0.3f)));

    GLint count;
    glGetProgramiv(terrainShader->shaderID, GL_ACTIVE_UNIFORMS, &count);
    std::cout << "Active uniforms: " << count << '\n';

    /* Debug */
    for (GLint i = 0; i < count; i++) {
        char name[64];
        GLsizei length;
        GLint size;
        GLenum type;
        glGetActiveUniform(terrainShader->shaderID, i, 64, &length, &size, &type, name);
        std::cout << "  Uniform " << i << ": " << name << '\n';
    }

    lightShader->useShader();
    lightShader->setVec3("lightColor", lightColor);
    lightShader->setMat4("projection", projection);
    lightShader->setVec3("lightPos", lightPos);
    std::cout << "Shaders loaded." << "\n";
}

void Game::setupLight() {
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &lightVBO);

    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(lightCubeVertices), lightCubeVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    // glBindVertexArray(0);
}

void Game::mainLoop() {
    std::cout << "Entering game loop" << "\n";
    while (!glfwWindowShouldClose(window)) {
        calculateFPS();
        update();
        render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void Game::finish() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Game::update() {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastTime;
    lastTime = currentFrame;

    camera.processInput(window, deltaTime);
}

void Game::render() {
    // Set background color and clear
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view = glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);

    /* Terrain */
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);

    terrainShader->useShader();
    terrainShader->setMat4("view", view); // vertices into world space
    terrainShader->setVec3("cameraPos", camera.Position);
    glm::mat4 terrainModel = glm::mat4(1.0f);
    terrainShader->setMat4("terrainModel", terrainModel);
    terrainShader->setMat4("projection", projection); // vertices into screen space

    /* Chunk generation */
    int playerChunk_x = static_cast<int>(std::floor(camera.Position.x / CHUNK_SIZE));
    int playerChunk_z = static_cast<int>(std::floor(camera.Position.z / CHUNK_SIZE));
    // std::cout << "Player chunk: " << playerChunk_x << ", " << playerChunk_z << '\n';
    chunkManager.update(playerChunk_x, playerChunk_z, RENDER_DISTANCE);
    chunkManager.uploadMesh();
    chunkManager.render();

    /* Light cube */
    lightShader->useShader();
    lightShader->setMat4("view", view);
    glm::mat4 lightModel = glm::mat4(1.0f); // can move outside
    lightModel = glm::translate(lightModel, lightPos);
    lightShader->setMat4("lightModel", lightModel);

    glBindVertexArray(lightVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // glBindVertexArray(0);

    /* Debug */
    // std::cout << "Camera pos: " 
    //           << camera.Position.x << ", " 
    //           << camera.Position.y << ", " 
    //           << camera.Position.z << '\n';
    // std::cout << "Camera front: "
    //           << camera.Front.x << ","
    //           << camera.Front.y << ","
    //           << camera.Front.z << '\n';
}

void Game::onFrameBufferResize(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void Game::calculateFPS() {
    double currentTime = glfwGetTime();
    frameCount++;

    if (currentTime - lastFrame >= 1.0) {
        fps = (float)frameCount / (currentTime - lastFrame);

        std::cout << std::fixed << std::setprecision(1) << fps << " FPS" << std::endl;

        frameCount = 0;
        lastFrame = currentTime;
    }
}

// TODO: Rewrite this
void Game::onMouseMove(GLFWwindow* window, double xPos, double yPos) {
    auto* app = static_cast<Game*>(glfwGetWindowUserPointer(window));
    app->camera.mouseCallBack(window, xPos, yPos);
}

}