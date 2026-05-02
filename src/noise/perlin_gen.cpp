#define STB_PERLIN_IMPLEMENTATION
#include <stb_perlin.h>
#include "perlin_gen.hpp"

#include <vector>
#include <random>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>

/*
Process

This algorithm will iterate through all the blocks in a chunk, depending on
the perlin noise generation function, it will determine whether it is a solid
block or an air block.

The chunk data structure stores all the IDs of the blocks within the chunk,
the algorithm iterates through the chunk data and appends the vertex mesh data
based on the block ID to the std::vector v which is then uploaded to the buffer.

Before uploading, it will perform greedy meshing based on the face, ID and plane of the
texture and combine them.

https://www.youtube.com/watch?v=4xs66m1Of4A&t=410s

Optimizations
A 1D vector hash is used to optimize storage and lookup. (to implement)

*/

/**
 * Define size constraints for a default chunk here
 * A 16x16x32 chunk is used as a default
 */
const unsigned int CHUNK_WIDTH = 16;
const unsigned int CHUNK_LENGTH = 16;
const unsigned int CHUNK_HEIGHT = 32;

// change these values to configure the generation
constexpr float airThreshold = 0.0f;

// implement block IDs for different types of blocks
const int airID = 0;
const int solidID = 1;

// textures IDs for different faces
const float defaultTex = 0;
const float sideTex = 1;
const float topTex = 2;
const float flowerTex = 3;

// chance of flower tile generating
constexpr float chance = 0.06f;

// greedy merge helper - finds largest rectangle in mask and emits faces
// mask value of -1 means no face, otherwise stores texID
using Mask2D = std::vector<std::vector<float>>;
using Used2D = std::vector<std::vector<bool>>;

static void greedyMergeXZ(
    std::vector<Vertex>& v,
    const Mask2D& mask,
    int width, int depth, int k,
    int chunkX, int chunkZ,
    void (*emitFace)(std::vector<Vertex>&, int, int, int, float, int, int)
) {
    Used2D used(width, std::vector<bool>(depth, false));

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < depth; j++) {
            if (used[i][j] || mask[i][j] < 0.0f) continue;

            float texID = mask[i][j];

            // expand width along x
            int w = 1;
            while (i + w < width && !used[i + w][j] && mask[i + w][j] == texID)
                w++;

            // expand depth along z
            int d = 1;
            bool canExpand = true;
            while (j + d < depth && canExpand) {
                for (int di = 0; di < w; di++) {
                    if (used[i + di][j + d] || mask[i + di][j + d] != texID) {
                        canExpand = false;
                        break;
                    }
                }
                if (canExpand) d++;
            }

            // mark used
            for (int di = 0; di < w; di++)
                for (int dj = 0; dj < d; dj++)
                    used[i + di][j + dj] = true;

            int worldX = i + chunkX * CHUNK_WIDTH;
            int worldZ = j + chunkZ * CHUNK_LENGTH;
            emitFace(v, worldX, worldZ, k, texID, w, d);
        }
    }
}

/**
 * @param scale The scale used for the perlin generation size.
 * @return A vector of vertices storing the generated chunk.
 */
