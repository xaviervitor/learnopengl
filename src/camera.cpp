#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 position, glm::vec3 worldUp, float yaw, float pitch) : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM) {
    this->position = position;
    this->worldUp = worldUp;
    this->yaw = yaw;
    this->pitch = pitch;
    updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float worldUpX, float worldUpY, float worldUpZ, float yaw, float pitch) : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM) {
    this->position = glm::vec3(posX, posY, posZ);
    this->worldUp = glm::vec3(worldUpX, worldUpY, worldUpZ);
    this->yaw = yaw;
    this->pitch = pitch;
    updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() {
    return glm::lookAt(
        this->position, // position
        this->position + this->front, // target
        this->up); // up vector
}

void Camera::ProcessKeyboard(CameraMovement direction, float deltaTime) {
    // deltaTime is calculated at main() and stores the time
    // the last frame took to process. if the hardware
    // processes less frames per second, every frame will move
    // the camera more each frame (and vice-versa).
    float velocity = this->movementSpeed * deltaTime;
    
    // uses camera vectors to move in the desired directions.
    // FORWARD and BACKWARD multiplies with Front and -Front;
    // LEFT and RIGHT multiplies with Right and -Right.
    if (direction == FORWARD)
        this->position += this->front * velocity;
    if (direction == BACKWARD)
        this->position -= this->front * velocity;
    if (direction == LEFT)
        this->position -= this->right * velocity;
    if (direction == RIGHT)
        this->position += this->right * velocity;
}

void Camera::ProcessMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch) {
    xOffset *= mouseSensitivity;
    yOffset *= mouseSensitivity;

    // adds mouse delta to Euler angles
    this->yaw += xOffset;
    this->pitch += yOffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch) {
        if (this->pitch > 89.0f)
            this->pitch = 89.0f;
        if (this->pitch < -89.0f)
            this->pitch = -89.0f;
    }

    // update front, right and Up Vectors using the updated Euler angles
    updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yOffset) {
    // "zoom" here is just an alias for FOV. if the FOV gets reduced or
    // increased, the effect in the scene is a "zoom".
    this->zoom -= (float) yOffset;
    if (this->zoom < 1.0f)
        this->zoom = 1.0f;
    if (this->zoom > 45.0f)
        this->zoom = 45.0f;
}

void Camera::updateCameraVectors() {
    // after the mouse is moved, the front vector needs to be recalculated
    // using the new yaw and pitch values.
    // x = cos(Y) * cos(P)
    // y = sin(P)
    // z = sin(Y) * cos(P)
    glm::vec3 front;
    front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
    front.y = sin(glm::radians(this->pitch));
    front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
    this->front = glm::normalize(front);
    
    // the right vector can be calculated based in a used-defined WorldUp
    // vector.
    this->right = glm::normalize(glm::cross(this->front, this->worldUp)); 
    
    // with the front and right vectors calculated, we get the up vector
    // as the cross product of the two.
    this->up = glm::normalize(glm::cross(this->right, this->front));
    // the vectors need to be normalized, because their length gets closer to 
    // 0 the more you look up or down which results in slower movement.
}