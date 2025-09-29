#include "config.h"
#include "shapes/triangle.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

std::string loadShaderSource(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << filePath << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();  // read whole file into buffer
    return buffer.str();
}

int main() {

    // import vertex shader text
    std::string vertexShaderCode = loadShaderSource("shaders/vertex.txt");
    const char* vertexShaderSource = vertexShaderCode.c_str();

    // GLFW initialization
    GLFWwindow* window; // pointer to window

    if (!glfwInit()) {
        std::cout << "GLFW cannot start." << std::endl;
        return -1;
    }

    // GLFW version and profile control
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(640, 480, "Voxel Engine", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // call on every window resize

    // GLAD initialization
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { // find functions for Open GL
        std::cout << "Glad cannot start." << std::endl; 
        glfwTerminate();
        return -1;
    }

    glViewport(0, 0, 640, 480);


    // vertex shader buffer
    unsigned int VBO;
    glGenBuffers(1, &VBO); // generate ID for vertex buffer store this id in VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // bind ID to vertex buffer object

    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);

    // compile vertex shader
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // attach shader source to shader object
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" <<
        infoLog << std::endl;
    }

    // main loop
    while (!glfwWindowShouldClose(window)) { // keeps window up

        // key inputs
        processInput(window); // checks esc

        // rendering inputs
        glClearColor(0.25f, 0.5f, 0.75f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT); // resets color buffer to defined color

        // buffers and event calls
        glfwPollEvents(); // checks keyboard or mouse events
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

// esc close key input
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

// resizing window
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}