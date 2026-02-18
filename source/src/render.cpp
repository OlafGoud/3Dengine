#include "render.h"
#include <iostream>
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


RenderTerrain::RenderTerrain(Shader* shader) {
  this->shader = shader;

  int size = 20;
  for(int z = -size; z <= size; z++) for(int x = -size; x <= size; x++) {
    chunks.push_back(Chunk(x, z));
  }
  //chunks.push_back(Chunk(0, 0));
}

void RenderTerrain::render(Camera &camera) {

  shader->use();
  shader->setMat4("projection", camera.getProjectionMatrix());
  shader->setMat4("view", camera.GetViewMatrix());
  
  for(Chunk chunk : chunks) {
    shader->setMat4("model", glm::mat4(1.0f));
    chunk.render();
  }
}

void RenderTerrain::checkForClick(float xPos, float yPos, Camera &camera) {
  float x = (2.0f * xPos) / SCR_WIDTH - 1.0f;
  float y = 1.0f - (2.0f * yPos) / SCR_HEIGHT;
  glm::vec4 ray_clip(x, y, -1.0f, 1.0f);

  glm::vec4 ray_eye = glm::inverse(camera.getProjectionMatrix()) * ray_clip;
  ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f);

  glm::vec3 ray_world = glm::normalize(glm::vec3(glm::inverse(camera.GetViewMatrix()) * ray_eye));

  int sqX, sqZ;
  for(Chunk &chunk : this->chunks) {
    if(chunk.pickSquare(camera.Position, ray_world, sqX, sqZ)) {
      /** do shit */
      chunk.setColor(sqX, sqZ, glm::vec3(0.1f, 0.01f, 1.0f));
      chunk.changeHeight(sqX, sqZ, -0.1f);
      std::cout << " dsaf\n";
    }
  }
}

Chunk::Chunk(int worldX, int worldZ) {

  this->position = glm::vec3(worldX * CHUNK_SIZE, 0.0f, worldZ * CHUNK_SIZE);

  GRID_SIZE = CHUNK_SIZE;
  float half = GRID_SIZE / 2.0f;

  for (int z = 0; z <= GRID_SIZE; z++) {
    for (int x = 0; x <= GRID_SIZE; x++) {
      vertices.push_back(x - half + (CHUNK_SIZE * worldX));
      vertices.push_back((float)(rand() % 3) / 4);
      vertices.push_back(z - half + (CHUNK_SIZE * worldZ));

      normals.push_back(0);
      normals.push_back(1);
      normals.push_back(0);

      colors.push_back(0.4f);
      colors.push_back(0.8f);
      colors.push_back(0.4f);
    }
  }

  for (int z = 0; z < GRID_SIZE; z++) {
    for (int x = 0; x < GRID_SIZE; x++) {
      unsigned int tl = z * (GRID_SIZE + 1) + x;
      unsigned int tr = tl + 1;
      unsigned int bl = (z + 1) * (GRID_SIZE + 1) + x;
      unsigned int br = bl + 1;

      indices.insert(indices.end(), {tl, bl, tr, tr, bl, br});
    }
  }

  recalcNormals();
  genGPUBuffers();


}

void Chunk::render() {
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

}

void Chunk::recalcNormals() {
  std::fill(normals.begin(), normals.end(), 0.0f);

  for (size_t i = 0; i < indices.size(); i += 3) {
    unsigned int i0 = indices[i];
    unsigned int i1 = indices[i + 1];
    unsigned int i2 = indices[i + 2];

    glm::vec3 v0(vertices[i0 * 3], vertices[i0 * 3 + 1], vertices[i0 * 3 + 2]);
    glm::vec3 v1(vertices[i1 * 3], vertices[i1 * 3 + 1], vertices[i1 * 3 + 2]);
    glm::vec3 v2(vertices[i2 * 3], vertices[i2 * 3 + 1], vertices[i2 * 3 + 2]);

    glm::vec3 n = glm::normalize(glm::cross(v1 - v0, v2 - v0));

    for (unsigned int idx : {i0, i1, i2}) {
      normals[idx * 3] += n.x;
      normals[idx * 3 + 1] += n.y;
      normals[idx * 3 + 2] += n.z;
    }
  }

  for (size_t i = 0; i < normals.size(); i += 3) {
    glm::vec3 n(normals[i], normals[i + 1], normals[i + 2]);
    n = glm::normalize(n);
    normals[i] = n.x;
    normals[i + 1] = n.y;
    normals[i + 2] = n.z;
  }

}


