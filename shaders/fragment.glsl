
#version 410 core

in vec3 FragPos;
in vec2 TexCoords;
in mat3 TBN;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;

uniform int hasDiffuseTexture;
uniform int hasNormalTexture;

void main()
{
    vec3 objectColor = vec3(0.65, 0.68, 0.72);

    if (hasDiffuseTexture == 1)
    {
        objectColor = texture(texture_diffuse1, TexCoords).rgb;
    }

    vec3 normal = normalize(TBN[2]);

    if (hasNormalTexture == 1)
    {
        vec3 normalMap = texture(texture_normal1, TexCoords).rgb;
        normalMap = normalize(normalMap * 2.0 - 1.0);

        normal = normalize(TBN * normalMap);
    }

    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);

    float diffuse = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);

    float specular = pow(
        max(dot(viewDir, reflectDir), 0.0),
        32.0
    );

    vec3 ambient = 0.2 * objectColor;
    vec3 diffuseColor = diffuse * objectColor;
    vec3 specularColor = 0.3 * specular * vec3(1.0);

    vec3 result = ambient + diffuseColor + specularColor;

    FragColor = vec4(result, 1.0);
}