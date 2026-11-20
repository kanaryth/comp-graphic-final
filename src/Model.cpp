
#define GL_SILENCE_DEPRECATION

#include "Model.h"

#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

Model::Model(const std::string& path)
{
    loaded = false;
    LoadModel(path);
}

bool Model::IsLoaded() const
{
    return loaded;
}

void Model::LoadModel(const std::string& path)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(
        path,
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        aiProcess_JoinIdenticalVertices |
        aiProcess_ImproveCacheLocality
    );

    if (!scene || !scene->mRootNode ||
        (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE))
    {
        std::cerr << "ASSIMP ERROR: "
                  << importer.GetErrorString() << '\n';
        return;
    }

    meshes.clear();

    rootNode = ProcessNode(scene->mRootNode, scene);

    loaded = true;

    std::cout << "Model loaded successfully: " << path << '\n';
    std::cout << "Meshes: " << meshes.size() << '\n';
}

ModelNode Model::ProcessNode(
    aiNode* node,
    const aiScene* scene
)
{
    ModelNode result;

    result.transform = ConvertMatrix(node->mTransformation);

    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        unsigned int meshIndex = node->mMeshes[i];

        result.meshIndices.push_back(
            ProcessMesh(scene->mMeshes[meshIndex], scene)
        );
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        result.children.push_back(
            ProcessNode(node->mChildren[i], scene)
        );
    }

    return result;
}

unsigned int Model::ProcessMesh(
    aiMesh* mesh,
    const aiScene* scene
)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;

        vertex.Position = glm::vec3(
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z
        );

        if (mesh->HasNormals())
        {
            vertex.Normal = glm::vec3(
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z
            );
        }
        else
        {
            vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
        }

        if (mesh->mTextureCoords[0])
        {
            vertex.TexCoords = glm::vec2(
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y
            );
        }
        else
        {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        const aiFace& face = mesh->mFaces[i];

        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    meshes.emplace_back(vertices, indices, textures);

    return static_cast<unsigned int>(meshes.size() - 1);
}

glm::mat4 Model::ConvertMatrix(const aiMatrix4x4& m)
{
    glm::mat4 result(1.0f);

    result[0][0] = m.a1;
    result[1][0] = m.a2;
    result[2][0] = m.a3;
    result[3][0] = m.a4;

    result[0][1] = m.b1;
    result[1][1] = m.b2;
    result[2][1] = m.b3;
    result[3][1] = m.b4;

    result[0][2] = m.c1;
    result[1][2] = m.c2;
    result[2][2] = m.c3;
    result[3][2] = m.c4;

    result[0][3] = m.d1;
    result[1][3] = m.d2;
    result[2][3] = m.d3;
    result[3][3] = m.d4;

    return result;
}

void Model::DrawNode(
    const ModelNode& node,
    const Shader& shader,
    const glm::mat4& parentTransform
)
{
    glm::mat4 globalTransform = parentTransform * node.transform;

    for (unsigned int meshIndex : node.meshIndices)
    {
        meshes[meshIndex].Draw(shader, globalTransform);
    }

    for (const ModelNode& child : node.children)
    {
        DrawNode(child, shader, globalTransform);
    }
}

void Model::Draw(
    const Shader& shader,
    const glm::mat4& transform
)
{
    if (!loaded)
        return;

    DrawNode(rootNode, shader, transform);
}