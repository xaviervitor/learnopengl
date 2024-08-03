#pragma once

#include "glm/glm.hpp"
#include <string>
#include <vector>
#include "shader.hpp"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

struct Texture {
    unsigned int id;
    // used to index textures to texture units. in our case,
    // can only be "texture_diffuse" or "texture_specular"
    std::string type;
    // used to skip texture loading if a texture with the same
    // path and name is already loaded
    std::string path;
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    Mesh(std::vector<Vertex> inVertices, std::vector<unsigned int> inIndices, std::vector<Texture> inTextures);
    void Draw(Shader &shader);
private:
    unsigned int VAO, VBO, EBO;
    void setup();
};