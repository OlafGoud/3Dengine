#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>
#include <string>
#include <unordered_map>


class Model {
public:
  struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 uv;
  };

  struct Mesh {
    GLuint VAO = 0, VBO = 0, EBO = 0;
    GLuint instanceVBO = 0;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    GLuint diffuseTex = 0;

    float boundingRadius = 0.0f;

    void setup();

    void updateInstances(const std::vector<glm::mat4> &mats);

    void draw(GLuint shader, size_t count);
  };

  // --------------------------

  Model(const std::string &path) { load(path); }

  void drawInstancedCulled(const std::vector<glm::mat4> &instances, const glm::mat4 &viewProj, GLuint shader);
  std::string directory;

private:
  std::vector<Mesh> meshes;
  std::unordered_map<std::string, GLuint> loadedTextures;

  // --------------------------

  struct Frustum {
    glm::vec4 planes[6];

    void update(const glm::mat4 &vp);

    bool visible(glm::vec3 c, float r) const;
  };

  // --------------------------

  void load(const std::string &path);

  void processNode(aiNode *node, const aiScene *scene);

  Mesh processMesh(aiMesh *mesh, const aiScene *scene);

  GLuint loadTexture(const std::string &file);
};
