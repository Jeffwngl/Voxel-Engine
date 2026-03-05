#version 330 core
out vec4 FragColor;

struct Material {
    float ambient;
    float diffuse;
    float specular; // only use a vec3 here is color is needed
    float shineness;
};

struct Light {
    vec3 color;
    vec3 position;
    // vec3 ambient;
    // vec3 diffuse;
    // vec3 specular;
};

in vec3 outTexCoord;
in vec3 outNormal;
in vec3 outFragPos;

// uniform vec3 lightPos;

uniform sampler2DArray textureIDs;

// uniform vec3 lightColor;
uniform vec3 cameraPos;
uniform Material material;
uniform Light light;

void main()
{
    // ambient
    vec3 ambient = material.ambient * light.color;

    //diffuse
    vec3 norm = normalize(outNormal);
    vec3 lightDir = normalize(light.position - outFragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.color * material.diffuse;

    // specular
    float specularStrength = 0.6;
    vec3 viewDir = normalize(cameraPos - outFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shineness);
    vec3 specular = material.specular * spec * light.color;

    // combine
    vec3 lighting = ambient + diffuse + specular;

    vec4 texColor = texture(textureIDs, outTexCoord);
    
    vec3 finalColor = texColor.rgb * lighting;

    FragColor = vec4(finalColor, texColor.a);
}