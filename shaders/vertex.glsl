#version 330 core
layout (location = 0) in vec3 aPos; // position
layout (location = 1) in vec3 aNormal; // normal
layout (location = 2) in vec2 aTexCoord; // texture
layout (location = 3) in float aTexID;

out vec3 outTexCoord;
out vec3 outFragPos;
out vec3 outNormal;

// uniform mat4 model;
uniform mat4 terrainModel;
uniform mat4 view;
uniform mat4 projection;
// uniform mat4 transform;

void main()
{
    // gl_Position = transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    outFragPos = vec3(terrainModel * vec4(aPos.x, aPos.y, aPos.z, 1.0));
    outNormal = mat3(transpose(inverse(terrainModel))) * aNormal;
    gl_Position = projection * view * vec4(outFragPos, 1.0);
    outTexCoord = vec3(aTexCoord.x, aTexCoord.y, aTexID);
}