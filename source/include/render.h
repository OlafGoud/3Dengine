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
#include "render/model.h"
#include "render/renderterrain.h"


extern float deltaTime;

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


class RenderModel : public RenderObject {
public:
  RenderModel(Shader* shader);

  void render(Camera &camera) override;
  void checkForClick(float xPos, float yPos, Camera &camera) override;
  
  
  private:
  std::vector<ModelObject*> models;
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

