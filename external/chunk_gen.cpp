#include "chunk_gen.hpp"
#include "perlin_gen.hpp"

#include <math.h>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



/**
 * Helper to generate a unique key single for unordered_map from two coords
 * 
 * getChunkKey(int x, int z) packs two 32-bit chunk coordinates into a single 64-bit integer by placing x in the upper 32 bits and z in the lower 32 bits, 
 * producing a unique key that can be used in an unordered_map<long long, Chunk> for fast lookup of chunks.
 * @param x
 * @param y
 * @return 
 */
static long long getChunkKey(int x, int z) {
    return (static_cast<long long>(x) << 32) | (z & 0xffffffff);
}

/**
 * @param playerChunk_x 
 * @param playerChunk_z 
 * @param render_distance Render distance of world.
 * @return void
 */
void ChunkManager::update(const int playerChunk_x, const int playerChunk_z, const int render_distance) {
    for (int dx = -render_distance; dx <= render_distance; ++dx) {
        for (int dz = -render_distance; dz <= render_distance; ++dz) {
            int x_shifted = playerChunk_x + dx;
            int z_shifted = playerChunk_z + dz;

            long long key = getChunkKey(x_shifted, z_shifted);

            // Add new chunk in range
            if (world.find(key) == world.end()) {
                Chunk chunk;
                chunk.coord = {x_shifted, z_shifted};

                chunk.vertices = PerlinGen::generate(0.05f, x_shifted, z_shifted);
                chunk.ready = false;
                // Avoid using temporaries
                world.emplace(key, std::move(chunk));
            }
        }
    }

    for (auto it = world.begin(); it != world.end(); ) {
        int chunkX = static_cast<int>(it->second.coord.x);
        int chunkZ = static_cast<int>(it->second.coord.y);

        if (std::abs(chunkX - playerChunk_x) > render_distance ||
            std::abs(chunkZ - playerChunk_z) > render_distance) {
            
            if (it->second.ready) {
                glDeleteVertexArrays(1, &it->second.VAO);
                glDeleteBuffers(1, &it->second.VBO);
            }

            it = world.erase(it);
        }
        else {
            ++it;
        }
    }
}

void ChunkManager::uploadMesh() {
    for (auto& [key, chunk] : world) {
        if (!chunk.ready && !chunk.vertices.empty()) {
            glGenVertexArrays(1, &chunk.VAO);
            glGenBuffers(1, &chunk.VBO);

            glBindVertexArray(chunk.VAO);
            glBindBuffer(GL_ARRAY_BUFFER, chunk.VBO);

            glBufferData(GL_ARRAY_BUFFER,
                         chunk.vertices.size() * sizeof(Vertex),
                         chunk.vertices.data(),
                         GL_STATIC_DRAW);

            // Vertex positions
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
            glEnableVertexAttribArray(0);

            // Normals
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
            glEnableVertexAttribArray(1);

            // Texture coordinates
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex));
            glEnableVertexAttribArray(2);

            // Texture id
            glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texID));
            glEnableVertexAttribArray(3);

            chunk.ready = true;
        }
    }
}

void ChunkManager::render() {
    for (auto& [key, chunk] : world) {
        if (!chunk.ready) continue;

        glm::mat4 model = glm::mat4(1.0f);
        // Offset the chunk in world space
        //model = glm::translate(model, glm::vec3(chunk.coord.x * 16, 0.0f, chunk.coord.y * 16));

        glBindVertexArray(chunk.VAO);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(chunk.vertices.size()));
    }
}