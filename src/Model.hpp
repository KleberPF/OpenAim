#pragma once

#include "Mesh.hpp"
#include "Shader.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <string>
#include <vector>

class Model
{
public:
    Model(const std::string& path);

    void render(const Shader& shader);

    std::vector<Texture> texturesLoaded;
    std::vector<Mesh> meshes;
    std::string directory;
    bool gammaCorrection;

private:
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat,
                                              aiTextureType type,
                                              const std::string& typeName);
    static GLuint textureFromFile(const std::string& path,
                                  const std::string& directory,
                                  bool gamma = false);
};