void Chunk::genGPUBuffers() {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &NBO);
  glGenBuffers(1, &CBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, NBO);
  glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_DYNAMIC_DRAW);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, CBO);
  glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_DYNAMIC_DRAW);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

  glBindVertexArray(0); /** unbind */

}

void Chunk::changeHeight(int x, int z, float difference) {
  int tl = z * (CHUNK_SIZE + 1) + x;
  int tr = tl + 1;
  int bl = (z + 1) * (CHUNK_SIZE + 1) + x;
  int br = bl + 1;

  for (int v : {tl, tr, bl, br}) vertices[v * 3 + 1] += difference;

  recalcNormals();

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());

  glBindBuffer(GL_ARRAY_BUFFER, NBO);
  glBufferSubData(GL_ARRAY_BUFFER, 0, normals.size() * sizeof(float), normals.data());
}

void Chunk::setColor(int x, int z, glm::vec3 color) {
  int tl = z * (CHUNK_SIZE + 1) + x;
  int tr = tl + 1;
  int bl = (z + 1) * (CHUNK_SIZE + 1) + x;
  int br = bl + 1;

  for (int v : {tl, tr, bl, br}) {
    colors[v * 3] = color.r;
    colors[v * 3 + 1] = color.g;
    colors[v * 3 + 2] = color.b;
  }

  glBindBuffer(GL_ARRAY_BUFFER, CBO);
  glBufferSubData(GL_ARRAY_BUFFER, 0, colors.size() * sizeof(float), colors.data());
}

bool Chunk::pickSquare(const glm::vec3 &rayOrigin, const glm::vec3 &rayDir, int &outX, int &outZ) {
  float closest = 999999.0f;
  bool hit = false;

  for (int z = 0; z < CHUNK_SIZE; z++) {
    for (int x = 0; x < CHUNK_SIZE; x++) {
      int tl = z * (CHUNK_SIZE + 1) + x;
      int tr = tl + 1;
      int bl = (z + 1) * (CHUNK_SIZE + 1) + x;
      int br = bl + 1;

      glm::vec3 v0(vertices[tl * 3], vertices[tl * 3 + 1], vertices[tl * 3 + 2]);
      glm::vec3 v1(vertices[bl * 3], vertices[bl * 3 + 1], vertices[bl * 3 + 2]);
      glm::vec3 v2(vertices[tr * 3], vertices[tr * 3 + 1], vertices[tr * 3 + 2]);

      float t;
      if (rayTriangleIntersect(rayOrigin, rayDir, v0, v1, v2, t)) {
        if (t < closest) {
          closest = t;
          outX = x;
          outZ = z;
          hit = true;
        }
      }

      v0 = glm::vec3(vertices[tr * 3], vertices[tr * 3 + 1], vertices[tr * 3 + 2]);
      v1 = glm::vec3(vertices[bl * 3], vertices[bl * 3 + 1], vertices[bl * 3 + 2]);
      v2 = glm::vec3(vertices[br * 3], vertices[br * 3 + 1], vertices[br * 3 + 2]);

      if (rayTriangleIntersect(rayOrigin, rayDir, v0, v1, v2, t)) {
        if (t < closest) {
          closest = t;
          outX = x;
          outZ = z;
          hit = true;
        }
      }
    }
  }

  return hit;
}


bool rayTriangleIntersect(const glm::vec3 &orig, const glm::vec3 &dir, const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2, float &t) {
  const float EPS = 0.0000001f;

  glm::vec3 edge1 = v1 - v0;
  glm::vec3 edge2 = v2 - v0;
  glm::vec3 h = glm::cross(dir, edge2);
  float a = glm::dot(edge1, h);

  if (a > -EPS && a < EPS)
    return false;

  float f = 1.0f / a;
  glm::vec3 s = orig - v0;
  float u = f * glm::dot(s, h);
  if (u < 0.0 || u > 1.0)
    return false;

  glm::vec3 q = glm::cross(s, edge1);
  float v = f * glm::dot(dir, q);
  if (v < 0.0 || u + v > 1.0)
    return false;

  t = f * glm::dot(edge2, q);
  return t > EPS;
}


