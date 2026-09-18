#include "camera.h"
#include "mesh.h"

Camera::Camera() {
    Position = glm::vec3(0.0f, 1.0f, 5.0f);
    WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    Yaw = -90.0f;
    Pitch = 0.0f;
    MovementSpeed = 5.0f;
    MouseSensitivity = 0.1f;

    updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() {
    return glm::lookAt(Position, Position + Front, Up);
}

void Camera::processKeyboard(bool w, bool s, bool a, bool d, float deltaTime) {
    float velocity = MovementSpeed * deltaTime;
    
    // Flatten front vector for movement on X/Z plane only
    glm::vec3 flatFront = glm::normalize(glm::vec3(Front.x, 0.0f, Front.z));

    if (w) Position += flatFront * velocity;
    if (s) Position -= flatFront * velocity;
    if (a) Position -= Right * velocity;
    if (d) Position += Right * velocity;

    // Bound camera within the 380x380 fence
    float bound = 379.0f;
    if (Position.x > bound) Position.x = bound;
    if (Position.x < -bound) Position.x = -bound;
    if (Position.z > bound) Position.z = bound;
    if (Position.z < -bound) Position.z = -bound;

    // Keep the camera at eye level on the terrain
    Position.y = Mesh::getTerrainHeight(Position.x, Position.z) + 1.8f;
}

void Camera::processMouseMovement(float xoffset, float yoffset) {
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw   += xoffset;
    Pitch -= yoffset;

    // Constrain pitch
    if (Pitch > 89.0f) Pitch = 89.0f;
    if (Pitch < -89.0f) Pitch = -89.0f;

    updateCameraVectors();
}

void Camera::teleportToGate() {
    Position = glm::vec3(0.0f, 1.8f, -370.0f);
    Yaw = 90.0f; 
    Pitch = 0.0f;
    updateCameraVectors();
}

void Camera::updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up    = glm::normalize(glm::cross(Right, Front));
}
