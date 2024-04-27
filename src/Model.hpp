#pragma once

#include "Material.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"

#include <assimp/scene.h>

#include <string>
#include <vector>

class Model {
public:
    Model(const std::string& path, const Material& material,
        const Shader& shader);

    void render() const;

    std::reference_wrapper<const Material> material;
    std::reference_wrapper<const Shader> shader;

private:
    void processNode(aiNode* node, const aiScene* scene);
    static Mesh processMesh(aiMesh* mesh);

    std::vector<Mesh> m_meshes;
    std::string m_directory;
    bool m_gammaCorrection;
};
