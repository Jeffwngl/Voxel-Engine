#include "depth_map.hpp"
#include <glm/gtc/matrix_transform.hpp> // glm::ortho
#include <iostream>
// #include <glm/gtc/type_ptr.hpp> // glm::lookAt

DepthMap::DepthMap() {
    glGenFramebuffers(1, &depthMapFBO);
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(
        GL_TEXTURE_2D, 
        0, 
        GL_DEPTH_COMPONENT, 
        SHADOW_WIDTH, 
        SHADOW_HEIGHT, 
        0, 
        GL_DEPTH_COMPONENT, 
        GL_FLOAT, 
        NULL
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Clamp to border so areas outside the shadow map are not in shadow
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

DepthMap::~DepthMap() {
    glDeleteFramebuffers(1, &depthMapFBO);
    glDeleteTextures(1, &depthMap);
}

// glm::mat4 DepthMap::getLightSpaceMatrix(const glm::vec3& sunDir, const glm::vec3& cameraPos) {
//     float nearPlane = 1.0f;
//     float farPlane = 200.0f;
//     glm::mat4 lightProjection = glm::ortho(
//         -100.0f, 
//         100.0f, 
//         -100.0f, 
//         100.0f, 
//         nearPlane, 
//         farPlane
//     );
//     glm::vec3 normSunDir = glm::normalize(sunDir);
//     glm::vec3 lightPos = cameraPos + normSunDir * 100.0f;

//     glm::vec3 up = (glm::abs(normSunDir.y) > 0.99f)
//         ? glm::vec3(1.0f, 0.0f, 0.0f)
//         : glm::vec3(0.0f, 1.0f, 0.0f);

//     glm::mat4 lightView = glm::lookAt(lightPos, cameraPos, up);

//     glm::mat4 lightSpaceMatrix = lightProjection * lightView; 

//     return lightSpaceMatrix;
// }

glm::mat4 DepthMap::getLightSpaceMatrix(const glm::vec3& sunDir, const glm::vec3& cameraPos) {
    glm::vec3 normSunDir = glm::normalize(sunDir);

    glm::vec3 up = (glm::abs(normSunDir.y) > 0.99f)
        ? glm::vec3(1.0f, 0.0f, 0.0f)
        : glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec3 lightPos = cameraPos + normSunDir * 150.0f;

    float shadowRange = 100.0f;
    glm::mat4 lightProjection = glm::ortho(
        -shadowRange, 
        shadowRange,
        -shadowRange, 
        shadowRange,
        1.0f, 
        400.0f
    );

    glm::mat4 lightView = glm::lookAt(lightPos, cameraPos, up);
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;

    // snap to texel grid to prevent shimmering
    glm::vec4 shadowOrigin = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    shadowOrigin = lightSpaceMatrix * shadowOrigin;
    shadowOrigin *= (float)SHADOW_WIDTH / 2.0f;

    glm::vec4 roundedOrigin = glm::round(shadowOrigin);
    glm::vec4 roundOffset = roundedOrigin - shadowOrigin;
    roundOffset = roundOffset * 2.0f / (float)SHADOW_WIDTH;
    roundOffset.z = 0.0f;
    roundOffset.w = 0.0f;

    glm::mat4 shadowProj = lightProjection;
    shadowProj[3] += roundOffset;

    return shadowProj * lightView;
}

void DepthMap::bindForWriting() {
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void DepthMap::bindForReading(unsigned int textureID) {
    glActiveTexture(GL_TEXTURE0 + textureID);
    glBindTexture(GL_TEXTURE_2D, depthMap);
}