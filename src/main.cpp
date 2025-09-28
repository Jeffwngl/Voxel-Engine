#include "config.h"

int main() {
    // std::cout << "hello" << std::endl;
    
    // boilerplate
    GLFWwindow* window;

    if (!glfwInit()) {
        std::cout << "GLFW cannot start." << std::endl;
        return -1;
    }

    window = glfwCreateWindow(640, 480, "Test", NULL, NULL);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents(); // clear event queue every frame
    }

    glfwTerminate();
    return 0;
}