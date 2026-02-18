#ifndef SHADER_H
#define SHADER_H

#include "utils.h"

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
    public:
        unsigned int shaderID;

        Shader(const char* vertexPath, const char* fragmentPath) {
            // load shaders
            std::string vertexSource = utils::readFile(vertexPath);
            std::string fragmentSource = utils::readFile(fragmentPath);
            const char* vertexSourcePtr = vertexSource.c_str();
            const char* fragmentSourcePtr = fragmentSource.c_str();

            // compile shaders
            unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertexShader, 1, &vertexSourcePtr, NULL);
            glCompileShader(vertexShader);
            utils::checkErrors(vertexShader, "VERTEX");

            unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragmentShader, 1, &fragmentSourcePtr, NULL);
            glCompileShader(fragmentShader);
            utils::checkErrors(fragmentShader, "FRAGMENT");
            
            // program
            shaderID = glCreateProgram();
            glAttachShader(shaderID, vertexShader);
            glAttachShader(shaderID, fragmentShader);
            glLinkProgram(shaderID);
            utils::checkErrors(shaderID, "PROGRAM");

            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
        };

        void useShader() {
            glUseProgram(shaderID);
        };

        void setBool(const std::string &name, bool value) const {
            glUniform1i(glGetUniformLocation(shaderID, name.c_str()), (int)value);
        }
        void setInt(const std::string &name, int value) const {
            glUniform1i(glGetUniformLocation(shaderID, name.c_str()), value);
        }
        void setFloat(const std::string &name, float value) const {
            glUniform1f(glGetUniformLocation(shaderID, name.c_str()), value);
        }
        void setMat4(const std::string &name, const glm::mat4 &mat) const {
            glUniformMatrix4fv(glGetUniformLocation(shaderID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }
};

#endif