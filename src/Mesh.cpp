
#define GL_SILENCE_DEPRECATION

#include "Mesh.h"

#include <cstddef>
#include <string>

Mesh::Mesh(
    const std::vector<Vertex>& vertices,
    const std::vector<unsigned int>& indices,
    const std::vector<Texture>& textures
)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    SetupMesh();
}

void Mesh::SetupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(Vertex),
        vertices.data(),
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(unsigned int),
        indices.data(),
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, Position)
    );
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, Normal)
    );
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(
        2, 2, GL_FLOAT, GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, TexCoords)
    );
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Mesh::Draw(const Shader& shader, const glm::mat4& transform)
{
    shader.SetMat4("model", transform);

    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;

    for (unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);

        std::string number;
        std::string name = textures[i].type;

        if (name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if (name == "texture_specular")
            number = std::to_string(specularNr++);
        else if (name == "texture_normal")
            number = std::to_string(normalNr++);
        else if (name == "texture_height")
            number = std::to_string(heightNr++);

        shader.SetInt(name + number, i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    glBindVertexArray(VAO);

    glDrawElements(
        GL_TRIANGLES,
        static_cast<GLsizei>(indices.size()),
        GL_UNSIGNED_INT,
        nullptr
    );

    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
}