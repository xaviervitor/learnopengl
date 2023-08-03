#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
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
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // set glfw window to current context 
    glfwMakeContextCurrent(window);

    // pass glfw GL loader function pointer to glad gl loader,
    // so that glad can initialize itself and OpenGL
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // set opengl viewport to be the same as the glfw window
    glViewport(0, 0, 800, 600);
    // pass window resizing callback address to glfw
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // create vertex shader object
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    const char *vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";

    // set vertex shader object's source code to specified string
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // check compilation success
    int vertexSuccess;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexSuccess);
    if (!vertexSuccess) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // create fragment shader object
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()"
    "{\n"
    "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\0";

    // set fragment shader object's source code to specified string
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // check compilation success
    int fragmentSuccess;
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentSuccess);
    if (!fragmentSuccess) {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // create shader program object
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    // add shaders to the program
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    // link all shaders in the specified program
    glLinkProgram(shaderProgram);

    // check linking success
    int programSuccess;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &programSuccess);
    if (!programSuccess) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
    }

    // delete shader objects that are already compiled and
    // linked to the shader program
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set vertex data to be stored in the Vertex Buffer
    // and passed to the vertex shader
    float vertices[] = {
        0.5f,  0.5f, 0.0f, // vertex 1
        0.5f, -0.5f, 0.0f, // vertex 2
        -0.5f, -0.5f, 0.0f, // vertex 3
        -0.5f,  0.5f, 0.0f // vertex 4
    };

    // set indices used by the EBO (Element Buffer Object)
    unsigned int indices[] = {
        0, 1, 3, // triangle 1
        1, 2, 3 // triangle 2
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
    
    // vertex attributes are disabled by default
    glEnableVertexAttribArray(0);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        // binds VAO that contains all the stored vertex data
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // unbinds VAO
        glBindVertexArray(0);
        glUseProgram(0);

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