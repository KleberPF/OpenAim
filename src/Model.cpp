#include "Model.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <stb_image.h>

#include <iostream>

Model::Model(
    const std::string& path, const Material& material, const Shader& shader)
    : material(material)
    , shader(shader)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs
            | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE
        || !scene->mRootNode) {
        throw std::string("ERROR::ASSIMP:: ") + importer.GetErrorString();
        return;
    }

    m_directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene);
}

void Model::render() const
{
    for (const auto& mesh : m_meshes) {
        mesh.render();
    }
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        // the node object only contains indices to index the actual objects in
        // the scene. the scene contains all the data, node is just to keep
        // stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        m_meshes.push_back(Model::processMesh(mesh));
    }
    // after we've processed all of the meshes (if any) we then recursively
    // process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh)
{
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;

        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;

        if (mesh->HasNormals()) {
            vertex.normal.x = mesh->mNormals[i].x;
            vertex.normal.y = mesh->mNormals[i].y;
            vertex.normal.z = mesh->mNormals[i].z;
        }

        // does the mesh contain texture coordinates?
        if (mesh->mTextureCoords[0]) {
            // a vertex can contain up to 8 different texture coordinates. We
            // thus make the assumption that we won't use models where a vertex
            // can have multiple texture coordinates so we always take the first
            // set (0).
            vertex.texCoords.x = mesh->mTextureCoords[0][i].x;
            vertex.texCoords.y = mesh->mTextureCoords[0][i].y;

            vertex.tangent.x = mesh->mTangents[i].x;
            vertex.tangent.y = mesh->mTangents[i].y;
            vertex.tangent.z = mesh->mTangents[i].z;

            vertex.bitangent.x = mesh->mBitangents[i].x;
            vertex.bitangent.y = mesh->mBitangents[i].y;
            vertex.bitangent.z = mesh->mBitangents[i].z;
        } else {
            vertex.texCoords = glm::vec2(0.0, 0.0);
        }

        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    return { vertices, indices };
}
