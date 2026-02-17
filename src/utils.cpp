#include "utils.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glfw/include/GLFW/glfw3.h>

namespace utils {
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

    void checkErrors(unsigned int ID, std::string type) {
        int success;
        char infoLog[512];
        if (type != "PROGRAM") {
            glGetShaderiv(ID, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(ID, 512, NULL, infoLog);
                std::cout << "Shader Compilation Error: " << type << ":\n" << infoLog << std::endl;
            }
        } else {
            glGetProgramiv(ID, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(ID, 512, NULL, infoLog);
                std::cout << "Program Linking Error:\n" << infoLog << std::endl;
            }
        }
    }
}