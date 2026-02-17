#ifndef SHADER_H
#define SHADER_H

#include "utils.h"

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

std::string readFile(const char* filePath) {
    std::ifstream file(filePath);
    std::stringstream buffer;

    if (file.is_open()) {
        buffer << file.rdbuf();
        file.close();
    }
    else {
        std::cerr << "Could not open: " << filePath << std::endl;
    }
    return buffer.str();
}

class Shader {
    public:
        unsigned int shaderProgram;

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
            shaderProgram = glCreateProgram();
            glAttachShader(shaderProgram, vertexShader);
            glAttachShader(shaderProgram, fragmentShader);
            glLinkProgram(shaderProgram);
            utils::checkErrors(shaderProgram, "PROGRAM");

            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
        };

        void useShader() {
            glUseProgram(shaderProgram);
        };

        void setBool(const std::string &name, bool value) const {
            glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), (int)value);
        }
        void setInt(const std::string &name, int value) const {
            glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), value);
        }
        void setFloat(const std::string &name, float value) const {
            glUniform1f(glGetUniformLocation(shaderProgram, name.c_str()), value);
        }
};

#endif