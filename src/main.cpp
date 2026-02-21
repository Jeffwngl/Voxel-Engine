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


void processInput(GLFWwindow *window);
void mouseCallBack(GLFWwindow *window, double xPos, double yPos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void calculateFPS();


const unsigned int SCREEN_HEIGHT = 600;
const unsigned int SCREEN_WIDTH = 800;

// TESTING GEOMETRY / LIGHT CUBE DEFINITION

// float vertices[] = {
//     -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
//     0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
//     0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
//     0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
//     -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
//     -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
//     -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
//     0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
//     0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
//     0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
//     -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
//     -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
//     -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
//     -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
//     -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
//     -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
//     -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
//     -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
//     0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
//     0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
//     0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
//     0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
//     0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
//     0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
//     -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
//     0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
//     0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
//     0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
//     -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
//     -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
//     -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
//     0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
//     0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
//     0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
//     -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
//     -0.5f, 0.5f, -0.5f, 0.0f, 1.0f
// };

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

// mouse camera
float yaw = -90.0f;
float pitch = 0.0f;
float lastMouseX = static_cast<float>(SCREEN_WIDTH) / 2.0f;
float lastMouseY = static_cast<float>(SCREEN_HEIGHT) / 2.0f;

bool firstLoad = true;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

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
    std::vector<Vertex> perlinTerrain = PerlinGen::generate();

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
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                        sizeof(Vertex),
                        (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    // Tex coords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
                        sizeof(Vertex),
                        (void*)offsetof(Vertex, tex));
    glEnableVertexAttribArray(2);


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


    // textures
    // 1
    unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, numChannels;
    unsigned char *data = stbi_load("../assets/textures/dirt.png", &width, &height, &numChannels, 0);

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); // change to GL_RGB for container.jpg
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "No file in: " << data << std::endl;
    }

    stbi_image_free(data);


    shaderProgram.useShader();
    shaderProgram.setInt("texture1", 0);
    shaderProgram.setVec3("lightColor", glm::vec3(0.0f, 1.0f, 1.0f));
    // shaderProgram.setInt("texture2", 1);

    glEnable(GL_DEPTH_TEST);

    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT), 0.1f, 100.0f); // change to variables
    shaderProgram.setMat4("projection", projection);

    lightProgram.useShader();
    lightProgram.setVec3("lightColor", glm::vec3(0.0f, 1.0f, 1.0f));
    lightProgram.setMat4("projection", projection);
    lightProgram.setVec3("lightPos", lightPos);

    // Rendering loop
    while (!glfwWindowShouldClose(window)) {

        calculateFPS();

        // delta time
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Check for ESC key press
        processInput(window);

        // Set background color and clear
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // -- 1. terrain --
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);

        shaderProgram.useShader();
        
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        shaderProgram.setMat4("view", view);

        glm::mat4 terrainModel = glm::mat4(1.0f);
        shaderProgram.setMat4("terrainModel", terrainModel);

        glBindVertexArray(VAO);

        glDrawArrays(GL_TRIANGLES, 0, perlinTerrain.size());

        // -- 2. light cube -- 
        lightProgram.useShader();
        lightProgram.setMat4("view", view);

        glm::mat4 lightModel = glm::mat4(1.0f);
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

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS ) {
        glfwSetWindowShouldClose(window, true);
    }

    const float cameraSpeed = static_cast<float>(5 * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        cameraPos += cameraSpeed * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        cameraPos -= cameraSpeed * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        cameraPos += cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        cameraPos -= cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        cameraPos += cameraSpeed * cameraUp;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        cameraPos -= cameraSpeed * cameraUp;
    }
}

void mouseCallBack(GLFWwindow *window, double xPos, double yPos) {
    if (firstLoad) {
        lastMouseX = xPos;
        lastMouseY = yPos;
        firstLoad = false;
    }
    // std::cout << pitch << ", " << yaw << '\n';
    float xMouseOffset = xPos - lastMouseX;
    float yMouseOffset = yPos - lastMouseY;

    lastMouseX = xPos;
    lastMouseY = yPos;

    const float sens = 0.05f;
    xMouseOffset *= sens;
    yMouseOffset *= sens;

    yaw += xMouseOffset;
    pitch -= yMouseOffset;

    if (pitch > 89.0f) {
        pitch = 89.0f;
    }
    if (pitch < -89.0f) {
        pitch = -89.0f;
    }

    glm::vec3 cameraDirection;
    cameraDirection.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
    cameraDirection.y = glm::sin(glm::radians(pitch));
    cameraDirection.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));

    cameraFront = glm::normalize(cameraDirection);
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