#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    // Camera attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    // Euler angles
    float Yaw;
    float Pitch;

    // Camera options
    float MovementSpeed;
    float MouseSensitivity;

    Camera();

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix();

    // Process keyboard input
    void processKeyboard(bool w, bool s, bool a, bool d, float deltaTime);

    // Process mouse movement
    void processMouseMovement(float xoffset, float yoffset);

    // Teleport to the North Entrance Gate
    void teleportToGate();

private:
    void updateCameraVectors();
};

#endif // CAMERA_H
