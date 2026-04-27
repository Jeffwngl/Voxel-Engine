#pragma once

#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>
#include "../noise/perlin_gen.hpp"

// #include "chunk.h"

/**
 * @struct Chunk
 * @brief Represents a single voxel block chunk in the world
 * 
 * Each chunk has a 2D coordinate, a vertex buffer for rendering, and OpenGL calls.
 */
struct Chunk {
    glm::vec2 coord;
    std::vector<Vertex>vertices;
    unsigned int VBO, VAO;
    /**
     * @brief Becomes true after mesh generation and buffer uploads.
     */
    bool ready = false;
};


class ChunkManager {
    private:
        std::unordered_map<long long, Chunk> world;

    public:
        void update(const int playerChunk_x, const int playerChunk_z, const int render_distance);
        void uploadMesh();
        void render();
};
