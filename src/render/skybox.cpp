#include "skybox.hpp"
#include <iostream>
#include <stb_image.h>

// constructor
SkyBox::SkyBox() {
    setupMesh();
}

// destructor
SkyBox::~SkyBox() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void SkyBox::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);
}

unsigned int SkyBox::loadCubeMap(const std::vector<std::string>& faces) { // TODO: remove
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nChannels;
    for (size_t i = 0; i < faces.size(); ++i) {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nChannels, 0);
        GLenum format = (nChannels == 4) ? GL_RGBA : GL_RGB;
        if (data) {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                0, 
                format, 
                width, height, 
                0, 
                format, 
                GL_UNSIGNED_BYTE, 
                data
            );

            stbi_image_free(data);
        }
        else {
            std::cout << "Failed to load CubeMap from Path " << faces[i] << '\n';
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    return textureID;
}

void SkyBox::draw() const {
    glDepthFunc(GL_LEQUAL); // ensures skybox is always behind at z = 1.0
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}