#include "event/inputevents.h"


MouseButtonEvent::MouseButtonEvent(GLFWwindow* window, Camera* camera, int button, int action, int mods) : window(window), camera(camera), button(button), action(action), modifiers(mods) {}

KeyBoardEvent::KeyBoardEvent(GLFWwindow* window, int key, int scancode, int action, int mods) : window(window), key(key), scancode(scancode), action(action), modifiers(mods) {}

MouseMoveEvent::MouseMoveEvent(GLFWwindow* window, Camera* camera,  float xOffset, float yOffset) : window(window), camera(camera), xOffset(xOffset), yOffset(yOffset) {}

MouseScrollEvent::MouseScrollEvent(GLFWwindow* window, Camera* camera,  float xOffset, float yOffset) : window(window), camera(camera), xOffset(xOffset), yOffset(yOffset) {}
