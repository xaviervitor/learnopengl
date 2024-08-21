#include "camera.hpp"

#include "glm/gtc/matrix_transform.hpp"

Camera::Camera(glm::vec3 inPosition, glm::vec3 inWorldUp, float inYaw, float inPitch)
        : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(Camera::defaultSpeed), mouseSensitivity(Camera::defaultSensitivity), zoom(Camera::defaultZoom) {
    position = inPosition;
    worldUp = inWorldUp;
    yaw = inYaw;
    pitch = inPitch;
    updateCameraVectors();
}

Camera::Camera(float inPosX, float inPosY, float inPosZ, float inWorldUpX, float inWorldUpY, float inWorldUpZ, float inYaw, float inPitch)
        : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(Camera::defaultSpeed), mouseSensitivity(Camera::defaultSensitivity), zoom(Camera::defaultZoom) {
    position = glm::vec3(inPosX, inPosY, inPosZ);
    worldUp = glm::vec3(inWorldUpX, inWorldUpY, inWorldUpZ);
    yaw = inYaw;
    pitch = inPitch;
    updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() {
    return glm::lookAt(
        position, // position
        position + front, // target
        up); // up vector
}

void Camera::ProcessKeyboard(CameraMovement direction, float deltaTime) {
    // deltaTime is calculated at main() and stores the time
    // the last frame took to process. if the hardware
    // processes less frames per second, every frame will move
    // the camera more each frame (and vice-versa).
    float velocity = movementSpeed * deltaTime;

    // uses camera vectors to move in the desired directions.
    // FORWARD and BACKWARD multiplies with Front and -Front;
    // LEFT and RIGHT multiplies with Right and -Right.
    if (direction == CameraMovement::FORWARD)
        position += front * velocity;
    if (direction == CameraMovement::BACKWARD)
        position -= front * velocity;
    if (direction == CameraMovement::LEFT)
        position -= right * velocity;
    if (direction == CameraMovement::RIGHT)
        position += right * velocity;
}

void Camera::ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch) {
    xOffset *= mouseSensitivity;
    yOffset *= mouseSensitivity;

    // adds mouse delta to Euler angles
    yaw += xOffset;
    pitch += yOffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch) {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }

    // update front, right and Up Vectors using the updated Euler angles
    updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yOffset) {
    // "zoom" here is just an alias for FOV. if the FOV gets reduced or
    // increased, the effect in the scene is a "zoom".
    zoom -= (float) yOffset;
    if (zoom < 1.0f)
        zoom = 1.0f;
    if (zoom > 45.0f)
        zoom = 45.0f;
}

void Camera::updateCameraVectors() {
    // after the mouse is moved, the front vector needs to be recalculated
    // using the new yaw and pitch values.
    // x = cos(Y) * cos(P)
    // y = sin(P)
    // z = sin(Y) * cos(P)
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(newFront);

    // the right vector can be calculated based in a used-defined WorldUp
    // vector.
    right = glm::normalize(glm::cross(front, worldUp));

    // with the front and right vectors calculated, we get the up vector
    // as the cross product of the two.
    up = glm::normalize(glm::cross(right, front));
    // the vectors need to be normalized, because their length gets closer to
    // 0 the more you look up or down which results in slower movement.
}