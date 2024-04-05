#pragma once

#include "Material.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <functional>
#include <string>
#include <vector>

class Model
{
public:
    Model(const std::string& path, const Material& material);

    void render(const Shader& shader);

    std::vector<Mesh> meshes;
    std::string directory;
    bool gammaCorrection;

    void setMaterial(const Material& material);

private:
    void processNode(aiNode* node, const aiScene* scene);
    static Mesh processMesh(aiMesh* mesh);

    std::reference_wrapper<const Material> material_;
};
