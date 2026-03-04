#include "render.h"
#include <iostream>
#include "render/model.h"
#include "userinput.h"

/** render terrain */

RenderTerrain::RenderTerrain(Shader* shader) {
  this->shader = shader;

  int size = 20;
  for(int z = -size; z <= size; z++) for(int x = -size; x <= size; x++) {
    chunkList.insert({makeKey(x, z), Chunk(x, z)});
  }
}

void RenderTerrain::render(Camera &camera) {

  shader->use();
  shader->setMat4("projection", camera.getProjectionMatrix());
  shader->setMat4("view", camera.GetViewMatrix());
  
  for(auto chunk : chunkList) {
    shader->setMat4("model", glm::mat4(1.0f));
    chunk.second.render();
  }
}

void RenderTerrain::checkForClick(float xPos, float yPos, Camera &camera) {
  /** screen -> ray */
  float x = (2.0f * xPos) / camera.SCR_WIDTH - 1.0f;
  float y = 1.0f - (2.0f * yPos) / camera.SCR_HEIGHT;
  glm::vec4 ray_clip(x, y, -1.0f, 1.0f);

  glm::vec4 ray_eye = glm::inverse(camera.getProjectionMatrix()) * ray_clip;
  ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f);

  glm::vec3 ray_world = glm::normalize(glm::vec3(glm::inverse(camera.GetViewMatrix()) * ray_eye));


  /** ray -> world coord on y=0 */
  if (fabs(ray_world.y) < 0.0001f) return;

  float t = -camera.Position.y / ray_world.y;
  if (t < 0) return;

  glm::vec3 hitPos = camera.Position + ray_world * t;

  int baseChunkX = floor(hitPos.x / CHUNK_SIZE);
  int baseChunkZ = floor(hitPos.z / CHUNK_SIZE);
  std::cout << "x: " << hitPos.x << ", z: " << hitPos.z << "\n";

  for (int dz = -1; dz <= 1; dz++) for (int dx = -1; dx <= 1; dx++) {
    int cx = baseChunkX + dx;
    int cz = baseChunkZ + dz;

    Chunk* chunk = getChunk(cx, cz);
    if(!chunk) continue;

    int sqX, sqZ;
    if(chunk->pickSquare(camera.Position, ray_world, sqX, sqZ)) {
      //if(user.eventManager.isEvent("Color_Blue")) {
        chunk->setColor(sqX, sqZ, glm::vec3(0.1f, 0.01f, 1.0f));
        //chunk->changeHeight(sqX, sqZ, -0.1f);
        std::cout << "x: " << sqX + chunk->position.x << ", z: " << sqZ + chunk->position.z << "kkk\n"; 
      //}
      //std::cout << user.eventManager.isEvent("Color_Blue") << '\n';
    }
  }
}


long long RenderTerrain::makeKey(int x, int z) {
  return ((long long) x << 32) | (unsigned int)z;
}

Chunk* RenderTerrain::getChunk(int x, int z) {
  auto it = chunkList.find(makeKey(x, z));
  if(it != chunkList.end()) return &it->second;

  return nullptr;
}

/** render UI */

RenderUI::RenderUI(Shader* shader) {
  this->shader = shader;

  this->uiElements.push_back(new UIElementLabel(900, 1000, "GAME"));
  this->uiElements.push_back(new UIElementFPSCounter(10, 1000, &deltaTime));
  this->uiElements.push_back(new UIElementNumber(20, 20, user.getStructurePointer()));
}

void RenderUI::render(Camera &camera) {
  for(UIElement* element : uiElements) {
    element->render(this->shader);
  }
}

void RenderUI::checkForClick(float xPos, float yPos, Camera &camera) {
  
}

/** render model */

RenderModel::RenderModel(Shader* shader) {
  this->shader = shader;

  this->models.push_back(new ModelObject(glm::vec3(1.0f, 1.0f, 0.0f), new Model("bin/resources/models/castle.obj")));
}


void RenderModel::render(Camera &camera) {
  this->shader->use(); 
  this->shader->setMat4("view", camera.GetViewMatrix());
  this->shader->setMat4("projection", camera.getProjectionMatrix());

  for(ModelObject* object : models) {
    object->render(shader->getID());
  }
}


void RenderModel::checkForClick(float xPos, float yPos, Camera &camera) {

}

SphereRenderer::SphereRenderer(Shader* shader) {
  this->shader = shader;

  this->spheres.push_back(new Sphere(glm::vec3(0.0f, 5.0f, 0.0f), 5, 16));
  this->spheres.push_back(new Sphere(glm::vec3(2.0f, 2.0f, 4.0f), 2, 16));
}

void SphereRenderer::render(Camera &camera) {
  this->shader->use(); 
  this->shader->setMat4("view", camera.GetViewMatrix());
  this->shader->setMat4("projection", camera.getProjectionMatrix());

  for(Sphere* object : spheres) {
    shader->setMat4("model", glm::translate(glm::mat4(1.0f), object->position));
    object->render();
  }
}


void SphereRenderer::checkForClick(float xPos, float yPos, Camera &camera) {

}