#include <iostream>
#include <stdlib.h>
#include <vector>
#include <random>

#define STB_PERLIN_IMPLEMENTATION
#include <stb_perlin.h>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>

const unsigned int CHUNK_WIDTH = 128;
const unsigned int CHUNK_LENGTH = 128;
const unsigned int CHUNK_HEIGHT = 32;

// change these values to configure the generation
float airThreshold = 0.0f;

// implement block IDs for different types of blocks
const int airID = 0;
const int solidID = 1;

// textures IDs for different faces
const float defaultTex = 0;
const float sideTex = 1;
const float topTex = 2;
const float flowerTex = 3;

// chance of flower tile generating
const float chance = 0.06f;

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex;
    float texID;
};

class PerlinGen {
    private:
        // represents the +Y face (skyward)
        static void addTopFace(std::vector<Vertex>& v, int x, int z, int y, float ID) {
            glm::vec3 normal = glm::vec3(0, 1, 0);

            static std::mt19937 gen(std::random_device{}()); // only gets called once static
            static std::uniform_int_distribution<int> dist(0, 3); // 0,1,2,3
            
            int r = ID == 3 ? dist(gen) : 0;

            glm::vec2 uv[4] = {
                {0.0f, 0.0f}, // bottom-left
                {1.0f, 0.0f}, // bottom-right
                {1.0f, 1.0f}, // top-right
                {0.0f, 1.0f}  // top-left
            };

            glm::vec2 rotatedUV[4];

            for (int i = 0; i < 4; ++i) {
                switch(r) {
                    case 0: rotatedUV[i] = uv[i]; break; // 0
                    case 1: rotatedUV[i] = glm::vec2(1.0f - uv[i].y, uv[i].x); break; // 90
                    case 2: rotatedUV[i] = glm::vec2(1.0f - uv[i].x, 1.0f - uv[i].y); break; // 180
                    case 3: rotatedUV[i] = glm::vec2(uv[i].y, 1.0f - uv[i].x); break; // 270
                }
            }

            // triangle 1
            v.push_back({glm::vec3(x, y + 1, z), normal, rotatedUV[0], ID});
            v.push_back({glm::vec3(x + 1, y + 1, z), normal, rotatedUV[1], ID});
            v.push_back({glm::vec3(x + 1, y + 1, z + 1), normal, rotatedUV[2], ID});

            // triangle 2
            v.push_back({glm::vec3(x, y + 1, z), normal, rotatedUV[0], ID});
            v.push_back({glm::vec3(x + 1, y + 1, z + 1), normal, rotatedUV[2], ID});
            v.push_back({glm::vec3(x, y + 1, z + 1), normal, rotatedUV[3], ID});
        }

        // represents the -Y face (groundward)
        static void addBottomFace(std::vector<Vertex>& v, int x, int z, int y, float ID) {
            glm::vec3 normal = glm::vec3(0, -1, 0);
            
            // triangle 1
            v.push_back({glm::vec3(x, y, z), normal, {0.0f, 0.0f}, ID});
            v.push_back({glm::vec3(x, y, z + 1), normal, {1.0f, 0.0f}, ID});
            v.push_back({glm::vec3(x + 1, y, z + 1), normal, {1.0f, 1.0f}, ID});

            // triangle 2
            v.push_back({glm::vec3(x, y, z), normal, {0.0f, 0.0f}, ID});
            v.push_back({glm::vec3(x + 1, y, z + 1), normal, {1.0f, 1.0f}, ID});
            v.push_back({glm::vec3(x + 1, y, z), normal, {0.0f, 1.0f}, ID});
        }

        static void addRightFace(std::vector<Vertex>& v, int x, int z, int y, float ID) {
            glm::vec3 normal = glm::vec3(1, 0, 0);
            
            // triangle 1
            v.push_back({glm::vec3(x + 1, y, z), normal, {0.0f, 1.0f}, ID}); // bot left
            v.push_back({glm::vec3(x + 1, y + 1, z), normal, {0.0f, 0.0f}, ID}); // top left
            v.push_back({glm::vec3(x + 1, y + 1, z + 1), normal, {1.0f, 0.0f}, ID}); // top right

            // triangle 2
            v.push_back({glm::vec3(x + 1, y, z), normal, {0.0f, 1.0f}, ID}); // bot left
            v.push_back({glm::vec3(x + 1, y + 1, z + 1), normal, {1.0f, 0.0f}, ID}); // top right
            v.push_back({glm::vec3(x + 1, y, z + 1), normal, {1.0f, 1.0f}, ID}); // bot right
        }

        static void addLeftFace(std::vector<Vertex>& v, int x, int z, int y, float ID) {
            glm::vec3 normal = glm::vec3(-1, 0, 0);
            
            // triangle 1
            v.push_back({glm::vec3(x, y, z), normal, {1.0f, 1.0f}, ID});
            v.push_back({glm::vec3(x, y, z + 1), normal, {0.0f, 1.0f}, ID});
            v.push_back({glm::vec3(x, y + 1, z + 1), normal, {0.0f, 0.0f}, ID});

            // triangle 2
            v.push_back({glm::vec3(x, y, z), normal, {1.0f, 1.0f}, ID});
            v.push_back({glm::vec3(x, y + 1, z + 1), normal, {0.0f, 0.0f}, ID});
            v.push_back({glm::vec3(x, y + 1, z), normal, {1.0f, 0.0f}, ID});
        }

