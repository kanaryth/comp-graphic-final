
#ifndef MESH_H
#define MESH_H

#include <vector>
#include <string>

#include <OpenGL/gl3.h>
#include <glm/glm.hpp>

#include "shader.h"

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};

struct Texture
{
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh
{
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    unsigned int VAO;

    Mesh(
        const std::vector<Vertex>& vertices,
        const std::vector<unsigned int>& indices,
        const std::vector<Texture>& textures
    );

    void Draw(const Shader& shader, const glm::mat4& transform);

private:
    unsigned int VBO;
    unsigned int EBO;

    void SetupMesh();
};

#endif