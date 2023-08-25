#version 330 core

layout (location = 0) in vec3 aPos;

// the normal vector is passed to the vertex shader
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// the interpolated position and normal vector of the fragment
// need to be passed in an out variable
out vec3 FragPos;
out vec3 Normal;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    
    // the position and normal vectors need to be in world
    // space. a simple model matrix multiplication is enough for
    // the position vector, 
    FragPos = vec3(model * vec4(aPos, 1.0));
    // the normal vector needs to account for the non-uniform scaling
    // of the model matrix. to transform a normal to world space,
    // we use a "normal matrix", the transpose of the inverse of the
    // upper-left 3x3 part of the model matrix.
    Normal = mat3(transpose(inverse(model))) * aNormal;
}