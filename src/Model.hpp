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

    const Material& getMaterial() const;
    void setMaterial(const Material& material);

    const Shader& getShader() const;
    void setShader(const Shader& shader);

private:
    void processNode(aiNode* node, const aiScene* scene);
    static Mesh processMesh(aiMesh* mesh);

    std::vector<Mesh> m_meshes;
    std::string m_directory;
    bool m_gammaCorrection;

    std::reference_wrapper<const Material> m_material;
    std::reference_wrapper<const Shader> m_shader;
};