std::vector<Vertex> PerlinGen::generate(float scale, int chunkX, int chunkZ) {

    std::vector<Vertex>v;

    // initialize chunk
    // structure is [x][z][y] to keep y as the vertical axis
    std::vector<std::vector<std::vector<int>>>chunk( // TODO: change to flat array
        CHUNK_WIDTH, std::vector<std::vector<int>>(
            CHUNK_LENGTH, std::vector<int> (CHUNK_HEIGHT, 0)
        )
    );

    auto isAir = [&](int x, int z, int y) -> bool {
        if (x < 0 || x >= CHUNK_WIDTH ||
            z < 0 || z >= CHUNK_LENGTH ||
            y < 0 || y >= CHUNK_HEIGHT) {
            return true;
        }
        return chunk[x][z][y] == airID;
    };

    // random flower texture generator
    std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    for (int i = 0; i < (int)CHUNK_WIDTH; i++) {
        for (int j = 0; j < (int)CHUNK_LENGTH; j++) {
            for (int k = 0; k < (int)CHUNK_HEIGHT; k++) {
                float nX = (i + chunkX * (int)CHUNK_WIDTH)  * scale;
                float nZ = (j + chunkZ * (int)CHUNK_LENGTH) * scale;
                float nY = k * scale;

                float noiseValue = stb_perlin_noise3(nX, nY, nZ, 0, 0, 0);
                float heightGradient = (static_cast<float>(k) / CHUNK_HEIGHT) * 2.0f - 1.0f;
                float finalValue = noiseValue - heightGradient;

                chunk[i][j][k] = (finalValue > airThreshold) ? solidID : airID;
            }
        }
    }

    /* Greed meshing */
    // top faces
    for (int k = 0; k < CHUNK_HEIGHT; k++) {
        Mask2D mask(CHUNK_WIDTH, std::vector<float>(CHUNK_LENGTH, -1.0f));
        for (int i = 0; i < CHUNK_WIDTH; i++) {
            for (int j = 0; j < CHUNK_LENGTH; j++) {
                if (chunk[i][j][k] != airID && isAir(i, j, k + 1)) {
                    float r = dist(gen);
                    // flowers don't merge with grass — different texID keeps them separate
                    mask[i][j] = r < chance ? flowerTex : topTex;
                }
            }
        }
        greedyMergeXZ(v, mask, CHUNK_WIDTH, CHUNK_LENGTH, k, chunkX, chunkZ, addTopFaceGreedy);
    }

    // bottom faces
    for (int k = 0; k < CHUNK_HEIGHT; k++) {
        Mask2D mask(CHUNK_WIDTH, std::vector<float>(CHUNK_LENGTH, -1.0f));
        for (int i = 0; i < CHUNK_WIDTH; i++)
            for (int j = 0; j < CHUNK_LENGTH; j++)
                if (chunk[i][j][k] != airID && isAir(i, j, k - 1))
                    mask[i][j] = defaultTex;
        greedyMergeXZ(v, mask, CHUNK_WIDTH, CHUNK_LENGTH, k, chunkX, chunkZ, addBottomFaceGreedy);
    }

    // front faces +z — merge along z only, height = 1
    for (int k = 0; k < CHUNK_HEIGHT; k++) {
        Mask2D mask(CHUNK_WIDTH, std::vector<float>(CHUNK_LENGTH, -1.0f));
        for (int i = 0; i < CHUNK_WIDTH; i++) {
            for (int j = 0; j < CHUNK_LENGTH; j++) {
                if (chunk[i][j][k] != airID && isAir(i, j + 1, k)) {
                    bool topExposed = (k + 1 >= CHUNK_HEIGHT) || chunk[i][j][k + 1] == airID;
                    mask[i][j] = topExposed ? sideTex : defaultTex;
                }
            }
        }
        // for side faces only merge width (x), not depth (z is face direction)
        Used2D used(CHUNK_WIDTH, std::vector<bool>(CHUNK_LENGTH, false));
        for (int i = 0; i < CHUNK_WIDTH; i++) {
            for (int j = 0; j < CHUNK_LENGTH; j++) {
                if (used[i][j] || mask[i][j] < 0.0f) continue;
                float texID = mask[i][j];
                int w = 1;
                while (i + w < CHUNK_WIDTH && !used[i + w][j] && mask[i + w][j] == texID) w++;
                for (int di = 0; di < w; di++) used[i + di][j] = true;
                int worldX = i + chunkX * CHUNK_WIDTH;
                int worldZ = j + chunkZ * CHUNK_LENGTH;
                addFrontFaceGreedy(v, worldX, worldZ, k, texID, w, 1);
            }
        }
    }

    // back faces -z
    for (int k = 0; k < CHUNK_HEIGHT; k++) {
        Mask2D mask(CHUNK_WIDTH, std::vector<float>(CHUNK_LENGTH, -1.0f));
        for (int i = 0; i < CHUNK_WIDTH; i++) {
            for (int j = 0; j < CHUNK_LENGTH; j++) {
                if (chunk[i][j][k] != airID && isAir(i, j - 1, k)) {
                    bool topExposed = (k + 1 >= CHUNK_HEIGHT) || chunk[i][j][k + 1] == airID;
                    mask[i][j] = topExposed ? sideTex : defaultTex;
                }
            }
        }
        Used2D used(CHUNK_WIDTH, std::vector<bool>(CHUNK_LENGTH, false));
        for (int i = 0; i < CHUNK_WIDTH; i++) {
            for (int j = 0; j < CHUNK_LENGTH; j++) {
                if (used[i][j] || mask[i][j] < 0.0f) continue;
                float texID = mask[i][j];
                int w = 1;
                while (i + w < CHUNK_WIDTH && !used[i + w][j] && mask[i + w][j] == texID) w++;
                for (int di = 0; di < w; di++) used[i + di][j] = true;
                int worldX = i + chunkX * CHUNK_WIDTH;
                int worldZ = j + chunkZ * CHUNK_LENGTH;
                addBackFaceGreedy(v, worldX, worldZ, k, texID, w, 1);
            }
        }
    }

    // right faces +x — merge along z
    for (int k = 0; k < CHUNK_HEIGHT; k++) {
        Mask2D mask(CHUNK_WIDTH, std::vector<float>(CHUNK_LENGTH, -1.0f));
        for (int i = 0; i < CHUNK_WIDTH; i++) {
            for (int j = 0; j < CHUNK_LENGTH; j++) {
                if (chunk[i][j][k] != airID && isAir(i + 1, j, k)) {
                    bool topExposed = (k + 1 >= CHUNK_HEIGHT) || chunk[i][j][k + 1] == airID;
                    mask[i][j] = topExposed ? sideTex : defaultTex;
                }
            }
        }
        Used2D used(CHUNK_WIDTH, std::vector<bool>(CHUNK_LENGTH, false));
        for (int i = 0; i < CHUNK_WIDTH; i++) {
            for (int j = 0; j < CHUNK_LENGTH; j++) {
                if (used[i][j] || mask[i][j] < 0.0f) continue;
                float texID = mask[i][j];
                int d = 1;
                while (j + d < CHUNK_LENGTH && !used[i][j + d] && mask[i][j + d] == texID) d++;
                for (int dj = 0; dj < d; dj++) used[i][j + dj] = true;
                int worldX = i + chunkX * CHUNK_WIDTH;
                int worldZ = j + chunkZ * CHUNK_LENGTH;
                addRightFaceGreedy(v, worldX, worldZ, k, texID, d, 1);
            }
        }
    }

    // left faces -x - merge along x
    for (int k = 0; k < CHUNK_HEIGHT; k++) {
        Mask2D mask(CHUNK_WIDTH, std::vector<float>(CHUNK_LENGTH, -1.0f));
        for (int i = 0; i < CHUNK_WIDTH; i++) {
            for (int j = 0; j < CHUNK_LENGTH; j++) {
                if (chunk[i][j][k] != airID && isAir(i - 1, j, k)) {
                    bool topExposed = (k + 1 >= CHUNK_HEIGHT) || chunk[i][j][k + 1] == airID;
                    mask[i][j] = topExposed ? sideTex : defaultTex;
                }
            }
        }
        Used2D used(CHUNK_WIDTH, std::vector<bool>(CHUNK_LENGTH, false));
        for (int i = 0; i < CHUNK_WIDTH; i++) {
            for (int j = 0; j < CHUNK_LENGTH; j++) {
                if (used[i][j] || mask[i][j] < 0.0f) continue;
                float texID = mask[i][j];
                int d = 1;
                while (j + d < CHUNK_LENGTH && !used[i][j + d] && mask[i][j + d] == texID) d++;
                for (int dj = 0; dj < d; dj++) used[i][j + dj] = true;
                int worldX = i + chunkX * CHUNK_WIDTH;
                int worldZ = j + chunkZ * CHUNK_LENGTH;
                addLeftFaceGreedy(v, worldX, worldZ, k, texID, d, 1);
            }
        }
    }
    
    return v;
};

