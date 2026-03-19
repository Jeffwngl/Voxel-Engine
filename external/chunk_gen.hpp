#pragma once

#include "chunk.h"

class ChunkManager {
    private:
        std::unordered_map<long long, Chunk> world;

    public:
        void update(const int playerChunk_x, const int playerChunk_z, const int render_distance);
        void uploadMesh();
        void render();
};
