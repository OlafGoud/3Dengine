#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>

#include "camera.h"

#include "event/event.h"


class MouseButtonEvent : public Event {
public:
  MouseButtonEvent(GLFWwindow* window, Camera* camera, int button, int action, int mods);

  GLFWwindow* window;
  Camera* camera;
  int button;
  int action;
  int modifiers;

private:


};


class KeyBoardEvent : public Event {
public:
  KeyBoardEvent(GLFWwindow* window, int key, int scancode, int action, int mods);

  GLFWwindow* window; /** window */
  int key; /** key: GLFW_KEY_(A,B,C,etc) */
  int scancode; /** scancode of key */
  int action; /** action: GLFW_RELEASE(0) or GLFW_PRESS(1) */
  int modifiers; /** Modifier: GLFW_MOD_ (SHIFT, CONTROL, ALT, SUPER, CAPS_LOCK, NUM_LOCK)*/ 

};

class MouseMoveEvent : public Event {
public:
  MouseMoveEvent(GLFWwindow* window, Camera* camera, float xOffset, float yOffset);

  GLFWwindow* window; /** window */
  Camera* camera;
  float xOffset;
  float yOffset;
  
};

class MouseScrollEvent : public Event {
public:
  MouseScrollEvent(GLFWwindow* window, Camera* camera,  float xOffset, float yOffset);

  GLFWwindow* window; /** window */
  Camera* camera;
  float xOffset;
  float yOffset;
  
};




