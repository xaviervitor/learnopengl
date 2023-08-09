#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <iostream>

#include "shader.hpp"
#include "stb_image.h"

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

    unsigned int texture1;
    glGenTextures(1, &texture1);
    // the default active texture is already GL_TEXTURE0, this is just for clarity
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // if the object that this texture is supposed to map to is smaller in the
    // viewport than the texture itself, the operation defined in the
    // MIN filter (in this case GL_LINEAR_MIPMAP_LINEAR) will be performed. 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // the same goes for MAG filter, if the object is bigger than the texture in the viewport.
    // mipmaps are used for smaller objects in screen, and thus it doesn't make
    // sense to set mipmap filtering options on the MAG filter. giving it a
    // mipmap filtering option will generate an OpenGL GL_INVALID_ENUM error code.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    // OpenGL expects images to have its 0.0 coordinates at the 
    // bottom, and the images the code loads have its 0.0 coordinates 
    // at the top. stbi can flip the image coordinates on load.
    stbi_set_flip_vertically_on_load(true);
    
    int width, height, nrChannels;
    // if the last argument, "desired_channels" is set to zero, stbi
    // will load all of the channels available in the image file (RGB). 
    unsigned char *data = stbi_load("resources/textures/container.jpg",
        &width, &height, &nrChannels, 0);
    
    if (data) {
        // the format arguments (set to GL_RGB) have to be set as having
        // the same channels as the loaded image. in the stbi_load this
        // can be specified in the desired_channels argument.
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture 1" << std::endl;
    }

    stbi_image_free(data);

    unsigned int texture2;
    glGenTextures(1, &texture2);
    // to have multiple textures in a shader, we specify texture units.
    // texture units have the form of GL_TEXTURE#. before binding textures,
    // we have to activate a GL_TEXTURE# and later assign the corresponding
    // location to the shader sampler2D uniform using glUniform1i(). 
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width1, height1, nrChannels1;
    unsigned char* data1 = stbi_load("resources/textures/awesomeface.png", 
        &width1, &height1, &nrChannels1, 0);
    
    if (data1) {
        // the loaded image has the .png format, and 4 channels. this 
        // is specified in the format arguments as "GL_RGBA". again, 
        // these arguments have to match 
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width1, height1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data1);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture 2" << std::endl;
    }

    stbi_image_free(data1);

    shader.use();
    // now we assign the index of the loaded texture to the 
    // location of corresponding sampler2D (0 for GL_TEXTURE0)
    glUniform1i(glGetUniformLocation(shader.ID, "texture1"), 0);
    // same as above for the texture2, 1 for GL_TEXTURE1
    shader.setInt("texture2", 1);
    
    // set vertex data to be stored in the Vertex Buffer
    // and passed to the vertex shader
    float vertices[] = {
        // positions         // colors           // texture coords
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
       -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
       -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left
    };

    // set indices used by the EBO (Element Buffer Object)
    unsigned int indices[] = {
        1, 2, 3, // triangle 1
        0, 1, 3 // triangle 2
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) 0);

    // vertex attributes are disabled by default
    glEnableVertexAttribArray(0);
    
    // sets up the new color attributes, the vertex attributes have
    // a new stride 6 floats (stride = 24)., and the color data 
    // starts 3 floats ahead of the initial pointer, so a 
    // (void*) (3 * sizeof(float)) pointer has to be passed.
    // the stride has to be changed for every glVertexAttribPointer,
    // otherwise the attribute would be unaligned.
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (3 * sizeof(float)));
    
    // every active vertex attribute has to be enabled individually
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (6 * sizeof(float)));
    glEnableVertexAttribArray(2);

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
        // before drawing, all the textures used have to be active and bound 
        // to their target and their texture unit.
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        // for example, the following code can be read as "in the texture 
        // unit 1, bind the 'texture2' object to the GL_TEXTURE_2D target" 
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        // binds VAO that contains all the stored vertex data
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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