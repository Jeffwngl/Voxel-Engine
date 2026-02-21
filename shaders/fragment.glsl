#version 330 core
out vec4 FragColor;

in vec2 outTexCoord;

uniform sampler2D texture1;
uniform vec3 lightColor;

void main()
{
    float ambientStrength = 0.5;
    vec3 ambient = ambientStrength * lightColor;

    vec4 texColor = texture(texture1, outTexCoord);
    vec3 finalColor = texColor.rgb * ambient;
    FragColor = vec4(finalColor, texColor.a);
    // FragColor = texture(texture1, outTexCoord);
}