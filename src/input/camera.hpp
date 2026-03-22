#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.05f;

namespace Engine {

class Camera {
private:
    float lastMouseX = 0.0f;
    float lastMouseY = 0.0f;
    bool  firstLoad  = true;

    void updateCameraVectors();  // ← declaration only

public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    float Yaw;
    float Pitch;
    float MovementSpeed;
    float MouseSensitivity;

    Camera(glm::vec3 position = glm::vec3(0.0f),
           glm::vec3 up       = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw   = YAW,
           float pitch = PITCH);

    void processInput(GLFWwindow* window, float deltaTime);
    void mouseCallBack(GLFWwindow* window, double xPos, double yPos);
};

} // namespace Engine