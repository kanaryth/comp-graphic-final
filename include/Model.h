
#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "Mesh.h"
#include <assimp/matrix4x4.h>
#include <assimp/scene.h>

struct ModelNode
{
    glm::mat4 transform;
    std::vector<unsigned int> meshIndices;
    std::vector<ModelNode> children;
};

class Model
{
public:
    Model(const std::string& path);

    void Draw(const Shader& shader, const glm::mat4& transform);

    bool IsLoaded() const;

private:
    std::vector<Mesh> meshes;
    ModelNode rootNode;
    bool loaded;

    void LoadModel(const std::string& path);

    ModelNode ProcessNode(
        aiNode* node,
        const aiScene* scene
    );

    unsigned int ProcessMesh(
        aiMesh* mesh,
        const aiScene* scene
    );

    void DrawNode(
        const ModelNode& node,
        const Shader& shader,
        const glm::mat4& parentTransform
    );

    glm::mat4 ConvertMatrix(const aiMatrix4x4& matrix);
};

#endif