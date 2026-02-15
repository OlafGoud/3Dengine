// main.cpp
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <iostream>

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/trigonometric.hpp"
#include "shader.h"
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const int GRID_SIZE = 10;

// ---------------- Main ----------------

int main() {
  /** init glwf */
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Grid", nullptr, nullptr);
  if(!window) {
    std::cout << "Failed to create window\n";
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  /** load glad */
  if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to init GLAD\n";
    glfwTerminate();
    return -1;
  }

  glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
  glEnable(GL_DEPTH_TEST);

  /** gen grid */
  std::vector<float> vertices;
  std::vector<unsigned int> indices;

  float half = GRID_SIZE / 2.0f;
  // Generate vertices (11x11 grid of points)
  for (int z = 0; z <= GRID_SIZE; ++z) {
    for (int x = 0; x <= GRID_SIZE; ++x) {
        vertices.push_back(x - half);
        vertices.push_back((float)(rand() % 10) / 20);
        vertices.push_back(z - half);
    }
  }

  // Generate indices (2 triangles per quad)
  for (int z = 0; z < GRID_SIZE; ++z) {
    for (int x = 0; x < GRID_SIZE; ++x) {
      int topLeft     =  z      * (GRID_SIZE + 1) + x;
      int topRight    =  topLeft + 1;
      int bottomLeft  = (z + 1) * (GRID_SIZE + 1) + x;
      int bottomRight =  bottomLeft + 1;

      indices.push_back(topLeft);
      indices.push_back(bottomLeft);
      indices.push_back(topRight);

      indices.push_back(topRight);
      indices.push_back(bottomLeft);
      indices.push_back(bottomRight);
    }
  }

  /** GPU SHIT */
  GLuint VAO, VBO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);

  Shader shader("bin/resources/shaders/shader.vs", "bin/resources/shaders/shader.fs");
  shader.use();
  
  /** polyline mode */
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  /** camera shit */

  glm::vec3 position = glm::vec3(8.0f, 8.0f, 8.0f);
  glm::vec3 lookAt = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);



  while(!glfwWindowShouldClose(window)) {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::lookAt(position, lookAt, up);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.0f, 100.0f);

    glm::mat4 mvp = projection * view * model;

    shader.setMat4("uMVP", mvp);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();

  }

  glfwTerminate();
  return 0;
}
