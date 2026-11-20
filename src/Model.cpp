
#include "Model.h"

#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Model::Model(const std::string& path)
{
    LoadModel(path);
}

void Model::LoadModel(const std::string& path)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(
        path,
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        aiProcess_FlipUVs |
        aiProcess_JoinIdenticalVertices
    );

    if (!scene || !scene->mRootNode ||
        (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE))
    {
        std::cerr << "ASSIMP ERROR: "
                  << importer.GetErrorString() << std::endl;
        return;
    }

    directory = path.substr(0, path.find_last_of("/\\"));

    meshes.clear();

    ProcessNode(
        scene->mRootNode,
        scene,
        glm::mat4(1.0f)
    );

    std::cout << "Model loaded: " << path
              << "\nMeshes: " << meshes.size() << std::endl;
}

void Model::ProcessNode(
    const aiNode* node,
    const aiScene* scene,
    const glm::mat4& parentTransform
)
{
    aiMatrix4x4 a = node->mTransformation;

    glm::mat4 localTransform(
        a.a1, a.b1, a.c1, a.d1,
        a.a2, a.b2, a.c2, a.d2,
        a.a3, a.b3, a.c3, a.d3,
        a.a4, a.b4, a.c4, a.d4
    );

    glm::mat4 transform = parentTransform * localTransform;

    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        unsigned int meshIndex = node->mMeshes[i];

        meshes.push_back(
            ProcessMesh(scene->mMeshes[meshIndex], scene, transform)
        );
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene, transform);
    }
}

Mesh Model::ProcessMesh(
    const aiMesh* mesh,
    const aiScene* scene,
    const glm::mat4& transform
)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    glm::mat3 normalMatrix =
        glm::transpose(glm::inverse(glm::mat3(transform)));

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex{};

        glm::vec4 position(
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z,
            1.0f
        );

        position = transform * position;

        vertex.Position = glm::vec3(position);

        if (mesh->HasNormals())
        {
            glm::vec3 normal(
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z
            );

            vertex.Normal = glm::normalize(normalMatrix * normal);
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
            vertex.TexCoords = glm::vec2(0.0f);
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

    return Mesh(
        std::move(vertices),
        std::move(indices),
        std::move(textures)
    );
}

void Model::Draw(Shader& shader) const
{
    for (const Mesh& mesh : meshes)
    {
        mesh.Draw(shader);
    }
}
