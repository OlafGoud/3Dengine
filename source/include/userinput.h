#pragma once
#include "camera.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <unordered_map>
#include <cstdint>
#include <vector>
#include "render.h"


extern std::vector<float> vertices; // x,y,z
extern std::vector<float> colors;   // r,g,b

extern GLuint VAO, VBO, CBO, NBO;

class User {
public:
  User();
  void setInputCallbacks(GLFWwindow *window);

  void mouseMovementInput(GLFWwindow* window, float xposIn, float yposIn);
  void mouseButtonInput(GLFWwindow* window, int button, int action, int mods);
  void mouseScrollInput(double yoffset);


  void keyboardInput(GLFWwindow *window, float deltaTime);
  const int * getStructurePointer();

  Camera camera;
  std::vector<RenderObject*> renderObjects;

private:

void calculateClickPosition(GLFWwindow* window);
bool rayTriangleIntersect(const glm::vec3 &orig, const glm::vec3 &dir, const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2, float &t);

std::unordered_map<int, uint8_t> pressedKeys{}; /** keyboard KEY, on/of */
std::unordered_map<int, uint8_t> pressedMouseButtons{}; /** mousebuttons, on/of */
std::unordered_map<int, glm::vec3> structureColors{}; /** mousebuttons, on/of */

int structure = -1;
float lastX, lastY;

};

extern User user;
