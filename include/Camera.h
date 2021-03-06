#pragma once
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum CameraMovements{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

const float YAW = -135.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera
{
public:
   // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
    // ctor with scalar
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);
    glm::mat4 GetViewMatrix();
    void ProcessKeyboard(CameraMovements direction, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
    void ProcessMouseScroll(float yoffset);
private:
    void updateCameraVectors();
};