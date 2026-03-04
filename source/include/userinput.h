#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <unordered_map>
#include <cstdint>
#include <vector>

#include "camera.h"
#include "render.h"


class User {
public:
  User();
  void setInputCallbacks(GLFWwindow *window);
  void addRenderObject(RenderObject* obj);

  void keyboardInput(GLFWwindow *window, float deltaTime);
  const int* getStructurePointer();

  std::vector<RenderObject*> renderObjects;
  Camera& getCamera();
  
private:
  
  std::unordered_map<int, uint8_t> pressedKeys{}; /** keyboard KEY, on/of */
  std::unordered_map<int, uint8_t> pressedMouseButtons{}; /** mousebuttons, on/of */
  std::unordered_map<int, glm::vec3> structureColors{}; /** mousebuttons, on/of */
  
  Camera camera;
  int structure = -1;
  float lastX, lastY;

};

User& getUser();
Camera& getUserCamera();