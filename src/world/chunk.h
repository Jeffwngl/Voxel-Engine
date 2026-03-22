#ifndef CHUNK_H
#define CHUNK_H
// TODO: get rid of this file
#include <glm/glm.hpp>
#include "../noise/perlin_gen.hpp"

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

#endif