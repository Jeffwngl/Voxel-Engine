#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glfw/include/GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.05f;


class Camera {
    private:
        float lastMouseX, lastMouseY;
        bool firstLoad = true;
        // calculates the front vector from the Camera's (updated) Euler Angles
        void updateCameraVectors()
        {
            // calculate the new Front vector
            glm::vec3 front;
            front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
            front.y = sin(glm::radians(Pitch));
            front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
            Front = glm::normalize(front);
            // also re-calculate the Right and Up vector
            Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
            Up    = glm::normalize(glm::cross(Right, Front));
        }

    public:
        // camera Attributes
        glm::vec3 Position;
        glm::vec3 Front;
        glm::vec3 Up;
        glm::vec3 Right;
        glm::vec3 WorldUp;
        // euler Angles
        float Yaw;
        float Pitch;
        // camera options
        float MovementSpeed;
        float MouseSensitivity;

        // vector construction
        Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY)
        {
            Position = position;
            WorldUp = up;
            Yaw = yaw;
            Pitch = pitch;
            updateCameraVectors();
        }

        // scalar construction
        Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY)
        {
            Position = glm::vec3(posX, posY, posZ);
            WorldUp = glm::vec3(upX, upY, upZ);
            Yaw = yaw;
            Pitch = pitch;
            updateCameraVectors();
        }

        void processInput(GLFWwindow *window, float deltaTime) {
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS ) {
                glfwSetWindowShouldClose(window, true);
            }

            const float cameraSpeed = static_cast<float>(5 * deltaTime);
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                Position += cameraSpeed * Front;
            }
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                Position -= cameraSpeed * Front;
            }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                Position += cameraSpeed * glm::normalize(glm::cross(Front, Up));
            }
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                Position -= cameraSpeed * glm::normalize(glm::cross(Front, Up));
            }
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
                Position += cameraSpeed * Up;
            }
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
                Position -= cameraSpeed * Up;
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

            Yaw += xMouseOffset;
            Pitch -= yMouseOffset;

            if (Pitch > 89.0f) {
                Pitch = 89.0f;
            }
            if (Pitch < -89.0f) {
                Pitch = -89.0f;
            }

            glm::vec3 cameraDirection;
            cameraDirection.x = glm::cos(glm::radians(Yaw)) * glm::cos(glm::radians(Pitch));
            cameraDirection.y = glm::sin(glm::radians(Pitch));
            cameraDirection.z = glm::sin(glm::radians(Yaw)) * glm::cos(glm::radians(Pitch));

            Front = glm::normalize(cameraDirection);
        }
};

#endif