#pragma once

#include "glm/glm.hpp"

// Defines several possible options for camera movement. Used as abstraction
// to stay away from window-system specific input methods.
enum class CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// An abstract camera class that processes input and calculates the
// corresponding Euler Angles, Vectors and Matrices for use in OpenGL.
class Camera {
public:
    // camera Attributes
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    // euler Angles
    float yaw;
    float pitch;
    // camera options
    float movementSpeed;
    float mouseSensitivity;
    float zoom;

    // constructor with vectors
    Camera(glm::vec3 inPosition = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 inWorldUp = glm::vec3(0.0f, 1.0f, 0.0f),
        float inYaw = Camera::defaultYaw,
        float inPitch = Camera::defaultPitch);

    // constructor with scalar values
    Camera(float inPosX, float inPosY, float inPosZ,
        float inWorldUpX, float inWorldUpY, float inWorldUpZ,
        float inYaw,
        float inPitch);

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix();

    // processes input received from any keyboard-like input system. Accepts input
    // parameter in the form of camera defined ENUM (to abstract it from windowing
    // systems)
    void ProcessKeyboard(CameraMovement direction, float deltaTime);

    // processes input received from a mouse input system. Expects the offset
    // value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

    // processes input received from a mouse scroll-wheel event. Only requires
    // input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset);

private:
    // Default camera values
    constexpr static float defaultYaw = -90.0f;
    constexpr static float defaultPitch = 0.0f;
    constexpr static float defaultSpeed = 2.5f;
    constexpr static float defaultSensitivity = 0.1f;
    constexpr static float defaultZoom = 45.0f;

    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors();
};