
#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec3 FragPos;
out vec2 TexCoords;
out mat3 TBN;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 worldPosition = model * vec4(aPos, 1.0);

    FragPos = worldPosition.xyz;
    TexCoords = aTexCoords;

    mat3 normalMatrix = transpose(inverse(mat3(model)));

    vec3 N = normalize(normalMatrix * aNormal);
    vec3 T = normalize(mat3(model) * aTangent);

    T = normalize(T - dot(T, N) * N);

    vec3 B = normalize(cross(N, T));

    if (dot(B, mat3(model) * aBitangent) < 0.0)
    {
        B = -B;
    }

    TBN = mat3(T, B, N);

    gl_Position = projection * view * worldPosition;
}