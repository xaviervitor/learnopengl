#include <cstdio>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "stb/stb_image.h"

#include "shader.hpp"
#include "camera.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);

const int DEFAULT_SCREEN_WIDTH = 1200;
const int DEFAULT_SCREEN_HEIGHT = 900;

int screenWidth;
int screenHeight;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX;
float lastY;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    screenWidth = DEFAULT_SCREEN_WIDTH;
    screenHeight = DEFAULT_SCREEN_HEIGHT;

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return -1;
    }

    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_STENCIL_TEST);
    // if stencil & depth tests succeed, GL_REPLACE with ref value (1). otherwise, GL_KEEP
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    Shader textureShader = Shader(
        "resources/shaders/texture.vs",
        "resources/shaders/texture.fs");

    Shader colorShader = Shader(
        "resources/shaders/color.vs",
        "resources/shaders/color.fs");

    float cubeVertices[] = {
        // positions          // texture Coords
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

    // Set texture coords higher than 1.0 (together with GL_REPEAT as texture wrapping mode)
    // will cause the floor texture to repeat
    float planeVertices[] = {
        // positions          // texture Coords
         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
         5.0f, -0.5f, -5.0f,  2.0f, 2.0f
    };

    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glBindVertexArray(planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    unsigned int marbleTexture = loadTexture("resources/textures/marble.jpg");
    unsigned int metalTexture = loadTexture("resources/textures/metal.png");

    textureShader.use();
    textureShader.setInt("texture0", 0);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = (float) glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float) screenWidth / (float) screenHeight, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model;

        // setup shaders
        textureShader.use();
        textureShader.setMat4("projection", projection);
        textureShader.setMat4("view", view);
        colorShader.use();
        colorShader.setMat4("projection", projection);
        colorShader.setMat4("view", view);
        colorShader.setVec3("color", glm::vec3(1.0f, 0.0f, 0.0f));

        // make sure to not update the stencil buffer while drawing the floor
        glStencilMask(0x00);

        // draw floor
        glBindVertexArray(planeVAO);
        textureShader.use();
        glBindTexture(GL_TEXTURE_2D, metalTexture);
        model = glm::mat4(1.0f);
        textureShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // 1st render pass: draw boxes as normal, writing to the stencil buffer
        // -----------------------------------------------------------------------------------------

        // all fragments should GL_ALWAYS pass the stencil test
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF); // enable writing to the stencil buffer

        // draw boxes
        glBindVertexArray(cubeVAO);
        glBindTexture(GL_TEXTURE_2D, marbleTexture);
        // first box
        model = glm::mat4(1.0f);
        textureShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // second box
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(1.25f, 0.0f, -0.75f));
        textureShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        // 2nd render pass: draw scaled versions of the objects, this time disabling stencil
        // writing. The parts of the stencil buffer that have been written (the entire box) are not
        // drawn, thus only drawing the objects' size differences, making it look like borders.
        // -----------------------------------------------------------------------------------------

        // stencil test passes only if the buffer value is GL_NOTEQUAL to ref value (1)
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00); // disable writing to the stencil buffer

        glDisable(GL_DEPTH_TEST); // disable depth testing to draw the outline above all fragments

        // draw scaled boxes
        glBindVertexArray(cubeVAO);
        colorShader.use();
        glm::vec3 outlineScale(1.01f);
        // first box
        model = glm::mat4(1.0f);
        model = glm::scale(model, outlineScale);
        colorShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // second box
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(1.25f, 0.0f, -0.75f));
        model = glm::scale(model, outlineScale);
        colorShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        // reenable depth testing after outline drawing
        glEnable(GL_DEPTH_TEST);

        // Enable writing to the stencil buffer - this has to be done before the
        // glClear(GL_STENCIL_BUFFER_BIT) call, or the stencil buffer will not be cleared!
        glStencilMask(0xFF);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow*, int width, int height) {
    screenWidth = width;
    screenHeight = height;
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow*, double xPos, double yPos) {
    if (firstMouse) {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    float xOffset = xPos - lastX;
    float yOffset = lastY - yPos;

    lastX = xPos;
    lastY = yPos;

    camera.ProcessMouseMovement(xOffset, yOffset);
}

void scroll_callback(GLFWwindow*, double, double yOffset) {
    camera.ProcessMouseScroll(yOffset);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(CameraMovement::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(CameraMovement::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(CameraMovement::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(CameraMovement::RIGHT, deltaTime);
}

unsigned int loadTexture(const char* path) {
    unsigned int id;
    glGenTextures(1, &id);

    int width, height, channels;
    unsigned char *data = stbi_load(path,
        &width, &height, &channels, 0);

    if (data != NULL) {
        GLenum format;
        if (channels == 1)
            format = GL_RED;
        else if (channels == 3)
            format = GL_RGB;
        else if (channels == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, (GLint) format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        printf("Texture load failed. Path: %s\n", path);
    }
    stbi_image_free(data);

    return id;
}