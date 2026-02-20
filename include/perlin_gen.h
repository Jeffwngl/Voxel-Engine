#include <iostream>
#include <stdlib.h>

#define STB_PERLIN_IMPLEMENTATION
#include <stb_perlin.h>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>

const unsigned int CHUNK_WIDTH = 32;
const unsigned int CHUNK_LENGTH = 32;
const unsigned int CHUNK_HEIGHT = 64;

// change these values to configure the generation
float scale = 0.05f;
float airThreshold = -0.5f;

// implement block IDs for different types of blocks
const int airID = 0;
const int stoneID = 1;

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex;
};

class PerlinGen {
    private:
        static void addTopFace(std::vector<Vertex>& v, int x, int y, int z) {
            glm::vec3 normal = glm::vec3(0, 0, 1);

            // triangle 1
            v.push_back({glm::vec3(x, y, z + 1), normal, {0.0f, 0.0f}});
            v.push_back({glm::vec3(x + 1, y, z + 1), normal, {1.0f, 0.0f}});
            v.push_back({glm::vec3(x + 1, y + 1, z + 1), normal, {1.0f, 1.0f}});

            // triangle 2
            v.push_back({glm::vec3(x, y, z + 1), normal, {0.0f, 0.0f}});
            v.push_back({glm::vec3(x + 1, y + 1, z + 1), normal, {1.0f, 1.0f}});
            v.push_back({glm::vec3(x, y + 1, z + 1), normal, {0.0f, 1.0f}});
        }

        static void addBottomFace(std::vector<Vertex>& v, int x, int y, int z) {
            glm::vec3 normal = glm::vec3(0, 0, -1);
            
            // triangle 1
            v.push_back({glm::vec3(x, y, z), normal, {0.0f, 0.0f}});
            v.push_back({glm::vec3(x, y + 1, z), normal, {1.0f, 0.0f}});
            v.push_back({glm::vec3(x + 1, y + 1, z), normal, {1.0f, 1.0f}});

            // triangle 2
            v.push_back({glm::vec3(x, y, z), normal, {0.0f, 0.0f}});
            v.push_back({glm::vec3(x + 1, y + 1, z), normal, {1.0f, 1.0f}});
            v.push_back({glm::vec3(x + 1, y, z), normal, {0.0f, 1.0f}});
        }

        static void addRightFace(std::vector<Vertex>& v, int x, int y, int z) {
            glm::vec3 normal = glm::vec3(1, 0, 0);
            
            // triangle 1
            v.push_back({glm::vec3(x + 1, y, z), normal, {0.0f, 0.0f}});
            v.push_back({glm::vec3(x + 1, y + 1, z), normal, {1.0f, 0.0f}});
            v.push_back({glm::vec3(x + 1, y + 1, z + 1), normal, {1.0f, 1.0f}});

            // triangle 2
            v.push_back({glm::vec3(x + 1, y, z), normal, {0.0f, 0.0f}});
            v.push_back({glm::vec3(x + 1, y + 1, z), normal, {1.0f, 1.0f}});
            v.push_back({glm::vec3(x + 1, y, z + 1), normal, {0.0f, 1.0f}});
        }

        static void addLeftFace(std::vector<Vertex>& v, int x, int y, int z) {
            glm::vec3 normal = glm::vec3(-1, 0, 0);
            
            // triangle 1
            v.push_back({glm::vec3(x, y, z), normal, {0.0f, 0.0f}});
            v.push_back({glm::vec3(x, y, z + 1), normal, {1.0f, 0.0f}});
            v.push_back({glm::vec3(x, y + 1, z + 1), normal, {1.0f, 1.0f}});

            // triangle 2
            v.push_back({glm::vec3(x, y, z), normal, {0.0f, 0.0f}});
            v.push_back({glm::vec3(x, y + 1, z + 1), normal, {1.0f, 1.0f}});
            v.push_back({glm::vec3(x, y + 1, z), normal, {0.0f, 1.0f}});
        }

