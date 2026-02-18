#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement {
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT
};

const float YAW         = -90.0f;
const float PITCH       = -45.0f;     // tilted downward
const float SPEED       = 10.0f;
const float SENSITIVITY = 0.2f;
const float ZOOM        = 20.0f;

class Camera {
public:
  // Camera state
  glm::vec3 Position;
  glm::vec3 Target;      // point camera looks at
  glm::vec3 Up;

  float Yaw;
  float Pitch;
  float Distance;        // zoom distance

  float MovementSpeed;
  float MouseSensitivity;
  unsigned int SCR_WIDTH = 800;
  unsigned int SCR_HEIGHT = 600;


  Camera(glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f)) : Yaw(YAW), Pitch(PITCH), Distance(ZOOM), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY) {
    Target = target;
    Up = glm::vec3(0.0f, 1.0f, 0.0f);
    updateCameraPosition();
  }

  glm::mat4 GetViewMatrix() {
    return glm::lookAt(Position, Target, Up);
  }

  glm::mat4 getProjectionMatrix() {
    return glm::perspective(glm::radians(this->Distance), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
  }

  // Move target on XZ plane
  void ProcessKeyboard(Camera_Movement direction, float deltaTime) {
    float velocity = MovementSpeed * deltaTime;

    glm::vec3 forward = glm::normalize(glm::vec3(
      cos(glm::radians(Yaw)),
      0.0f,
      sin(glm::radians(Yaw))
    ));

    glm::vec3 right = glm::normalize(glm::cross(forward, Up));

    if (direction == FORWARD)
      Target += forward * velocity;
    if (direction == BACKWARD)
      Target -= forward * velocity;
    if (direction == LEFT)
      Target -= right * velocity;
    if (direction == RIGHT)
      Target += right * velocity;

    updateCameraPosition();
  }

  void ProcessMouseXYMovement(float xoffset, float yoffset) {
    glm::vec3 forward = glm::normalize(glm::vec3(
      cos(glm::radians(Yaw)),
      0.0f,
      sin(glm::radians(Yaw))
    ));

    glm::vec3 right = glm::normalize(glm::cross(forward, Up));
    
    yoffset = yoffset * 0.2;
    xoffset = xoffset * 0.2;

    Target += forward * xoffset;
    Target += right * yoffset;

    updateCameraPosition();
  }

  // Rotate around target
  void ProcessMouseRotation(float xoffset, float yoffset, GLboolean constrainPitch = true) {
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw   += xoffset;
    Pitch += yoffset;

    if (constrainPitch) {
      if (Pitch > -10.0f) Pitch = -10.0f;   // prevent flipping
      if (Pitch < -89.0f) Pitch = -89.0f;
    }

    updateCameraPosition();
  }

  // Zoom in/out
  void ProcessMouseScroll(float yoffset) {
    Distance -= yoffset;

    if (Distance < 5.0f)  Distance = 5.0f;
    if (Distance > 100.0f) Distance = 100.0f;

    updateCameraPosition();
  }

private:

  void updateCameraPosition() {
    glm::vec3 direction;

    direction.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    direction.y = sin(glm::radians(Pitch));
    direction.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));

    Position = Target - glm::normalize(direction) * Distance;
  }
};

#endif