void PerlinGen::addTopFaceGreedy(std::vector<Vertex>& v, int x, int z, int y, float ID, int width, int depth) {
    glm::vec3 normal = glm::vec3(0, 1, 0);
    float w = static_cast<float>(width);
    float d = static_cast<float>(depth);

    v.push_back({glm::vec3(x, y + 1, z), normal, {0.0f, 0.0f}, ID});
    v.push_back({glm::vec3(x + w, y + 1, z), normal, {w, 0.0f}, ID});
    v.push_back({glm::vec3(x + w, y + 1, z + d), normal, {w, d}, ID});

    v.push_back({glm::vec3(x, y + 1, z), normal, {0.0f, 0.0f}, ID});
    v.push_back({glm::vec3(x + w, y + 1, z + d), normal, {w, d}, ID});
    v.push_back({glm::vec3(x, y + 1, z + d), normal, {0.0f, d}, ID});
}

void PerlinGen::addBottomFaceGreedy(std::vector<Vertex>& v, int x, int z, int y, float ID, int width, int depth) {
    glm::vec3 normal = glm::vec3(0, -1, 0);
    float w = static_cast<float>(width);
    float d = static_cast<float>(depth);

    v.push_back({glm::vec3(x, y, z), normal, {0.0f, 0.0f}, ID});
    v.push_back({glm::vec3(x, y, z + d), normal, {0.0f, d}, ID});
    v.push_back({glm::vec3(x + w, y, z + d), normal, {w, d}, ID});

    v.push_back({glm::vec3(x, y, z), normal, {0.0f, 0.0f}, ID});
    v.push_back({glm::vec3(x + w, y, z + d), normal, {w, d}, ID});
    v.push_back({glm::vec3(x + w, y, z), normal, {w, 0.0f}, ID});
}

