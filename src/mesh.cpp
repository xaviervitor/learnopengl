#include "mesh.hpp"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) {
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    this->setup();
}

void Mesh::setup() {
    glGenVertexArrays(1, &this->VAO);
    glBindVertexArray(this->VAO);

    glGenBuffers(1, &this->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    // load all vertices to GPU
    glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &this->EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    // load all indices to GPU
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

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
void Mesh::Draw(Shader &shader) {
    // To set textures to their correct texture units, we use
    // a simple convention. Every texture is set to the shader
    // as, for example, "material.texture_diffuse0". the number
    // can go from 0 to the texture unit max and in our case,
    // the possible texture types are only diffuse and specular.
    unsigned int diffuseCount = 0;
    unsigned int specularCount = 0;
    for (unsigned int i = 0 ; i < this->textures.size() ; i++) {
        std::string number;
        std::string type = this->textures[i].type;
        if (type == "texture_diffuse")
            number = std::to_string(diffuseCount++);
        else if (type == "texture_specular")
            number = std::to_string(specularCount++);

        // set the Nth texture unit to the shader uniform.
        // our shader supports just 1 texture of each type, but it
        // can be extended to support more using this convention.
        shader.setInt(("material." + type + number).c_str(), i);
        // bind the Nth texture to the Nth texture unit
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
    }

    // draw the mesh
    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}