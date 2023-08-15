#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <iostream>

#include "stb_image.h"
#include "shader.hpp"
#include "camera.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// store screen size to evaluate aspect ratio dynamically
unsigned int SCREEN_WIDTH = 1200;
unsigned int SCREEN_HEIGHT = 900;

// instead of a view matrix, we define a Camera class
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX;
float lastY;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main() {
    // init glfw context
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // create glfw window
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // set glfw window to current context 
    glfwMakeContextCurrent(window);
    // pass callback function addresses to glfw
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // pass glfw GL loader function pointer to glad gl loader,
    // so that glad can initialize itself and OpenGL
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // enable the OpenGL auto depth test. in a single draw call, OpenGL
    // draws every triangle in the call, drawing triangles over other
    // triangles. the depth test draws the z-data in another buffer,
    // the DEPTH buffer and compares the z-data to draw or to skip drawing. 
    glEnable(GL_DEPTH_TEST);

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
    
    // cube model
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    unsigned int VAO; // (Vertex Array Object)
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO; // (Vertex Buffer Object)
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // stores the vertices array in the currently bound buffer, the VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // specifies how the vertex data from the buffer currently bound 
    // to GL_ARRAY_BUFFER target is set to be interpreted
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);

    // vertex attributes are disabled by default
    glEnableVertexAttribArray(0);
    
    // sets up the texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // world positions of each cube
    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)
    };

    while (!glfwWindowShouldClose(window)) {
        // deltaTime is the time it took to process the previous frame 
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        processInput(window);
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        // an enum has to be specified indicating which buffer is to be
        // cleared, with the previously specified glClearColor call.
        // possible values: GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT
        // and GL_STENCIL_BUFFER_BIT.
        // addendum: the depth buffer has to be cleared too.  
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

        // declares the projection matrix (see shader.vs).
        glm::mat4 projection(1.0f);
        projection = glm::perspective(glm::radians(camera.zoom), (float) SCREEN_WIDTH / (float) SCREEN_HEIGHT, 0.1f, 100.0f);
        shader.setMat4("projection", projection);
        
        // declares the view matrix (see shader.vs).
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("view", view);
        
        // binds VAO that contains all the stored vertex data
        glBindVertexArray(VAO);
        for (unsigned int i = 0 ; i < 10 ; i++) {
            // declares the model matrix (see shader.vs).
            glm::mat4 model(1.0f);
            // (2nd operation) applies constant translation. it is not cumulative.
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            // (1st operation) applies rotation based on how much time has passed
            model = glm::rotate(model, (float) glfwGetTime() * glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            // at every frame, the object starts at its center, (0, 0, 0), rotates
            // and translates to its position that can be seen on screen. 
            glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, &model[0][0]);
            
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

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

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback(GLFWwindow*, int width, int height) {
    SCREEN_WIDTH = width;
    SCREEN_HEIGHT = height;
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xPos, double yPos) {
    // if the callback was never called, lastX and lastY would never be
    // set. this sets with the callback value just once.
    if (firstMouse) {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }
    
    // gets the mouse movement delta from the last mouse position
    float xOffset = xPos - lastX;
    // this has to be reversed since y-coordinates range from bottom to top
    float yOffset = lastY - yPos; 

    lastX = xPos;
    lastY = yPos;

    camera.ProcessMouseMovement(xOffset, yOffset);
}

void scroll_callback(GLFWwindow* window, double xOffset, double yOffset) {
    camera.ProcessMouseScroll(yOffset);
}