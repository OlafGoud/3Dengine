#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "shader.h"
#include "camera.h"
#define CHUNK_SIZE 20

class Chunk;
bool rayTriangleIntersect(const glm::vec3 &orig, const glm::vec3 &dir, const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2, float &t);

class RenderObject {
public:

  RenderObject() {}

  virtual void render(Camera &camera) = 0;
  virtual void checkForClick(float xPos, float yPos, Camera &camera) = 0;

};


class RenderTerrain : public RenderObject{

public:
  RenderTerrain(Shader* shader);

  void render(Camera &camera) override;
  void checkForClick(float xPos, float yPos, Camera &camera) override;


private:
  std::vector<Chunk> chunks{};
  Shader* shader;
};


class Chunk {
public:
  Chunk(int x, int z);
  void render();

  void recalcNormals();
  void genGPUBuffers();

  void changeHeight(int x, int z, float difference);
  void setColor(int x, int z, glm::vec3 color);

  bool pickSquare(const glm::vec3 &rayOrigin, const glm::vec3 &rayDir, int &outX, int &outZ);
  glm::vec3 position;

private:
  std::vector<float> vertices;
  std::vector<float> normals;
  std::vector<float> colors;
  std::vector<unsigned int> indices;
  GLuint VAO, VBO, NBO, CBO, EBO;

  int GRID_SIZE;

};
