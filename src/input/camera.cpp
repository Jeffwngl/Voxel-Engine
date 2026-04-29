#include "camera.hpp"

namespace Engine {

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)),
      MovementSpeed(SPEED),
      MouseSensitivity(SENSITIVITY),
      Position(position),
      WorldUp(up),
      Yaw(yaw),
      Pitch(pitch)
{
    updateCameraVectors();
}

void Camera::updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
}

void Camera::processInput(GLFWwindow* window, float deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    const float speed = 5.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) Position += speed * Front;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) Position -= speed * Front;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) Position += speed * glm::normalize(glm::cross(Front, Up));
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) Position -= speed * glm::normalize(glm::cross(Front, Up));
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) Position += speed * Up;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) Position -= speed * Up;
}

void Camera::mouseCallBack(GLFWwindow* window, double xPos, double yPos) {
    if (firstLoad) {
        lastMouseX = xPos;
        lastMouseY = yPos;
        firstLoad = false;
    }

    float xOffset = (xPos - lastMouseX) * SENSITIVITY;
    float yOffset = (yPos - lastMouseY) * SENSITIVITY;
    lastMouseX = xPos;
    lastMouseY = yPos;

    Yaw += xOffset;
    Pitch -= yOffset;
    Pitch = glm::clamp(Pitch, -89.0f, 89.0f);

    updateCameraVectors();
}

}