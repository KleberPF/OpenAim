#include "Model.hpp"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"

#include <stb_image.h>

#include <iostream>

Model::Model(const std::string& path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        path, aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                  aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
        !scene->mRootNode)
    {
        throw std::string("ERROR::ASSIMP:: ") + importer.GetErrorString();
        return;
    }

    this->directory = path.substr(0, path.find_last_of('/'));
    this->processNode(scene->mRootNode, scene);
}

void Model::render(const Shader& shader)
{
    for (auto& mesh : this->meshes)
    {
        mesh.render(shader);
    }
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene.
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;

        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;

        if (mesh->HasNormals())
        {
            vertex.normal.x = mesh->mNormals[i].x;
            vertex.normal.y = mesh->mNormals[i].y;
            vertex.normal.z = mesh->mNormals[i].z;
        }

        // does the mesh contain texture coordinates?
        if (mesh->mTextureCoords[0])
        {
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vertex.texCoords.x = mesh->mTextureCoords[0][i].x;
            vertex.texCoords.y = mesh->mTextureCoords[0][i].y;

            vertex.tangent.x = mesh->mTangents[i].x;
            vertex.tangent.y = mesh->mTangents[i].y;
            vertex.tangent.z = mesh->mTangents[i].z;

            vertex.bitangent.x = mesh->mBitangents[i].x;
            vertex.bitangent.y = mesh->mBitangents[i].y;
            vertex.bitangent.z = mesh->mBitangents[i].z;
        }
        else
        {
            vertex.texCoords = glm::vec2(0.0, 0.0);
        }

        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    std::vector<Texture> diffuseMaps = this->loadMaterialTextures(
        material, aiTextureType_DIFFUSE, "textureDiffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

    std::vector<Texture> specularMaps = this->loadMaterialTextures(
        material, aiTextureType_SPECULAR, "textureSpecular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

    std::vector<Texture> normalMaps = this->loadMaterialTextures(
        material, aiTextureType_HEIGHT, "textureNormal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

    std::vector<Texture> heightMaps = this->loadMaterialTextures(
        material, aiTextureType_AMBIENT, "textureHeight");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    return {vertices, indices, textures};
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat,
                                                 aiTextureType type,
                                                 const std::string& typeName)
{
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;

        for (auto& texture : this->texturesLoaded)
        {
            if (std::strcmp(texture.path.data(), str.C_Str()) == 0)
            {
                textures.push_back(texture);
                skip = true;
                break;
            }
        }
        if (!skip)
        {
            Texture texture;
            texture.id = Model::textureFromFile(str.C_Str(), this->directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            this->texturesLoaded.push_back(texture);
        }
    }
    return textures;
}

GLuint Model::textureFromFile(const std::string& path,
                              const std::string& directory, bool /*gamma*/)
{
    std::string filename = directory + '/' + path;

    unsigned int textureId = 0;
    glGenTextures(1, &textureId);

    int width = 0;
    int height = 0;
    int nrComponents = 0;
    unsigned char* data =
        stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        // default and when nrComponents == 1
        GLenum format = GL_RED;
        if (nrComponents == 3)
        {
            format = GL_RGB;
        }
        else if (nrComponents == 4)
        {
            format = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                     GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        std::cerr << "Texture failed to load at path: " << path << '\n';
    }

    stbi_image_free(data);
    return textureId;
}