#include "model.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>

void Model::Mesh::setup() {
  // compute bounding sphere
  for (auto &v : vertices)
    boundingRadius = std::max(boundingRadius, glm::length(v.pos));

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  glGenBuffers(1, &instanceVBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

  // pos
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);

  // normal
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));

  // uv
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, uv));

  // instance matrix (mat4 = 4 vec4)
  glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);

  for (int i = 0; i < 4; i++) {
    glEnableVertexAttribArray(3 + i);
    glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)(sizeof(glm::vec4) * i));
    glVertexAttribDivisor(3 + i, 1);
  }

  glBindVertexArray(0);
}

void Model::Mesh::updateInstances(const std::vector<glm::mat4> &mats) {
  glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
  glBufferData(GL_ARRAY_BUFFER, mats.size() * sizeof(glm::mat4), mats.data(), GL_DYNAMIC_DRAW);
}

void Model::Mesh::draw(GLuint shader, size_t count) {
  glUseProgram(shader);

  if (diffuseTex) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseTex);
    glUniform1i(glGetUniformLocation(shader, "texture_diffuse0"), 0);
    std::cout << "texture ";
  }
  glBindVertexArray(VAO);
  glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, count);
  glBindVertexArray(0);
  std::cout << "added, " << count << "\n";
}

void Model::drawInstancedCulled(const std::vector<glm::mat4> &instances, const glm::mat4 &viewProj, GLuint shader) {
  Frustum fr;
  fr.update(viewProj);

  std::vector<glm::mat4> visible;

  for (const auto &m : instances) {
    glm::vec3 pos = glm::vec3(m[3]);

    if (meshes[0].boundingRadius <= 0 ||
        fr.visible(pos, meshes[0].boundingRadius)) {
      visible.push_back(m);
    }
  }

  for (auto &mesh : meshes) {
    mesh.updateInstances(visible);
    mesh.draw(shader, visible.size());
  }
}

void Model::Frustum::update(const glm::mat4 &vp) {
  planes[0] = vp[3] + vp[0];
  planes[1] = vp[3] - vp[0];
  planes[2] = vp[3] + vp[1];
  planes[3] = vp[3] - vp[1];
  planes[4] = vp[3] + vp[2];
  planes[5] = vp[3] - vp[2];

  for (int i = 0; i < 6; i++) planes[i] /= glm::length(glm::vec3(planes[i]));
}

bool Model::Frustum::visible(glm::vec3 c, float r) const {
  for (int i = 0; i < 6; i++)
    if (glm::dot(glm::vec3(planes[i]), c) + planes[i].w < -r) 
      return false;

  return true;
}
void Model::load(const std::string& path) {
  directory = path.substr(0, path.find_last_of("/\\"));

  Assimp::Importer importer;

  const aiScene* scene =
      importer.ReadFile(path,
      aiProcess_Triangulate |
      aiProcess_GenNormals |
      aiProcess_FlipUVs);

  if(!scene || !scene->mRootNode)
      throw std::runtime_error("OBJ load failed");

  processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene) {
  for (unsigned int i = 0; i < node->mNumMeshes; i++) {
    aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
    meshes.push_back(processMesh(mesh, scene));
  }

  for (unsigned int i = 0; i < node->mNumChildren; i++)
    processNode(node->mChildren[i], scene);
}

Model::Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
  Mesh m;

  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    Vertex v;
    v.pos = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};

    v.normal = mesh->HasNormals() ? glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z) : glm::vec3(0);

    if (mesh->mTextureCoords[0])
      v.uv = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
    else
      v.uv = glm::vec2(0);

    m.vertices.push_back(v);
  }

  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    aiFace f = mesh->mFaces[i];
    for (unsigned int j = 0; j < f.mNumIndices; j++)
      m.indices.push_back(f.mIndices[j]);
  }

  // diffuse texture
  if (mesh->mMaterialIndex >= 0) {
    aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];

    if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
      aiString str;
      mat->GetTexture(aiTextureType_DIFFUSE, 0, &str);

      std::string texPath = str.C_Str();

      if (loadedTextures.count(texPath))
        m.diffuseTex = loadedTextures[texPath];
      else {
        std::string fullPath = directory + "/" + texPath;
        GLuint id = loadTexture(fullPath);        
        loadedTextures[texPath] = id;
        m.diffuseTex = id;
      }
    }
  }

  m.setup();
  return m;
}

GLuint Model::loadTexture(const std::string &file) {
  int w, h, n;
  unsigned char *data = stbi_load(file.c_str(), &w, &h, &n, 0);

  if (!data) {
    std::cout << "Texture failed\n";
    return 0;
  }

  GLenum format = n == 4 ? GL_RGBA : GL_RGB;

  GLuint tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);

  glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);

  glGenerateMipmap(GL_TEXTURE_2D);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  stbi_image_free(data);
  return tex;
}
