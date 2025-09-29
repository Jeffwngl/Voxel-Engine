#include "config.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

int main() {
    // std::cout << "hello" << std::endl;
    
    // GLFW initialization
    GLFWwindow* window; // pointer to window

    if (!glfwInit()) {
        std::cout << "GLFW cannot start." << std::endl;
        return -1;
    }

    // glfw version and profile control
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(640, 480, "Voxel Engine", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // GLAD initialization
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { // find functions for Open GL
        std::cout << "Glad cannot start." << std::endl; 
        glfwTerminate();
        return -1;
    }

    glViewport(0, 0, 640, 480);

    glClearColor(0.25f, 0.5f, 0.75f, 1.0f);

    while (!glfwWindowShouldClose(window)) { // keeps window up

        processInput(window); // checks esc

        glfwPollEvents(); // checks keyboard or mouse events

        glClear(GL_COLOR_BUFFER_BIT); // resets color buffer to defined color

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

// resizing window
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}