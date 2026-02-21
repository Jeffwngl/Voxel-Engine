#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glfw/include/GLFW/glfw3.h>

namespace camera {
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
}

#endif