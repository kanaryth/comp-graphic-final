
#version 410 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    vec3 objectColor = vec3(0.65, 0.68, 0.72);

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    float diffuse = max(dot(norm, lightDir), 0.0);

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float specular = pow(
        max(dot(viewDir, reflectDir), 0.0),
        32.0
    );

    vec3 ambient = 0.2 * objectColor;
    vec3 diffuseColor = diffuse * objectColor;
    vec3 specularColor = 0.4 * specular * vec3(1.0);

    vec3 result = ambient + diffuseColor + specularColor;

    FragColor = vec4(result, 1.0);
}