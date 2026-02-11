#include <iostream>
#include <stdio.h>
#include <glad/glad.h>
#include <glfw/include/GLFW/glfw3.h>

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

    GLFWwindow* window = glfwCreateWindow(800, 600, "Test Window", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);  // Make context current

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    // Rendering loop
    while (!glfwWindowShouldClose(window)) {
        // Check for ESC key press
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // Set background color and clear
        glClearColor(0.2f, 0.6f, 0.9f, 1.0f);  // light blue
        glClear(GL_COLOR_BUFFER_BIT);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}