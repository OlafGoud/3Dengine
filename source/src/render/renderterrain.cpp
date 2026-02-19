#include "render/renderterrain.h"



Chunk::Chunk(int worldX, int worldZ) {
  this->position = glm::vec3(worldX * CHUNK_SIZE, 0.0f, worldZ * CHUNK_SIZE);

  for (int z = 0; z <= CHUNK_SIZE; z++) {
    for (int x = 0; x <= CHUNK_SIZE; x++) {
      vertices.push_back(x + (CHUNK_SIZE * worldX));
      vertices.push_back((float)(rand() % 3) / 4);
      vertices.push_back(z + (CHUNK_SIZE * worldZ));

      normals.push_back(0);
      normals.push_back(1);
      normals.push_back(0);

      colors.push_back(0.4f);
      colors.push_back(0.8f);
      colors.push_back(0.4f);
    }
  }

  for (int z = 0; z < CHUNK_SIZE; z++) {
    for (int x = 0; x < CHUNK_SIZE; x++) {
      unsigned int tl = z * (CHUNK_SIZE + 1) + x;
      unsigned int tr = tl + 1;
      unsigned int bl = (z + 1) * (CHUNK_SIZE + 1) + x;
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

bool Chunk::pickSquareFast(const glm::vec3& rayOrigin, const glm::vec3& rayDir, int& outX, int& outZ) {
  if (fabs(rayDir.y) < 0.0001f)
    return false;

  float t = -rayOrigin.y / rayDir.y;

  if (t < 0)
    return false;

  glm::vec3 hitPos = rayOrigin + rayDir * t;

  float half = CHUNK_SIZE / 2.0f;

  int x = (int)floor(hitPos.x + half);
  int z = (int)floor(hitPos.z + half);

  if (x < 0 || x >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE)
    return false;

  outX = x;
  outZ = z;
  return true;
}



bool Chunk::rayTriangleIntersect(const glm::vec3 &orig, const glm::vec3 &dir, const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2, float &t) {
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

