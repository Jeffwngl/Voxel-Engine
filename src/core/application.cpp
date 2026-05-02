#include "application.hpp"

#include <stb_image.h>
#include <iomanip> // file loading

// imgui
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

void loadTextureLayer(const char* path, GLint layer) {
    int w, h, channels;
    unsigned char* data = stbi_load(path, &w, &h, &channels, 0);
    if (data) {
        glTexSubImage3D(
            GL_TEXTURE_2D_ARRAY, 
            0,
            0, 
            0, 
            layer,
            Engine::TEXTURE_SIZE, 
            Engine::TEXTURE_SIZE, 
            1,
            GL_RGBA, 
            GL_UNSIGNED_BYTE, 
            data
        );
        stbi_image_free(data);
    } else {
        std::cerr << "[Texture] Failed to load: " << path << '\n';
    }
}

namespace Engine {

// Constructor
Game::Game() : camera(glm::vec3(0.0f, 32.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)) {}

// Destructor
Game::~Game() {
    finish();
}

/* Entry point */

void Game::run() {
    if (!init()) return;
    loadTextures();
    setupSkyBox();
    setupDepthMap();
    configureShaders();
    configureImgui();
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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return false;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    
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

void Game::configureShaders() { // TODO: Move this to another file
    terrainShader = new Shader("../src/shaders/vertex.glsl", "../src/shaders/fragment.glsl");
    skyBoxShader = new Shader("../src/shaders/skybox_vertex.glsl", "../src/shaders/skybox_fragment.glsl");
    depthShader = new Shader("../src/shaders/depth_vertex.glsl", "../src/shaders/depth_fragment.glsl");

    float farPlane = (renderDistance + 1) * CHUNK_SIZE * 2.0f;
    projection = glm::mat4(1.0f);
    projection = glm::perspective(
        glm::radians(45.0f), 
        static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT), 
        0.1f, 
        farPlane
    );

    fogEnd = farPlane * 0.6f;
    fogStart = farPlane * 0.2f;

    terrainShader->useShader();
    terrainShader->setInt("textureIDs", 0);
    terrainShader->setInt("shadowMap", 1);
    terrainShader->setVec3("light.position", sunDir);
    terrainShader->setVec3("light.color", lightColor);
    terrainShader->setFloat("material.ambient", ambientStrength);
    terrainShader->setFloat("material.diffuse", diffuseStrength);
    terrainShader->setFloat("material.specular", specularStrength);
    terrainShader->setFloat("material.shininess", shininess);
    terrainShader->setFloat("fog.fogStart", fogStart);
    terrainShader->setFloat("fog.fogEnd", fogEnd);
    terrainShader->setVec3("fog.fogColor", fogColor);

    // scattering testing
    terrainShader->setVec3("betaRayleigh", glm::vec3(1.0e-3f, 2.0e-3f, 4.0e-3f)); // beta sc Air coefficients (RGB)
    terrainShader->setVec3("betaMie", glm::vec3(5e-4f, 5e-4f, 5e-4f)); // beta sc Haze coefficients (RGB)
    terrainShader->setFloat("g", -0.75f);
    terrainShader->setFloat("Esun", 15.0f);

    // intense scattering
    // terrainShader->setVec3("betaRayleigh", glm::vec3(1.16e-3f, 2.7e-3f, 6.62e-3f));
    // terrainShader->setVec3("betaMie", glm::vec3(4e-4f, 4e-4f, 4e-4f));
    // terrainShader->setFloat("g", -0.75f);
    // terrainShader->setFloat("Esun", 20.0f);

    skyBoxShader->useShader();
    skyBoxShader->setMat4("projection", projection);
    skyBoxShader->setVec3("sunDir", sunDir);
    skyBoxShader->setVec3("betaRayleigh", glm::vec3(1.16e-3f, 2.7e-3f, 6.62e-3f));
    skyBoxShader->setVec3("betaMie", glm::vec3(4e-4f, 4e-4f, 4e-4f));
    skyBoxShader->setFloat("g", -0.75f);
    skyBoxShader->setFloat("Esun", 20.0f);

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

    std::cout << "Shaders loaded." << "\n";
}
   

void Game::configureImgui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;      // IF using Docking Branch

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();
}

void Game::setupSkyBox() {
    skyBox = new SkyBox();
}

