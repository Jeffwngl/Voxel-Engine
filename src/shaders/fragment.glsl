#version 330 core
out vec4 FragColor;

struct Material {
    float ambient;
    float diffuse;
    float specular; // only use a vec3 here is color is needed
    float shininess;
};

struct Light {
    vec3 color;
    vec3 position; // treated as directional light
};

struct Fog {
    float fogStart;
    float fogEnd;
    vec3 fogColor;
};

in vec3 outTexCoord;
in vec3 outNormal;
in vec3 outFragPos;
in vec3 outFex;
in vec3 outLin;

uniform sampler2DArray textureIDs;

// uniform vec3 lightColor;
uniform vec3 cameraPos;
uniform Material material;
uniform Light light;
uniform Fog fog;

void main()
{
    // ambient
    vec3 ambient = material.ambient * light.color;

    //diffuse
    vec3 norm = normalize(outNormal);
    vec3 lightDir = normalize(light.position);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.color * material.diffuse;

    // specular
    float specularStrength = 0.6;
    vec3 viewDir = normalize(cameraPos - outFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = material.specular * spec * light.color;

    // fog
    float dist = length(outFragPos - cameraPos);
    float fogFactor = clamp((dist - fog.fogStart) / (fog.fogEnd - fog.fogStart), 0.0, 1.0);

    // combine
    vec3 lighting = ambient + diffuse + specular;

    vec4 texColor = texture(textureIDs, outTexCoord);

    // atmosphere effects
    vec3 litColor = texColor.rgb * lighting; // change

    // atmosphere
    vec3 atmosColor = litColor * outFex + outLin;
    
    vec3 finalColor = mix(atmosColor, fog.fogColor, fogFactor);

    FragColor = vec4(finalColor, texColor.a);
}

/* ---------------------- Debugging ---------------------- */

// #version 330 core
// out vec4 FragColor;

// struct Material {
//     float ambient;
//     float diffuse;
//     float specular; // only use a vec3 here is color is needed
//     float shininess;
// };

// struct Light {
//     vec3 color;
//     vec3 position;
// };

// in vec3 outTexCoord;
// in vec3 outNormal;
// in vec3 outFragPos;

// // uniform vec3 lightPos;

// uniform sampler2DArray textureIDs;

// // uniform vec3 lightColor;
// uniform vec3 cameraPos;
// uniform Material material;
// uniform Light light;

// // void main() {
// //     FragColor = vec4(1.0, 0.0, 0.0, 1.0); // solid red
// // }

// void main() {
//     // Show raw normal as color to test geometry
//     FragColor = vec4(normalize(outNormal) * 0.5 + 0.5, 1.0);
// }

// #version 330 core
// out vec4 FragColor;

// struct Material {
//     float ambient;
//     float diffuse;
//     float specular;
//     float shininess;
// };

// struct Light {
//     vec3 color;
//     vec3 position;
// };

// in vec3 outTexCoord;
// in vec3 outNormal;
// in vec3 outFragPos;

// uniform sampler2DArray textureIDs;
// uniform vec3 cameraPos;
// uniform Material material;
// uniform Light light;

// // void main() {
// //     FragColor = vec4(light.color, 1.0);
// // }

// void main() {
//     // ambient
//     vec3 ambient = material.ambient * light.color;

//     // diffuse
//     vec3 norm = normalize(outNormal);
//     vec3 lightDir = normalize(light.position - outFragPos);
//     float diff = max(dot(norm, lightDir), 0.0);
//     vec3 diffuse = diff * light.color * material.diffuse;

//     // specular
//     vec3 viewDir = normalize(cameraPos - outFragPos);
//     vec3 reflectDir = reflect(-lightDir, norm);
//     float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
//     vec3 specular = material.specular * spec * light.color;

//     // combine
//     vec3 lighting = ambient + diffuse + specular;
//     vec4 texColor = texture(textureIDs, outTexCoord);
//     FragColor = vec4(texColor.rgb * lighting, 1.0);
// }