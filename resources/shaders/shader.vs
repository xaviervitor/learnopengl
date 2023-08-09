#version 330 core

// the "in" variables of the vertex shader are its
// vertex attributes, as set with glVertexAttribPointer.
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

// these variables will be passed to the fragment shader, or
// the geometry shader, if there's one. the "out" variables
// of the vertex shader have to be of the same name and type
// as the variables in the next shader.
out vec3 ourColor;
out vec2 TexCoord;

void main() {
    ourColor = aColor;
    TexCoord = aTexCoord;
    
    // "gl_Position" is a predefined variable of glsl. the goal
    // of a vertex shader is to define a position for every vertex,
    // so somewhere in a vertex shader main function, the gl_Position
    // has to be set.
    gl_Position = vec4(aPos, 1.0);
}