#pragma once
#ifndef GUARD1
#define GUARD1
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "stb_image.h"

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

// --------------------- Vertex / Mesh ---------------------
struct Vertex {
  glm::vec3 pos;
  glm::vec3 normal;
  glm::vec2 uv;
};

struct Mesh {
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  GLuint diffuseTex = 0;
  GLuint VAO = 0, VBO = 0, EBO = 0, instanceVBO = 0;

  void setup() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &instanceVBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
                 vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, uv));

    // instance matrix
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    for (int i = 0; i < 4; i++) {
      glEnableVertexAttribArray(3 + i);
      glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                            (void *)(sizeof(glm::vec4) * i));
      glVertexAttribDivisor(3 + i, 1);
    }

    glBindVertexArray(0);
  }

  void updateInstances(const std::vector<glm::mat4> &mats) {
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, mats.size() * sizeof(glm::mat4), mats.data(),
                 GL_DYNAMIC_DRAW);
  }

  void draw(GLuint shader, size_t count) {
    glUseProgram(shader);
    if (diffuseTex) {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, diffuseTex);
      glUniform1i(glGetUniformLocation(shader, "texture_diffuse0"), 0);
    }
    glBindVertexArray(VAO);
    glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0,
                            count);
    glBindVertexArray(0);
  }
};

// --------------------- Model ---------------------
struct Model {
  std::vector<Mesh> meshes;
  std::string directory;
  std::unordered_map<std::string, GLuint> loadedTextures;

  Model(const std::string &path) { load(path); }
  void drawInstanced(const std::vector<glm::mat4> &instances, GLuint shader) {
    for (auto &m : meshes) {
      m.updateInstances(instances);
      m.draw(shader, instances.size());
    }
  }

private:
  void load(const std::string &path) {
    directory = path.substr(0, path.find_last_of("/\\"));
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(
        path, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs);
    if (!scene || !scene->mRootNode)
      throw std::runtime_error("OBJ load failed");
    processNode(scene->mRootNode, scene);
  }

  void processNode(aiNode *node, const aiScene *scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
      aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
      meshes.push_back(processMesh(mesh, scene));
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++)
      processNode(node->mChildren[i], scene);
  }

  Mesh processMesh(aiMesh *mesh, const aiScene *scene) {
    Mesh m;
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
      Vertex v;
      v.pos = {mesh->mVertices[i].x, mesh->mVertices[i].y,
               mesh->mVertices[i].z};
      v.normal = mesh->HasNormals()
                     ? glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y,
                                 mesh->mNormals[i].z)
                     : glm::vec3(0);
      v.uv = mesh->mTextureCoords[0] ? glm::vec2(mesh->mTextureCoords[0][i].x,
                                                 mesh->mTextureCoords[0][i].y)
                                     : glm::vec2(0);
      m.vertices.push_back(v);
    }
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
      aiFace f = mesh->mFaces[i];
      for (unsigned int j = 0; j < f.mNumIndices; j++)
        m.indices.push_back(f.mIndices[j]);
    }

    if (mesh->mMaterialIndex >= 0) {
      aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];
      if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
        aiString str;
        mat->GetTexture(aiTextureType_DIFFUSE, 0, &str);
        std::string texPath = str.C_Str();
        if (loadedTextures.count(texPath))
          m.diffuseTex = loadedTextures[texPath];
        else {
          std::string full = directory + "/" + texPath;
          GLuint id = loadTexture(full);
          loadedTextures[texPath] = id;
          m.diffuseTex = id;
        }
      }
    }

    m.setup();
    return m;
  }

  GLuint loadTexture(const std::string &file) {
    int w, h, n;
    unsigned char *data = stbi_load(file.c_str(), &w, &h, &n, 0);
    if (!data) {
      std::cout << "Failed to load " << file << "\n";
      return 0;
    }
    GLenum format = n == 4 ? GL_RGBA : GL_RGB;
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE,
                 data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(data);
    return tex;
  }
};


class ModelObject {
public:
  ModelObject(glm::vec3 position, Model* model) {
    this->model = model;
    this->positions.insert({makeKey(position.x, position.z), glm::translate(glm::mat4(1.0f), position)});

  }

  void render(GLuint shader) {

    std::vector<glm::mat4> values;
    values.reserve(positions.size());
    for(auto it : positions) {
      values.push_back(it.second);
    }

    model->drawInstanced(values, shader);
  }

  long long makeKey(int x, int z) {
    return ((long long) x << 32) | (unsigned int)z;
  }

  glm::mat4* getPosition(int x, int z) {

    auto it = positions.find(makeKey(x, z));
    if(it != positions.end()) return &it->second;

    return nullptr;
  }
  


private:
  std::unordered_map<long long, glm::mat4> positions;
  Model* model;
};

#endif