        static void addFrontFace(std::vector<Vertex>& v, int x, int z, int y, float ID) {
            glm::vec3 normal = glm::vec3(0, 0, 1);
            
            // triangle 1
            v.push_back({glm::vec3(x, y, z + 1), normal, {0.0f, 1.0f}, ID});
            v.push_back({glm::vec3(x, y + 1, z + 1), normal, {0.0f, 0.0f}, ID});
            v.push_back({glm::vec3(x + 1, y + 1, z + 1), normal, {1.0f, 0.0f}, ID});

            // triangle 2
            v.push_back({glm::vec3(x, y, z + 1), normal, {0.0f, 1.0f}, ID});
            v.push_back({glm::vec3(x + 1, y + 1, z + 1), normal, {1.0f, 0.0f}, ID});
            v.push_back({glm::vec3(x + 1, y, z + 1), normal, {1.0f, 1.0f}, ID});
        }

        static void addBackFace(std::vector<Vertex>& v, int x, int z, int y, float ID) {
            glm::vec3 normal = glm::vec3(0, 0, -1);
            
            // triangle 1
            v.push_back({glm::vec3(x, y, z), normal, {1.0f, 1.0f}, ID});
            v.push_back({glm::vec3(x + 1, y, z), normal, {0.0f, 1.0f}, ID});
            v.push_back({glm::vec3(x + 1, y + 1, z), normal, {0.0f, 0.0f}, ID});

            // triangle 2
            v.push_back({glm::vec3(x, y, z), normal, {1.0f, 1.0f}, ID});
            v.push_back({glm::vec3(x + 1, y + 1, z), normal, {0.0f, 0.0f}, ID});
            v.push_back({glm::vec3(x, y + 1, z), normal, {1.0f, 0.0f}, ID});
        }

    public:
        static std::vector<Vertex>generate(float scale) {

            // result output
            std::vector<Vertex>v;

            // initialize chunk
            // structure is [x][z][y] to keep y as the vertical axis
            std::vector<std::vector<std::vector<int>>>chunk(
                CHUNK_WIDTH, std::vector<std::vector<int>>(
                    CHUNK_LENGTH, std::vector<int> (
                        CHUNK_HEIGHT, 0
                    )
                )
            );

            // random flower texture generator
            std::mt19937 gen(std::random_device{}());

            for (int i = 0; i < CHUNK_WIDTH; i++) {
                for (int j = 0; j < CHUNK_LENGTH; j++) {
                    for (int k = 0; k < CHUNK_HEIGHT; k++) {
                        float nX = static_cast<float>(i) * scale;
                        float nZ = static_cast<float>(j) * scale;
                        float nY = static_cast<float>(k) * scale;
                        
                        float noiseValue = stb_perlin_noise3(nX, nY, nZ, 0, 0, 0);

                        // density calculation with height gradient
                        float heightGradient = (static_cast<float>(k) / CHUNK_HEIGHT) * 2.0f - 1.0f;
                        float finalValue = noiseValue - heightGradient;

                        if (finalValue > airThreshold) {
                            chunk[i][j][k] = solidID;
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
                        int blockType = chunk[i][j][k];

                        if (blockType == 0) continue;

                        if (blockType == 1) {
                            if (k == CHUNK_HEIGHT - 1 || chunk[i][j][k + 1] == 0) {
                                float r = std::uniform_real_distribution<float>(0.0f, 1.0f)(gen);
                                if (r < chance) {
                                    addTopFace(v, i, j, k, flowerTex);
                                }
                                else {
                                    addTopFace(v, i, j, k, topTex);   
                                }
                            }
                            if (k == 0 || chunk[i][j][k - 1] == 0) {
                                addBottomFace(v, i, j, k, defaultTex);
                            }
                            if ((j == CHUNK_LENGTH - 1 || chunk[i][j + 1][k] == 0) && (chunk[i][j][k + 1] == 0)) {
                                addFrontFace(v, i, j, k, sideTex);
                            }
                            if ((j == 0 || chunk[i][j - 1][k] == 0) && (chunk[i][j][k + 1] == 0)) {
                                addBackFace(v, i, j, k, sideTex);
                            }
                            if ((i == CHUNK_WIDTH - 1 || chunk[i + 1][j][k] == 0) && (chunk[i][j][k + 1] == 0)) {
                                addRightFace(v, i, j, k, sideTex);
                            }
                            if ((i == 0 || chunk[i - 1][j][k] == 0) && (chunk[i][j][k + 1] == 0)) {
                                addLeftFace(v, i, j, k, sideTex);
                            }

                            if (j == CHUNK_LENGTH - 1 || chunk[i][j + 1][k] == 0) {
                                addFrontFace(v, i, j, k, defaultTex);
                            }
                            if (j == 0 || chunk[i][j - 1][k] == 0) {
                                addBackFace(v, i, j, k, defaultTex);
                            }
                            if (i == CHUNK_WIDTH - 1 || chunk[i + 1][j][k] == 0) {
                                addRightFace(v, i, j, k, defaultTex);
                            }
                            if (i == 0 || chunk[i - 1][j][k] == 0) {
                                addLeftFace(v, i, j, k, defaultTex);
                            }
                        }
                    }
                }
            }
            
        return v;
    }
};