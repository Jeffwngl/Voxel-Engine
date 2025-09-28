#include "config.h"

int main() {
    // std::cout << "hello" << std::endl;
    
    // boilerplate
    GLFWwindow* window;

    if (!glfwInit()) {
        std::cout << "GLFW cannot start." << std::endl;
        return -1;
    }

    window = glfwCreateWindow(640, 480, "Voxel Engine", NULL, NULL);
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Glad cannot start." << std::endl; 
        glfwTerminate();
        return -1;
    }

    glClearColor(0.25f, 0.5f, 0.75f, 1.0f);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents(); // clear event queue every frame

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}