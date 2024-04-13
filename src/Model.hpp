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
    Model(const std::string& path, const Material& material,
          const Shader& shader);

    void render() const;

    std::vector<Mesh> meshes;
    std::string directory;
    bool gammaCorrection;

    const Material& getMaterial() const;
    void setMaterial(const Material& material);

    const Shader& getShader() const;
    void setShader(const Shader& shader);

private:
    void processNode(aiNode* node, const aiScene* scene);
    static Mesh processMesh(aiMesh* mesh);

    std::reference_wrapper<const Material> material_;
    std::reference_wrapper<const Shader> shader_;
};
