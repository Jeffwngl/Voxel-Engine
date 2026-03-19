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

    private:
        static void addTopFace(std::vector<Vertex>& v, int x, int z, int y, float ID);
        static void addBottomFace(std::vector<Vertex>& v, int x, int z, int y, float ID);
        static void addRightFace(std::vector<Vertex>& v, int x, int z, int y, float ID);
        static void addLeftFace(std::vector<Vertex>& v, int x, int z, int y, float ID);
        static void addFrontFace(std::vector<Vertex>& v, int x, int z, int y, float ID);
        static void addBackFace(std::vector<Vertex>& v, int x, int z, int y, float ID);
};