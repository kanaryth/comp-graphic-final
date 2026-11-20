
#version 410 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform bool hasTexture;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 objectColor;

void main()
{
    vec3 color = objectColor;

    if (hasTexture)
        color = texture(texture_diffuse1, TexCoords).rgb;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);

    vec3 ambient = 0.25 * color;
    vec3 diffuse = diff * color;

    vec3 result = ambient + diffuse;

    FragColor = vec4(result, 1.0);
}
