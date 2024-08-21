#include "mesh.hpp"

#include "shader.hpp"
#include "glad/glad.h"

Mesh::Mesh(std::vector<Vertex> inVertices, std::vector<unsigned int> inIndices, std::vector<Texture> inTextures) {
    vertices = inVertices;
    indices = inIndices;
    textures = inTextures;

    setup();
}

void Mesh::setup() {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // load all vertices to GPU
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) (vertices.size() * sizeof(Vertex)), &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // load all indices to GPU
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr) (indices.size() * sizeof(unsigned int)), &indices[0], GL_STATIC_DRAW);

    // set vertex attributes. this is easier now, using the Vertex struct. the
    // offsetof() function trivializes previous pointer arithmetics.
    // position (3 floats)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) 0);
    glEnableVertexAttribArray(0);

    // normals (3 floats)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    // texture coordinates (2 floats)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(2);

    // unbinds VAO
    glBindVertexArray(0);
}

// , ,
// sets the right texture unit in the material uniforms, binds the VAO
// and draws the mesh.
void Mesh::Draw(Shader& shader) {
    // To set textures to their correct texture units, we use
    // a simple convention. Every texture is set to the shader
    // as, for example, "material.texture_diffuse0". the number
    // can go from 0 to the texture unit max and in our case,
    // the possible texture types are only diffuse and specular.
    unsigned int diffuseCount = 0;
    unsigned int specularCount = 0;
    for (unsigned int i = 0 ; i < textures.size() ; i++) {
        std::string number;
        std::string type = textures[i].type;
        if (type == "texture_diffuse")
            number = std::to_string(diffuseCount++);
        else if (type == "texture_specular")
            number = std::to_string(specularCount++);

        // set the Nth texture unit to the shader uniform.
        // our shader supports just 1 texture of each type, but it
        // can be extended to support more using this convention.
        shader.setInt(("material." + type + number).c_str(), (int) i);
        // bind the Nth texture to the Nth texture unit
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    // draw the mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}