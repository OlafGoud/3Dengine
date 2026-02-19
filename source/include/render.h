#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <unordered_map>
#include <vector>
#include "shader.h"
#include "camera.h"

#include "render/renderui.h"

#define CHUNK_SIZE 20

extern float deltaTime;

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
  long long makeKey(int x, int z);
  Chunk* getChunk(int x, int z);

  std::unordered_map<long long, Chunk> chunkList{};
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
  bool pickSquareFast(const glm::vec3& rayOrigin, const glm::vec3& rayDir, int& outX, int& outZ);
  bool pickSquare(const glm::vec3 &rayOrigin, const glm::vec3 &rayDir, int &outX, int &outZ);
  glm::vec3 position;

private:
  std::vector<float> vertices;
  std::vector<float> normals;
  std::vector<float> colors;
  std::vector<unsigned int> indices;
  GLuint VAO, VBO, NBO, CBO, EBO;
};


class RenderModel : public RenderObject {
  RenderModel(Shader* shader);

  void render(Camera &camera) override;
  void checkForClick(float xPos, float yPos, Camera &camera) override;
  Shader* shader;

};


class RenderUI : public RenderObject {
public:
  RenderUI(Shader* shader);

  void render(Camera &camera) override;
  void checkForClick(float xPos, float yPos, Camera &camera) override;
  Shader* shader;

private:
  std::vector<UIElement*> uiElements;
};