        static void addFrontFace(std::vector<Vertex>& v, int x, int y, int z) {
            glm::vec3 normal = glm::vec3(0, 1, 0);
            
            // triangle 1
            v.push_back({glm::vec3(x, y + 1, z), normal, {0.0f, 0.0f}});
            v.push_back({glm::vec3(x, y + 1, z + 1), normal, {1.0f, 0.0f}});
            v.push_back({glm::vec3(x + 1, y + 1, z + 1), normal, {1.0f, 1.0f}});

            // triangle 2
            v.push_back({glm::vec3(x, y + 1, z), normal, {0.0f, 0.0f}});
            v.push_back({glm::vec3(x + 1, y + 1, z + 1), normal, {1.0f, 1.0f}});
            v.push_back({glm::vec3(x + 1, y + 1, z), normal, {0.0f, 1.0f}});
        }

        static void addBackFace(std::vector<Vertex>& v, int x, int y, int z) {
            glm::vec3 normal = glm::vec3(0, -1, 0);
            
            // triangle 1
            v.push_back({glm::vec3(x, y, z), normal, {0.0f, 0.0f}});
            v.push_back({glm::vec3(x + 1, y, z), normal, {1.0f, 0.0f}});
            v.push_back({glm::vec3(x + 1, y, z + 1), normal, {1.0f, 1.0f}});

            // triangle 2
            v.push_back({glm::vec3(x, y, z), normal, {0.0f, 0.0f}});
            v.push_back({glm::vec3(x + 1, y, z + 1), normal, {1.0f, 1.0f}});
            v.push_back({glm::vec3(x, y, z + 1), normal, {0.0f, 1.0f}});
        }

    public:
        static std::vector<Vertex>generate() {

            // result output
            std::vector<Vertex>v;

            // initialize chunk (change to flat array?)
            std::vector<std::vector<std::vector<int>>>chunk(
                CHUNK_WIDTH, std::vector<std::vector<int>>(
                    CHUNK_LENGTH, std::vector<int> (
                        CHUNK_HEIGHT, 0
                    )
                )
            );

            for (int i = 0; i < CHUNK_WIDTH; i++) {
                for (int j = 0; j < CHUNK_LENGTH; j++) {
                    for (int k = 0; k < CHUNK_HEIGHT; k++) {
                        float nX = static_cast<float>(i) * scale;
                        float nY = static_cast<float>(j) * scale;
                        float nZ = static_cast<float>(k) * scale;
                        
                        float noiseValue = stb_perlin_noise3(nX, nY, nZ, 0, 0, 0);

                        if (noiseValue < airThreshold) {
                            chunk[i][j][k] = stoneID;
                        }
                        else {
                            chunk[i][j][k] = airID;
                        }
                    }
                }
            }

            for (int i = 0; i < CHUNK_WIDTH; i++) {
                for (int j = 0; j < CHUNK_LENGTH; j++) {
                    for (int k = 0; k < CHUNK_HEIGHT; k++) {
                        uint8_t blockType = chunk[i][j][k];

                        if (blockType == 0) continue;

                        if (blockType == 1) {
                            if (k == 63 || chunk[i][j][k + 1] == 0) {
                                addTopFace(v, i, j, k);
                            }
                            if (k == 0 || chunk[i][j][k - 1] == 0) {
                                addBottomFace(v, i, j, k);
                            }
                            if (j == 31 || chunk[i][j + 1][k] == 0) {
                                addFrontFace(v, i, j, k);
                            }
                            if (j == 0 || chunk[i][j - 1][k] == 0) {
                                addBackFace(v, i, j, k);
                            }
                            if (i == 31 || chunk[i + 1][j][k] == 0) {
                                addRightFace(v, i, j, k);
                            }
                            if (i == 0 || chunk[i - 1][j][k] == 0) {
                                addLeftFace(v, i, j, k);
                            }
                        }
                    }
                }
            }
            
            return v;
        }
};