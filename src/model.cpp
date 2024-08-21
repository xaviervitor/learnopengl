#include "model.hpp"

#include <cstdio>

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "stb/stb_image.h"

#include "mesh.hpp"
#include "shader.hpp"

Model::Model(std::string path) {
    this->loadModel(path);
}

void Model::Draw(Shader &shader) {
    for (unsigned int i = 0 ; i < this->meshes.size() ; i++) {
        this->meshes[i].Draw(shader);
    }
}

void Model::loadModel(std::string path) {
    Assimp::Importer importer;
    // Triangulates the mesh because we only use the GL_TRIANGLES primitive
    // in our glDrawElements calls. Flips UVs because OpenGL expects images
    // to have their 0.0 coordinates at the bottom.
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
        printf("Assimp model loading error\nPath: %s\n%s", path.c_str(), importer.GetErrorString());
        return;
    }

    // stores only the directory to append with the filename returned by
    // GetTexture() to compare with a loaded texture.
    this->directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene) {
    // process all meshes in current node
    for (unsigned int i = 0 ; i < node->mNumMeshes ; i++) {
        unsigned int meshIndex = node->mMeshes[i];
        aiMesh* mesh = scene->mMeshes[meshIndex];
        this->meshes.push_back(processMesh(mesh, scene));
    }

    // process children recursively
    for (unsigned int i = 0 ; i < node->mNumChildren ; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // move all vertex data to glm vectors
    for (unsigned int i = 0 ; i < mesh->mNumVertices ; i++) {
        Vertex vertex;
        glm::vec3 vector;
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.position = vector;

        if(mesh->HasNormals()) {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.normal = vector;
        }

        if (mesh->mTextureCoords[0]) {
            vector.x = mesh->mTextureCoords[0][i].x;
            vector.y = mesh->mTextureCoords[0][i].y;
            vertex.texCoords = glm::vec2(vector);
        } else
            vertex.texCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }

    for (unsigned int i = 0 ; i < mesh->mNumFaces ; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0 ; j < face.mNumIndices ; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // load all textures and store in the textures array
    if (scene->mNumMaterials > 0) {
        // scene->mMaterials is of size scene->mNumMaterials
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuseTextures = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseTextures.begin(), diffuseTextures.end());
        std::vector<Texture> specularTextures = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularTextures.begin(), specularTextures.end());
    }

    return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* material, aiTextureType textureType, std::string textureTypeName) {
    std::vector<Texture> textures;
    for (unsigned int i = 0 ; i < material->GetTextureCount(textureType) ; i++) {
        aiString aiFilename;
        material->GetTexture(textureType, i, &aiFilename);
        const char* filename = aiFilename.C_Str();
        // texture loading skip logic
        bool skip = false;
        for (unsigned int j = 0 ; j < this->textures_loaded.size() ; j++) {
            bool textureAlreadyLoaded = std::strcmp(filename, this->textures_loaded[j].path.data()) == 0;
            if (textureAlreadyLoaded) {
                // just assign the loaded texture to the textures array
                // instead of loading again from the file
                textures.push_back(this->textures_loaded[j]);
                skip = true;
                break;
            }
        }

        // texture loading
        if (!skip) {
            Texture texture;
            std::string path = this->directory + '/' + std::string(filename);
            texture.id = this->textureFromFile(path.c_str());
            texture.type = textureTypeName;
            texture.path = filename;
            textures.push_back(texture);
            this->textures_loaded.push_back(texture);
        }
    }
    return textures;
}

unsigned int Model::textureFromFile(const char* path) {
    unsigned int id;
    glGenTextures(1, &id);

    int width, height, channels;
    unsigned char* data = stbi_load(path,
        &width, &height, &channels, 0);

    if (data != NULL) {
        GLenum format = 0;
        if (channels == 1)
            format = GL_RED;
        else if (channels == 3)
            format = GL_RGB;
        else if (channels == 4)
            format = GL_RGBA;

        if (format != 0) {
            glBindTexture(GL_TEXTURE_2D, id);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexImage2D(GL_TEXTURE_2D, 0, (GLint) format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            printf("Texture load failed\nPath: %s\n", path);
        }
    } else {
        printf("Texture load failed\nPath: %s\n", path);
    }
    stbi_image_free(data);

    return id;
}