void Game::setupDepthMap() {
    depthMap = new DepthMap();
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
    delete skyBox;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Game::update() { // TODO: Move to separate files
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastTime;
    lastTime = currentFrame;

    // toggle polygon mode on and off
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && !mPressed) {
        mPressed = true;
        wireframe = !wireframe;
    }

    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_RELEASE) {
        mPressed = false;
    }

    // toggle mouse cursor on and off
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && !tabPressed) {
        tabPressed = true;
        cursorEnabled = !cursorEnabled;
        if (cursorEnabled) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            firstLoad = true; // prevent camera snap when re-entering
        }
    }

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_RELEASE) {
        tabPressed = false;
    }

    if (!cursorEnabled) {
        camera.processInput(window, deltaTime);
    }
}

void Game::render() {
    /* Chunk Generation */
    int playerChunk_x = static_cast<int>(std::floor(camera.Position.x / CHUNK_SIZE));
    int playerChunk_z = static_cast<int>(std::floor(camera.Position.z / CHUNK_SIZE));
    chunkManager.update(playerChunk_x, playerChunk_z, renderDistance);
    chunkManager.uploadMesh(); // put this at top so depth map can use it

    /* Render scene to depth map */
    // pass 1
    glm::mat4 lightSpaceMatrix = depthMap->getLightSpaceMatrix(sunDir, camera.Position);
    depthMap->bindForWriting();
    depthShader->useShader();
    depthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
    depthShader->setMat4("terrainModel", glm::mat4(1.0f));
    glCullFace(GL_FRONT); // render back faces to depth map
    chunkManager.render();
    glCullFace(GL_BACK); // restore for normal rendering
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // return to default framebuffer
    // pass 2
    int fbWidth, fbHeight; // macOS retina displays framebuffer is double windows, safer to get framebuffer size for both
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view = glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);

    /* Terrain */
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);
    depthMap->bindForReading(1);
    terrainShader->useShader();
    terrainShader->setMat4("view", view); // vertices into world space
    terrainShader->setVec3("cameraPos", camera.Position);
    glm::mat4 terrainModel = glm::mat4(1.0f);
    terrainShader->setMat4("terrainModel", terrainModel);
    terrainShader->setMat4("projection", projection); // vertices into screen space
    terrainShader->setVec3("light.position", sunDir);
    terrainShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
    terrainShader->setFloat("fog.fogStart", fogStart);
    terrainShader->setFloat("fog.fogEnd", fogEnd); 

    if (wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // render chunk
    chunkManager.render();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    /* Skybox - draw last */
    glDepthFunc(GL_LEQUAL);
    skyBoxShader->useShader();
    skyBoxShader->setMat4("projection", projection);
    skyBoxShader->setMat4("view", view); // use the same view you computed above
    skyBoxShader->setVec3("sunDir", sunDir);
    skyBox->draw();
    glDepthFunc(GL_LESS);

    /* Imgui */
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Debug");
    ImGui::Text("FPS: %.1f", fps);
    ImGui::Separator();
    ImGui::Text("Camera");
    ImGui::Text("Position: %.1f, %.1f, %.1f", camera.Position.x, camera.Position.y, camera.Position.z);
    ImGui::Text("Front: %.1f, %.1f, %.1f", camera.Front.x, camera.Front.y, camera.Front.z);
    ImGui::Separator();
    ImGui::Text("Sun");
    ImGui::SliderFloat3("Direction", &sunDir.x, -1.0f, 1.0f);
    ImGui::Separator();
    ImGui::Text("World");
    int prevRenderDistance = renderDistance;
    ImGui::SliderInt("Render Distance", &renderDistance, 1, 16);
    if (renderDistance != prevRenderDistance) {
        chunkManager.clear();
        int fbWidth, fbHeight;
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
        float farPlane = (renderDistance + 1) * CHUNK_SIZE * 2.0f;
        projection = glm::perspective(glm::radians(45.0f),
            static_cast<float>(fbWidth) / static_cast<float>(fbHeight),
            0.1f, farPlane);
        fogEnd = farPlane * 0.6f;
        fogStart = farPlane * 0.2f;
        // send updated fog to shader
        terrainShader->useShader();
        terrainShader->setFloat("fog.fogStart", fogStart);
        terrainShader->setFloat("fog.fogEnd", fogEnd);
    }
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
    if (app->cursorEnabled) return;
    app->camera.mouseCallBack(window, xPos, yPos);
}
}