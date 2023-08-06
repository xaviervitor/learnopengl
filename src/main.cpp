#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <iostream>

#include "shader.hpp"

void framebuffer_size_callback(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main() {
    // init glfw context
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // create glfw window
    GLFWwindow* window = glfwCreateWindow(1200, 900, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // set glfw window to current context 
    glfwMakeContextCurrent(window);
    // pass window resizing callback address to glfw
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // pass glfw GL loader function pointer to glad gl loader,
    // so that glad can initialize itself and OpenGL
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // all the shader logic is now being done in a separate 
    // "Shader" class. more details in the "shader.cpp" file
    Shader shader = Shader(
        "resources/shaders/shader.vs",
        "resources/shaders/shader.fs");

    // set vertex data to be stored in the Vertex Buffer
    // and passed to the vertex shader
    float vertices[] = {
        // positions        // colors
        0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, // bottom right
       -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, // bottom left
        0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f  // top
    };

    // set indices used by the EBO (Element Buffer Object)
    unsigned int indices[] = {
        0, 1, 2, // triangle 1
    };

    unsigned int VAO; // (Vertex Array Object)
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO; // (Vertex Buffer Object)
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // stores the vertices array in the currently bound buffer, the VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned int EBO; // (Element Buffer Object)
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // stores the indices array in the currently bound buffer, the EBO
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // specifies how the vertex data from the buffer currently bound 
    // to GL_ARRAY_BUFFER target is set to be interpreted. in this case, 
    // we are specifying that location 0 is an attribute consisting
    // of 3 components, they are of float type, (in practice, a vec3), 
    // that they are tightly packed (stride = 12) and that the offset
    // of the buffer bound to GL_ARRAY_BUFFER is zero ((void*) 0).
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) 0);

    // vertex attributes are disabled by default
    glEnableVertexAttribArray(0);
    
    // sets up the new color attributes, the vertex attributes have
    // a new stride 6 floats (stride = 24)., and the color data 
    // starts 3 floats ahead of the initial pointer, so a 
    // (void*) (3 * sizeof(float)) pointer has to be passed.
    // the stride has to be changed for every glVertexAttribPointer,
    // otherwise the attribute would be unaligned.
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)));
    
    // every active vertex attribute has to be enabled individually
    glEnableVertexAttribArray(1);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        // an enum has to be specified indicating which buffer is to be
        // cleared, with the previously specified glClearColor call.
        // possible values: GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT
        // and GL_STENCIL_BUFFER_BIT.
        glClear(GL_COLOR_BUFFER_BIT);

        // calls glUseProgram() internally
        shader.use();
        
        // binds VAO that contains all the stored vertex data
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

        // swaps the back draw buffer with the currently shown draw buffer.
        // All draw calls above were executed in the back buffer to prevent
        // rendering artifacts.
        glfwSwapBuffers(window);

        // polls for events like keyboard interruptions, window resizes, etc.
        // and invokes the corresponding functions/callbacks
        glfwPollEvents();
    }

    // clean up resources used by glfw
    glfwTerminate();

    return 0;
}