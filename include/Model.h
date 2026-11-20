
#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <assimp/scene.h>

#include "Mesh.h"
#include "shader.h"

class Model
{
public:
    Model() = default;
    explicit Model(const std::string& path);

    void LoadModel(const std::string& path);
    void Draw(Shader& shader) const;

private:
    std::vector<Mesh> meshes;
    std::string directory;

    void ProcessNode(
        const aiNode* node,
        const aiScene* scene,
        const glm::mat4& parentTransform
    );

    Mesh ProcessMesh(
        const aiMesh* mesh,
        const aiScene* scene,
        const glm::mat4& transform
    );
};
