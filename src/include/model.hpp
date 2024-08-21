#pragma once

#include <string>
#include <vector>

#include "assimp/scene.h"

class Shader;
class Texture;
class Mesh;

class Model {
public:
    Model(std::string path);
    void Draw(Shader &shader);
private:
    std::vector<Mesh> meshes;
    std::string directory;
    std::vector<Texture> textures_loaded;

    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
    // stb image loading function (the same as in other chapters)
    unsigned int textureFromFile(const char* path);
};