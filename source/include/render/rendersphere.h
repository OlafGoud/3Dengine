#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


class Sphere {
public:
  Sphere(glm::vec3 position, float radius, int faces);
  void render();
  void genGPUBuffers();


  glm::vec3 position;

private:
  void genSphere(float radius, int faces);

  void recalcNormals();

  float radius;
  std::vector<float> vertices;
  std::vector<float> normals;
  std::vector<float> colors;
  std::vector<unsigned int> indices;
  GLuint VAO, VBO, NBO, CBO, EBO;


};