#version 330 core
layout (location = 0) in vec3 aPos; // position
layout (location = 1) in vec3 aColor; // color

out vec3 outColor;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    outColor = aColor; // aColor is input color into vertex shader
}