void PerlinGen::addFrontFaceGreedy(std::vector<Vertex>& v, int x, int z, int y, float ID, int width, int height) {
    glm::vec3 normal = glm::vec3(0, 0, 1);
    float w = static_cast<float>(width);
    float h = static_cast<float>(height);

    v.push_back({glm::vec3(x, y, z + 1), normal, {0.0f, h}, ID});
    v.push_back({glm::vec3(x, y + h, z + 1), normal, {0.0f, 0.0f}, ID});
    v.push_back({glm::vec3(x + w, y + h, z + 1), normal, {w, 0.0f}, ID});

    v.push_back({glm::vec3(x, y, z + 1), normal, {0.0f, h}, ID});
    v.push_back({glm::vec3(x + w, y + h, z + 1), normal, {w, 0.0f}, ID});
    v.push_back({glm::vec3(x + w, y, z + 1), normal, {w, h}, ID});
}

void PerlinGen::addBackFaceGreedy(std::vector<Vertex>& v, int x, int z, int y, float ID, int width, int height) {
    glm::vec3 normal = glm::vec3(0, 0, -1);
    float w = static_cast<float>(width);
    float h = static_cast<float>(height);

    v.push_back({glm::vec3(x, y, z), normal, {w, h}, ID});
    v.push_back({glm::vec3(x + w, y, z), normal, {0.0f, h}, ID});
    v.push_back({glm::vec3(x + w, y + h, z), normal, {0.0f, 0.0f}, ID});

    v.push_back({glm::vec3(x, y, z), normal, {w, h}, ID});
    v.push_back({glm::vec3(x + w, y + h, z), normal, {0.0f, 0.0f}, ID});
    v.push_back({glm::vec3(x, y + h, z), normal, {w, 0.0f}, ID});
}

void PerlinGen::addRightFaceGreedy(std::vector<Vertex>& v, int x, int z, int y, float ID, int depth, int height) {
    glm::vec3 normal = glm::vec3(1, 0, 0);
    float d = static_cast<float>(depth);
    float h = static_cast<float>(height);

    v.push_back({glm::vec3(x + 1, y, z), normal, {0.0f, h}, ID});
    v.push_back({glm::vec3(x + 1, y + h, z), normal, {0.0f, 0.0f}, ID});
    v.push_back({glm::vec3(x + 1, y + h, z + d), normal, {d, 0.0f}, ID});

    v.push_back({glm::vec3(x + 1, y, z), normal, {0.0f, h}, ID});
    v.push_back({glm::vec3(x + 1, y + h, z + d), normal, {d, 0.0f}, ID});
    v.push_back({glm::vec3(x + 1, y, z + d), normal, {d, h}, ID});
}

void PerlinGen::addLeftFaceGreedy(std::vector<Vertex>& v, int x, int z, int y, float ID, int depth, int height) {
    glm::vec3 normal = glm::vec3(-1, 0, 0);
    float d = static_cast<float>(depth);
    float h = static_cast<float>(height);

    v.push_back({glm::vec3(x, y, z), normal, {d, h}, ID});
    v.push_back({glm::vec3(x, y, z + d), normal, {0.0f, h}, ID});
    v.push_back({glm::vec3(x, y + h, z + d), normal, {0.0f, 0.0f}, ID});

    v.push_back({glm::vec3(x, y, z), normal, {d, h}, ID});
    v.push_back({glm::vec3(x, y + h, z + d), normal, {0.0f, 0.0f}, ID});
    v.push_back({glm::vec3(x, y + h, z), normal, {d, 0.0f}, ID});
}