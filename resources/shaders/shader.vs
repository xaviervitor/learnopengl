#version 330 core

// the "in" variables of the vertex shader are its
// vertex attributes, as set with glVertexAttribPointer.
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

// the model matrix is used to transform the model's local 
// coordinates into world coordinates (world space). after
// all objects are transformed to world space, we have a scene. 
uniform mat4 model;

// the view matrix can be thought as the camera. contains
// transformations used to adjust the view to make the objects
// visible in the screen.
uniform mat4 view;

// the projection matrix transforms the view coordinates into
// clip coordinates. it defines a frustum using fov, aspect ratio,
// near plane and far plane and clips everything out of that region.
// after the projection transformation, perspective division is 
// performed and the last step, the viewport transform is done by
// OpenGL to finally get normalized device coordinates (NDC).
// these coordinates are sent to the rasterizer to turn them into 
// fragments.
uniform mat4 projection;

// these variables will be passed to the fragment shader, or
// the geometry shader, if there's one. the "out" variables
// of the vertex shader have to be of the same name and type
// as the variables in the next shader.
out vec2 TexCoord;

void main() {
    TexCoord = aTexCoord;
    
    // "gl_Position" is a predefined variable of glsl. the goal
    // of a vertex shader is to define a position for every vertex,
    // so somewhere in a vertex shader main function, the gl_Position
    // has to be set.
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}