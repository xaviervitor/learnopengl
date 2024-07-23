#include "shader.hpp"

#include <cstdio>
#include "filestring.hpp"

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
    // store paths for debugging
    this->vertexSourcePath = vertexPath;
    this->fragmentSourcePath = fragmentPath;

    // read shader source files
    FileString vertexShaderFile = FileString(vertexPath);
    FileString fragmentShaderFile = FileString(fragmentPath);

    unsigned int vertexShader, fragmentShader;

    // vertex shader compilation and error checking
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, vertexShaderFile.getStringAddress(), NULL);
    glCompileShader(vertexShader);
    checkShaderCompileErrors(vertexShader, this->vertexSourcePath);

    // fragment shader compilation and error checking
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, fragmentShaderFile.getStringAddress(), NULL);
    glCompileShader(fragmentShader);
    checkShaderCompileErrors(fragmentShader, this->fragmentSourcePath);

    // shader program linking and error checking
    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);
    checkProgramLinkErrors(ID);

    // the already compiled and linked shaders can be deleted
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::use() {
    glUseProgram(ID);
}

void Shader::setBool(const char* name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name), (int) value);
}

void Shader::setInt(const char* name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name), value);
}

void Shader::setFloat(const char* name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name), value);
}

void Shader::setVec2(const char* name, const glm::vec2 &value) const {
    glUniform2fv(glGetUniformLocation(ID, name), 1, &value[0]);
}

void Shader::setVec2(const char* name, float x, float y) const {
    glUniform2f(glGetUniformLocation(ID, name), x, y);
}

void Shader::setVec3(const char* name, const glm::vec3 &value) const {
    glUniform3fv(glGetUniformLocation(ID, name), 1, &value[0]);
}

void Shader::setVec3(const char* name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(ID, name), x, y, z);
}

void Shader::setVec4(const char* name, const glm::vec4 &value) const {
    glUniform4fv(glGetUniformLocation(ID, name), 1, &value[0]);
}

void Shader::setVec4(const char* name, float x, float y, float z, float w) const {
    glUniform4f(glGetUniformLocation(ID, name), x, y, z, w);
}

void Shader::setMat2(const char* name, const glm::mat2 &mat) const {
    glUniformMatrix2fv(glGetUniformLocation(ID, name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat3(const char* name, const glm::mat3 &mat) const {
    glUniformMatrix3fv(glGetUniformLocation(ID, name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const char* name, const glm::mat4 &mat) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::checkShaderCompileErrors(unsigned int shader, const char* path) {
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        const int bufSize = 1024;
        char infoLog[bufSize];
        glGetShaderInfoLog(shader, bufSize, NULL, infoLog);
        printf("Shader compilation error\nPath: %s\n%s", path, infoLog);
    }
}

void Shader::checkProgramLinkErrors(unsigned int program) {
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        const int bufSize = 1024;
        char infoLog[bufSize];
        glGetProgramInfoLog(program, bufSize, NULL, infoLog);
        printf("Shader linking error\nPath vertex: %s\nPath fragment: %s\n%s\n", this->vertexSourcePath, this->fragmentSourcePath, infoLog);
    }
}