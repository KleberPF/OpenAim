#pragma once

#include "Material.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"

#include <assimp/scene.h>

#include <string>
#include <vector>

class Model {
public:
    Model(const std::string& path);

    void render() const;

private:
    void processNode(aiNode* node, const aiScene* scene);
    static Mesh processMesh(aiMesh* mesh);

    std::vector<Mesh> m_meshes;
    std::string m_directory;
    bool m_gammaCorrection;
};
