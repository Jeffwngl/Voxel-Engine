#pragma once

#include <vector>
#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex;
    float texID;
};

class PerlinGen {
    public:
        static std::vector<Vertex> generate(float scale, int chunkX, int chunkZ);
        static std::vector<Vertex> generateGreedy(float scale, int chunkX, int chunkZ);

    private:
        static void addTopFaceGreedy(std::vector<Vertex>& v, int x, int z, int y, float ID, int width, int depth);
        static void addBottomFaceGreedy(std::vector<Vertex>& v, int x, int z, int y, float ID, int width, int depth);
        static void addFrontFaceGreedy(std::vector<Vertex>& v, int x, int z, int y, float ID, int width, int height);
        static void addBackFaceGreedy(std::vector<Vertex>& v, int x, int z, int y, float ID, int width, int height);
        static void addRightFaceGreedy(std::vector<Vertex>& v, int x, int z, int y, float ID, int depth, int height);
        static void addLeftFaceGreedy(std::vector<Vertex>& v, int x, int z, int y, float ID, int depth, int height);
};