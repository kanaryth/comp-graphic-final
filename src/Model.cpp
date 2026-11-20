
#define GL_SILENCE_DEPRECATION

#include "Model.h"
#include "TextureLoader.h"

#include <iostream>
#include <filesystem>

#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>

namespace fs = std::filesystem;

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

    fs::path modelPath(path);
    directory = modelPath.parent_path().string();

    if (directory.empty())
        directory = ".";

    const aiScene* scene = importer.ReadFile(
        path,
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        aiProcess_CalcTangentSpace |
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
    loadedTextures.clear();

    rootNode = ProcessNode(scene->mRootNode, scene);

    loaded = true;

    std::cout << "Model loaded successfully: " << path << '\n';
    std::cout << "Meshes: " << meshes.size() << '\n';
    std::cout << "Textures loaded: " << loadedTextures.size() << '\n';
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
            vertex.Normal = glm::normalize(glm::vec3(
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z
            ));
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

        if (mesh->HasTangentsAndBitangents())
        {
            vertex.Tangent = glm::normalize(glm::vec3(
                mesh->mTangents[i].x,
                mesh->mTangents[i].y,
                mesh->mTangents[i].z
            ));

            vertex.Bitangent = glm::normalize(glm::vec3(
                mesh->mBitangents[i].x,
                mesh->mBitangents[i].y,
                mesh->mBitangents[i].z
            ));
        }
        else
        {
            glm::vec3 normal = vertex.Normal;

            glm::vec3 up = std::abs(normal.y) < 0.999f
                ? glm::vec3(0.0f, 1.0f, 0.0f)
                : glm::vec3(1.0f, 0.0f, 0.0f);

            vertex.Tangent = glm::normalize(glm::cross(up, normal));
            vertex.Bitangent = glm::normalize(
                glm::cross(normal, vertex.Tangent)
            );
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

    if (mesh->mMaterialIndex < scene->mNumMaterials)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        auto loadTextures = [&](aiTextureType type, const std::string& typeName)
        {
            for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
            {
                aiString texturePath;
                material->GetTexture(type, i, &texturePath);

                std::string relativePath = texturePath.C_Str();

                if (relativePath.empty() || relativePath[0] == '*')
                    continue;

                fs::path originalPath(relativePath);
                fs::path filename = originalPath.filename();

                std::vector<fs::path> candidates = {
                    fs::path(directory) / originalPath,
                    fs::path(directory) / filename,
                    fs::path(directory) / "textures" / filename
                };

                fs::path resolvedPath;
                bool found = false;

                for (const fs::path& candidate : candidates)
                {
                    std::error_code error;

                    if (fs::exists(candidate, error) &&
                        !error &&
                        fs::is_regular_file(candidate, error) &&
                        !error)
                    {
                        resolvedPath = candidate;
                        found = true;
                        break;
                    }
                }

                if (!found)
                {
                    std::cerr << "Texture not found: "
                              << relativePath << '\n';
                    continue;
                }

                std::error_code error;

                fs::path canonicalPath =
                    fs::weakly_canonical(resolvedPath, error);

                std::string fullPath = error
                    ? resolvedPath.string()
                    : canonicalPath.string();

                bool alreadyLoaded = false;

                for (const Texture& texture : loadedTextures)
                {
                    if (texture.path == fullPath &&
                        texture.type == typeName)
                    {
                        textures.push_back(texture);
                        alreadyLoaded = true;
                        break;
                    }
                }

                if (alreadyLoaded)
                    continue;

                unsigned int textureID = LoadTexture(fullPath);

                if (textureID == 0)
                    continue;

                Texture texture;
                texture.id = textureID;
                texture.type = typeName;
                texture.path = fullPath;

                textures.push_back(texture);
                loadedTextures.push_back(texture);

                std::cout << "Loaded texture: "
                          << fullPath << '\n';
            }
        };

        loadTextures(aiTextureType_DIFFUSE, "texture_diffuse");

        loadTextures(aiTextureType_NORMALS, "texture_normal");

        if (material->GetTextureCount(aiTextureType_NORMALS) == 0)
        {
            loadTextures(aiTextureType_HEIGHT, "texture_normal");
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