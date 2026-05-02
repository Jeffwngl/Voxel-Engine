#include "../noise/perlin_gen.hpp"
#include "chunk_gen.hpp"

#include <math.h>
#include <iostream>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/*
Process

1. Determine the set of chunk coordinates that should be visible
   based on the player's current chunk position and render distance.
   This forms a square grid of chunks centered around the player.

2. Iterate over this target grid and check whether each required
   chunk already exists in the world map.
   - If a chunk is missing, generate its terrain data using Perlin noise
     and insert it into the world container.
   - Newly created chunks are marked as not ready (buffers not uploaded yet).

3. Iterate through all currently loaded chunks in the world.
   - Identify chunks that fall outside the render distance.
   - For those chunks, we delete their OpenGL resources (VAO/VBO) 
     if they were uploaded we remove them from the world map to free memory.

4. After update():
   - uploadMesh() uploads vertex data of newly generated chunks to the GPU
     and sets up their VAO/VBO state.

5. During render():
   - Only chunks marked as ready are drawn.
   - Each chunk binds its VAO and issues a draw call.
*/


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

ChunkManager::ChunkManager() {
    workerThread = std::thread([this]() {
        while (running) {
            GenerationRequest req;
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                cv.wait(lock, [this]() {
                    return !generationQueue.empty() || !running;
                });
                if (!running) break;
                req = generationQueue.front();
                generationQueue.pop();
            }

            GenerationResult result;
            result.key = req.key;
            result.vertices = PerlinGen::generate(0.05f, req.x, req.z);

            {
                std::lock_guard<std::mutex> lock(uploadMutex);
                uploadQueue.push(std::move(result));
            }
        }
    });
}

ChunkManager::~ChunkManager() {
    running = false;
    cv.notify_one();
    if (workerThread.joinable())
        workerThread.join();
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

                chunk.ready = false;
                world.emplace(key, std::move(chunk));
                
                {
                    std::lock_guard<std::mutex> lock(queueMutex);
                    generationQueue.push({x_shifted, z_shifted, key});
                }
                cv.notify_one();
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
    int uploadsThisFrame = 0;

    {
        std::lock_guard<std::mutex> lock(uploadMutex);
        while (!uploadQueue.empty() && uploadsThisFrame < 4) {
            GenerationResult result = std::move(uploadQueue.front());
            uploadQueue.pop();

            auto it = world.find(result.key);
            if (it == world.end()) continue; // chunk was unloaded before upload

            Chunk& chunk = it->second;
            chunk.vertices = std::move(result.vertices);
            uploadsThisFrame++;
        }
    }

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
        glBindVertexArray(chunk.VAO);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(chunk.vertices.size()));
    }
}

void ChunkManager::clear() {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        while (!generationQueue.empty()) generationQueue.pop();
    }
    {
        std::lock_guard<std::mutex> lock(uploadMutex);
        while (!uploadQueue.empty()) uploadQueue.pop();
    }
    for (auto& [key, chunk] : world) {
        if (chunk.ready) {
            glDeleteVertexArrays(1, &chunk.VAO);
            glDeleteBuffers(1, &chunk.VBO);
        }
    }
    world.clear();
}