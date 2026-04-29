#version 330 core
out vec4 FragColor;

in vec3 viewDir;

uniform vec3 sunDir;
uniform vec3 betaRayleigh;
uniform vec3 betaMie;
uniform float g;
uniform float Esun;

const float pi = 3.14159265;
const float atmosphereHeight = 100.0; // tweak to taste

float rayleighPhase(float cosTheta) {
    return (3.0 / (16.0 * pi)) * (1.0 + cosTheta * cosTheta);
}

float miePhase(float cosTheta) {
    float g2 = g * g;
    return (1.0 - g2) / (4.0 * pi * pow(1.0 + g2 - 2.0 * g * cosTheta, 1.5));
}

void main() {
    vec3 dir = normalize(viewDir);

    // Path length through atmosphere — longer near horizon
    float s = atmosphereHeight / max(abs(dir.y), 0.001);

    float cosTheta = dot(dir, normalize(sunDir));

    // Extinction
    vec3 betaEx = betaRayleigh + betaMie;
    vec3 fex = exp(-betaEx * s);

    // In-scattering
    vec3 inscatter = betaRayleigh * rayleighPhase(cosTheta) + betaMie * miePhase(cosTheta);
    vec3 lin = Esun * inscatter * (1.0 - fex) / betaEx;

    // Sun disk
    float sunDisk = smoothstep(0.998, 1.0, cosTheta);
    vec3 sun = sunDisk * Esun * fex * vec3(1.0, 0.9, 0.7);

    // Fade out below horizon
    float horizon = smoothstep(-0.05, 0.05, dir.y);
    vec3 skyColor = (lin + sun) * horizon;

    FragColor = vec4(skyColor, 1.0);
}