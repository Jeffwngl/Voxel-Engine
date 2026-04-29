#version 330 core
layout (location = 0) in vec3 aPos; // position
layout (location = 1) in vec3 aNormal; // normal
layout (location = 2) in vec2 aTexCoord; // texture
layout (location = 3) in float aTexID;

uniform mat4 terrainModel;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 cameraPos;
uniform vec3 sunDir; // normalized direction to sun
uniform vec3 betaRayleigh; // (6.95e-6, 1.18e-5, 2.44e-5)
uniform vec3 betaMie; // haze scattering coefficient
uniform float g; // Henyey Greenstein factor
uniform float Esun; // sun intensity

out vec3 outTexCoord;
out vec3 outFragPos;
out vec3 outNormal;
out vec3 outFex; // extinction factor
out vec3 outLin; // in scattering

// // uniform mat4 model;
// // uniform mat4 transform;

const float pi = 3.14159;

// atmospheric scattering
vec3 computeFex(float s) {
    vec3 betaEx = betaRayleigh + betaMie;
    return exp(-betaEx * s);
}

// Rayleigh scattering
float rayleighPhase(float cosTheta) {
    return (3.0 / (16.0 * pi)) * (1.0 + cosTheta * cosTheta);
}

float miePhase(float cosTheta) {
    float g2 = g * g;
    return (1.0 - g2) / (4.0 * pi * pow(1.0 + g2 - 2.0 * g * cosTheta, 1.5));
}


void main()
{
    vec4 worldPos = terrainModel * vec4(aPos, 1.0);
    outFragPos = worldPos.xyz;
    outNormal = mat3(transpose(inverse(terrainModel))) * aNormal;
    outTexCoord = vec3(aTexCoord.x, aTexCoord.y, aTexID);
    gl_Position = projection * view * worldPos;

    // Atmospheric scattering
    float s = length(outFragPos - cameraPos);
    float cosTheta = dot(normalize(outFragPos - cameraPos), sunDir);

    outFex = computeFex(s);

    vec3 betaEx = betaRayleigh + betaMie;
    vec3 inscatter = betaRayleigh * rayleighPhase(cosTheta)
                   + betaMie * miePhase(cosTheta);
    outLin = Esun * inscatter * (1.0 - outFex) / betaEx;
    
    // // gl_Position = transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    // outFragPos = vec3(terrainModel * vec4(aPos.x, aPos.y, aPos.z, 1.0));
    // outNormal = mat3(transpose(inverse(terrainModel))) * aNormal;
    // gl_Position = projection * view * vec4(outFragPos, 1.0);
    // outTexCoord = vec3(aTexCoord.x, aTexCoord.y, aTexID);
}

/* ---------------------- Debugging ---------------------- */

// #version 330 core
// layout (location = 0) in vec3 aPos;

// uniform mat4 terrainModel;
// uniform mat4 view;
// uniform mat4 projection;

// void main() {
//     gl_Position = projection * view * terrainModel * vec4(aPos, 1.0);
// }