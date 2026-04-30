#include <glad/glad.h>
#include <glm/glm.hpp>

class DepthMap {
    private:
        unsigned int depthMapFBO;
        unsigned int depthMap;

    public:
        const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;

        DepthMap();
        ~DepthMap();

        void bindForWriting();
        void bindForReading(unsigned int textureID);
        glm::mat4 getLightSpaceMatrix(const glm::vec3& sunDir, const glm::vec3& cameraPos);
};