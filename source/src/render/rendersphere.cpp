#include "render/rendersphere.h"
#include <string>
#include "render/FastNoiseLite.h"

extern glm::vec3 colorFromHex6(std::string color);

Sphere::Sphere(glm::vec3 position, float radius, int faces) {
  this->position = position;
  this->radius = radius;

  genSphere(radius, faces);
  recalcNormals();
  genGPUBuffers();


}

void Sphere::genGPUBuffers() {
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


/** 
 * generates a sphere from radius and amount of faces on Latitude and Longitude
 */
void Sphere::genSphere(float radius, int faces) {
  FastNoiseLite noise;
  noise.SetSeed(2020);
  noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

  std::vector<glm::vec3> col;
  col.push_back(colorFromHex6("#2437e4"));
  col.push_back(colorFromHex6("#16da27"));
  col.push_back(colorFromHex6("#d01b1b"));
  col.push_back(colorFromHex6("#df2222"));

  for (int i = 0; i <= faces; i++) {
    float theta = i * glm::pi<float>() / faces;
    float sinT = sin(theta);
    float cosT = cos(theta);

    for (int j = 0; j <= faces; j++) {
      float phi = j * 2.0f * glm::pi<float>() / faces;
      float sinP = sin(phi);
      float cosP = cos(phi);

      vertices.push_back(radius * cosP * sinT);
      vertices.push_back(radius * cosT);
      vertices.push_back(radius * sinP * sinT);

      normals.push_back(0);
      normals.push_back(1);
      normals.push_back(0);


      int groundColor = (int)((noise.GetNoise(phi, theta) * 2) + 1); /** color from 0 - 4 */
      colors.push_back(col[groundColor].r);
      colors.push_back(col[groundColor].g); 
      colors.push_back(col[groundColor].b);
    }
  } 


  for (int i = 0; i < faces; i++) {
    for (int j = 0; j < faces; j++) {
      int first = i * (faces + 1) + j;
      int second = first + faces + 1;

      indices.push_back(first);
      indices.push_back(second);
      indices.push_back(first + 1);

      indices.push_back(second);
      indices.push_back(second + 1);
      indices.push_back(first + 1);
    }
  }
}

void Sphere::recalcNormals() {
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

void Sphere::render() {
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}