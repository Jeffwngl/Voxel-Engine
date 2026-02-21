#version 330 core
out vec4 FragColor;

in vec2 outTexCoord;
in vec3 outNormal;
in vec3 outFragPos;

uniform vec3 lightPos;
uniform sampler2D texture1;
uniform vec3 lightColor;
uniform vec3 cameraPos;

void main()
{
    // ambient
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;

    //diffuse
    vec3 norm = normalize(outNormal);
    vec3 lightDir = normalize(lightPos - outFragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    float specularStrength = 0.6;
    vec3 viewDir = normalize(cameraPos - outFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32); // last value = shineness
    vec3 specular = specularStrength * spec * lightColor;

    // combine
    vec3 lighting = ambient + diffuse + specular;

    vec4 texColor = texture(texture1, outTexCoord);
    vec3 finalColor = texColor.rgb * lighting;

    FragColor = vec4(finalColor, texColor.a);
}