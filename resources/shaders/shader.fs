#version 330 core

// these are the variables that were passed by the previous shader,
// in our case, the vertex shader. the "in" variables of the
// fragment shader have to be of the same name and type as the
// variables in the previous shader.
in vec3 ourColor;
in vec2 TexCoord;

// textures in a shader are declared as uniforms but aren't stored
// using a glUniform call. instead, texture data is set in each
// texture unit (GL_TEXTURE#) and the glUniform is used to specify
// the index of each texture unit.
uniform sampler2D texture1;
uniform sampler2D texture2;

// the fragment shader has a special requirement, as it is the 
// last shader in the pipeline. a output variable of type vec4
// has to be declared and set somewhere in the code. this is
// the final color of each fragment.
out vec4 FragColor;

void main() {
    FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
}