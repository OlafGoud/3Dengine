#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#define CHUNK_SIZE 20



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
  bool rayTriangleIntersect(const glm::vec3 &orig, const glm::vec3 &dir, const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2, float &t);


  std::vector<float> vertices;
  std::vector<float> normals;
  std::vector<float> colors;
  std::vector<unsigned int> indices;
  GLuint VAO, VBO, NBO, CBO, EBO;
};
