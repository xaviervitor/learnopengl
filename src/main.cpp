#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <cstdio>

#include "stb/stb_image.h"
#include "shader.hpp"
#include "camera.hpp"
#include "model.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);

const int DEFAULT_SCREEN_WIDTH = 1200;
const int DEFAULT_SCREEN_HEIGHT = 900;

int screenWidth;
int screenHeight;
bool fullscreen = true;

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

    GLFWmonitor* monitor = NULL;
    if (fullscreen) {
        monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* vidmode = glfwGetVideoMode(monitor);
        screenWidth = vidmode->width;
        screenHeight = vidmode->height;
    } else {
        screenWidth = DEFAULT_SCREEN_WIDTH;
        screenHeight = DEFAULT_SCREEN_HEIGHT;
    }

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", monitor, NULL);
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

    stbi_set_flip_vertically_on_load(true);

    Model backpackModel = Model("resources/models/backpack/backpack.obj");
    Model cubeModel = Model("resources/models/cube/cube.obj");

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = (float) glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        textureShader.use();

        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float) screenWidth / (float) screenHeight, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        textureShader.setMat4("projection", projection);
        textureShader.setMat4("view", view);
        textureShader.setMat4("model", model);

        // Draw floor
        // make sure to not update the stencil buffer while drawing the floor
        glStencilMask(0x00);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
        model = glm::scale(model, glm::vec3(100.0f, 1.0f, 100.0f));
        textureShader.setMat4("model", model);
        cubeModel.Draw(textureShader);

        // all fragments should GL_ALWAYS pass the stencil test
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF); // enable writing to the stencil buffer

        // Draw backpack
        model = glm::mat4(1.0f);
        textureShader.setMat4("model", model);
        backpackModel.Draw(textureShader);

        // stencil test passes only if the buffer value is GL_NOTEQUAL to ref value (1)
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00); // disable writing to the stencil buffer

        glDisable(GL_DEPTH_TEST); // disable depth testing to draw the outline above all fragments

        // Draw scaled backpack
        colorShader.use();
        colorShader.setMat4("projection", projection);
        colorShader.setMat4("view", view);

        glm::vec3 outlineColor = glm::vec3(1.0f, 0.0f, 0.0f);
        colorShader.setVec3("color", outlineColor);
        glm::vec3 outlineScale = glm::vec3(1.01f);
        model = glm::mat4(1.0f);
        model = glm::scale(model, outlineScale);
        colorShader.setMat4("model", model);
        backpackModel.Draw(colorShader);

        glEnable(GL_DEPTH_TEST); // reenable depth testing after outline drawing

        // Enable writing to the stencil buffer - this has to be done before the
        // glClear(GL_STENCIL_BUFFER_BIT) call, or the stencil buffer will not be cleared!
        glStencilMask(0xFF);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

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