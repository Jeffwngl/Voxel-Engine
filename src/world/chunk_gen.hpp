#pragma once

#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>
#include "../noise/perlin_gen.hpp"

#include <thread>
#include <mutex>
#include <queue>
#include <atomic>

struct GenerationRequest {
    int x, z;
    long long key;
};

struct GenerationResult {
    long long key;
    std::vector<Vertex> vertices;
};

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

        std::queue<GenerationRequest> generationQueue;
        std::queue<GenerationResult> uploadQueue;
        std::mutex queueMutex;
        std::mutex uploadMutex;
        std::atomic<bool> running{true};
        std::thread workerThread;
        std::condition_variable cv;

    public:
        ChunkManager();
        ~ChunkManager();

        void update(const int playerChunk_x, const int playerChunk_z, const int render_distance);
        void uploadMesh();
        void render();
        void clear();
};
