#pragma once
#include "camera.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <unordered_map>
#include <cstdint>
#include <vector>
#include "render.h"
#include "events.h"


class User {
public:
  User();
  void setInputCallbacks(GLFWwindow *window);

  void mouseMovementInput(GLFWwindow* window, float xposIn, float yposIn);
  void mouseButtonInput(GLFWwindow* window, int button, int action, int mods);
  void mouseScrollInput(double yoffset);


  void keyboardInput(GLFWwindow *window, float deltaTime);
  const int* getStructurePointer();

  Camera camera;
  std::vector<RenderObject*> renderObjects;
  EventManager eventManager{};

private:

std::unordered_map<int, uint8_t> pressedKeys{}; /** keyboard KEY, on/of */
std::unordered_map<int, uint8_t> pressedMouseButtons{}; /** mousebuttons, on/of */
std::unordered_map<int, glm::vec3> structureColors{}; /** mousebuttons, on/of */

int structure = -1;
float lastX, lastY;

};

extern User user;
