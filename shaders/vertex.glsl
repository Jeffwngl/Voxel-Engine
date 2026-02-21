#version 330 core
layout (location = 0) in vec3 aPos; // position
layout (location = 1) in vec3 aNormal; // normal
layout (location = 2) in vec2 aTexCoord; // texture

out vec2 outTexCoord;

// uniform mat4 model;
uniform mat4 lightModel;
uniform mat4 view;
uniform mat4 projection;
// uniform mat4 transform;

void main()
{
    // gl_Position = transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    gl_Position = projection * view * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    outTexCoord = vec2(aTexCoord.x, aTexCoord.y);
}