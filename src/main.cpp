#include <iostream>
#include <stdio.h>
#include <glad/glad.h>
#include <glfw/include/GLFW/glfw3.h>

// loading texture images
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// math libraries
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// file loading
#include <fstream>
#include <sstream>
#include <string>

// shader helpers
#include "../common/shader.h"

// perlin terrain
#include "../external/perlin_gen.h"

// camera helpers
#include "../common/camera.h"


void processInput(GLFWwindow *window);
void mouseCallBack(GLFWwindow *window, double xPos, double yPos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void calculateFPS();


const unsigned int SCREEN_HEIGHT = 600;
const unsigned int SCREEN_WIDTH = 800;

const int TEXTURE_SIZE = 128;

// TESTING GEOMETRY / LIGHT CUBE DEFINITION

float vertices[] = {
    // Back face
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

    // Front face
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

    // Left face
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    // Right face
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    // Bottom face
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

    // Top face
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
};

// delta time
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// camera setup
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

Camera camera(cameraPos, cameraUp);

// mouse camera
float yaw = -90.0f;
float pitch = 0.0f;
float lastMouseX = static_cast<float>(SCREEN_WIDTH) / 2.0f;
float lastMouseY = static_cast<float>(SCREEN_HEIGHT) / 2.0f;

bool firstLoad = true;

// lighting
glm::vec3 lightPos(16.0f, 30.0f, 16.0f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

// material properties
float diffuseStrength = 1.0f;
float ambientStrength = 0.3f;
float specularStrength = 0.6f;
float shineness = 32.0f;

// fps
float fps = 0.0f;
double lastTime = glfwGetTime();
int frameCount = 0;


int main() {
    
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    // Configure OpenGL version 3.3 Core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on macOS
#endif

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Voxel Engine v0.0.1", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);  // Make context current
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouseCallBack);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }


    // create shader class
    Shader shaderProgram("../shaders/vertex.glsl", "../shaders/fragment.glsl");
    Shader lightProgram("../shaders/light_vertex.glsl", "../shaders/light_fragment.glsl");

    // perlin stuff
    std::cout << "Starting up..." << '\n';
    std::vector<Vertex> perlinTerrain = PerlinGen::generate(0.05f);

    // vertices and shaders
    unsigned int VBO, VAO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO); // sets VBO to point to location of the newly created buffer

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // any changes to the buffer is applied to the buffer VBO points to

    // PERLIN
    glBufferData(GL_ARRAY_BUFFER, perlinTerrain.size() * sizeof(Vertex), perlinTerrain.data(), GL_STATIC_DRAW); // stores data in buffer
    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                        sizeof(Vertex), // this is the exact same as the diagram showed in the Shaders section in the notes
                        (void*)0); // tells openGL how to parse or analyse the buffer data, what part of the buffer data is what
    glEnableVertexAttribArray(0); // start sending data from currently bound VBO to variable in location 0 in vertex shader
    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    // Tex coords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex));
    glEnableVertexAttribArray(2);
    // Tex IDs
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texID));
    glEnableVertexAttribArray(3);


    // LIGHT SOURCE
    unsigned int lightVAO, lightVBO;

    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &lightVBO);

    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);
    
    // ----------------- moving to texture array ------------------- //

    unsigned int textureArray;
    glGenTextures(1, &textureArray);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);

    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, TEXTURE_SIZE, TEXTURE_SIZE, 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, numChannels;
    unsigned char* data;

    data = stbi_load("../assets/textures/dirt1.png", &width, &height, &numChannels, 0);
    if (data) {
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, TEXTURE_SIZE, TEXTURE_SIZE, 1, GL_RGBA, GL_UNSIGNED_BYTE, data); // layer index (5th item) determines where the tex goes
        stbi_image_free(data);
    }
    else {
        std::cout << "No file in: " << data << std::endl;
    }

    data = stbi_load("../assets/textures/grass-side3.png", &width, &height, &numChannels, 0);
    if (data) {
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 1, TEXTURE_SIZE, TEXTURE_SIZE, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    else {
        std::cout << "No file in: " << data << std::endl;
    }

    data = stbi_load("../assets/textures/grass-top.png", &width, &height, &numChannels, 0);
    if (data) {
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 2, TEXTURE_SIZE, TEXTURE_SIZE, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    else {
        std::cout << "No file in: " << data << std::endl;
    }

    data = stbi_load("../assets/textures/flowers.png", &width, &height, &numChannels, 0);
    if (data) {
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 3, TEXTURE_SIZE, TEXTURE_SIZE, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    else {
        std::cout << "No file in: " << data << std::endl;
    }

    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

    // set once for material shader uniforms that do not change
    shaderProgram.useShader();
    shaderProgram.setInt("textureIDs", 0);
    shaderProgram.setVec3("light.position", lightPos);
    shaderProgram.setVec3("light.color", lightColor); 

    shaderProgram.setFloat("material.ambient", ambientStrength);
    shaderProgram.setFloat("material.diffuse", diffuseStrength);
    shaderProgram.setFloat("material.specular", specularStrength);
    shaderProgram.setFloat("material.shineness", shineness);

    // set once for light shader uniforms that do not change
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT), 0.1f, 100.0f);
    shaderProgram.setMat4("projection", projection);

    lightProgram.useShader();
    lightProgram.setVec3("lightColor", lightColor);
    lightProgram.setMat4("projection", projection);
    lightProgram.setVec3("lightPos", lightPos);

    glEnable(GL_DEPTH_TEST);


    // Rendering loop
    while (!glfwWindowShouldClose(window)) {

        calculateFPS();

        // delta time
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Check for ESC key press
        camera.processInput(window, deltaTime);

        // Set background color and clear
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // -- 1. terrain --
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);

        // -- 1. main shader -- //
        shaderProgram.useShader();
        
        // light
        // shaderProgram.setVec3("light.position", lightPos);
        // shaderProgram.setVec3("light.color", lightColor); 
        // glm::vec3 diffuseColor = lightColor * diffuseStrength;
        // glm::vec3 ambientColor = diffuseColor * ambientStrength;
        // shaderProgram.setFloat("light.ambient", ambientStrength);
        // shaderProgram.setFloat("light.diffuse", diffuseStrength);
        // shaderProgram.setFloat("light.specular", specularStrength);

        // material
        // shaderProgram.setFloat("material.ambient", ambientStrength);
        // shaderProgram.setFloat("material.diffuse", diffuseStrength);
        // shaderProgram.setFloat("material.specular", specularStrength);
        // shaderProgram.setFloat("material.shineness", shineness);

        // keep working here >>>>
        glm::mat4 view = glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);
        shaderProgram.setMat4("view", view);

        shaderProgram.setVec3("cameraPos", camera.Position);

        glm::mat4 terrainModel = glm::mat4(1.0f);
        shaderProgram.setMat4("terrainModel", terrainModel);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, perlinTerrain.size());

        // -- 2. light cube -- //
        lightProgram.useShader();

        // view projection transformation
        lightProgram.setMat4("view", view);
        
        // world transformation
        glm::mat4 lightModel = glm::mat4(1.0f); // can move outide
        lightModel = glm::translate(lightModel, lightPos);
        lightProgram.setMat4("lightModel", lightModel);

        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}


// Functions

void mouseCallBack(GLFWwindow *window, double xPos, double yPos) {
    camera.mouseCallBack(window, xPos, yPos);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void calculateFPS() {
    double currentTime = glfwGetTime();
    frameCount++;

    if (currentTime - lastTime >= 1.0) {
        fps = (float)frameCount / (currentTime - lastTime);

        std::cout << std::fixed << std::setprecision(1) << fps << " FPS" << std::endl;

        frameCount = 0;
        lastTime = currentTime;
    }
}
