#version 330 core
layout (location = 0) in vec3 aPos; // position
// layout (location = 1) in vec3 aColor; // color
layout (location = 2) in vec2 aTexCoord; // texture

// out vec3 outColor; // use for color instead of texture
out vec2 outTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
// uniform mat4 transform;

void main()
{
    // gl_Position = transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    // outColor = aColor; // aColor is input color into vertex shader
    outTexCoord = vec2(aTexCoord.x, aTexCoord.y);
}