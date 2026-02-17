```cpp
// main.cpp
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

const int SCR_WIDTH = 1000;
const int SCR_HEIGHT = 800;

//////////////////////////////////////////////////////////////
// Simple Shader
//////////////////////////////////////////////////////////////

GLuint createShader(const char *vs, const char *fs) {
  GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vs, NULL);
  glCompileShader(vertex);

  GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fs, NULL);
  glCompileShader(fragment);

  GLuint program = glCreateProgram();
  glAttachShader(program, vertex);
  glAttachShader(program, fragment);
  glLinkProgram(program);

  glDeleteShader(vertex);
  glDeleteShader(fragment);

  return program;
}

bool rayTriangleIntersect(const glm::vec3 &orig, const glm::vec3 &dir,
                          const glm::vec3 &v0, const glm::vec3 &v1,
                          const glm::vec3 &v2, float &t) {
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

//////////////////////////////////////////////////////////////
// RenderGrid Class
//////////////////////////////////////////////////////////////

class RenderGrid {
public:
  int GRID_SIZE;
  std::vector<float> vertices;
  std::vector<float> normals;
  std::vector<float> colors;
  std::vector<unsigned int> indices;

  GLuint VAO, VBO, NBO, CBO, EBO;

  RenderGrid(int size) {
    GRID_SIZE = size;
    float half = GRID_SIZE / 2.0f;

    for (int z = 0; z <= GRID_SIZE; z++) {
      for (int x = 0; x <= GRID_SIZE; x++) {
        vertices.push_back(x - half);
        vertices.push_back(0.0f);
        vertices.push_back(z - half);

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
        int tl = z * (GRID_SIZE + 1) + x;
        int tr = tl + 1;
        int bl = (z + 1) * (GRID_SIZE + 1) + x;
        int br = bl + 1;

        indices.insert(indices.end(), {tl, bl, tr, tr, bl, br});
      }
    }

    recalcNormals();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &NBO);
    glGenBuffers(1, &CBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
                 vertices.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, NBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float),
                 normals.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, CBO);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(),
                 GL_DYNAMIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
  }
  bool pickSquare(const glm::vec3 &rayOrigin, const glm::vec3 &rayDir,
                  int &outX, int &outZ) {
    float closest = 999999.0f;
    bool hit = false;

    for (int z = 0; z < GRID_SIZE; z++) {
      for (int x = 0; x < GRID_SIZE; x++) {
        int tl = z * (GRID_SIZE + 1) + x;
        int tr = tl + 1;
        int bl = (z + 1) * (GRID_SIZE + 1) + x;
        int br = bl + 1;

        glm::vec3 v0(vertices[tl * 3], vertices[tl * 3 + 1],
                     vertices[tl * 3 + 2]);
        glm::vec3 v1(vertices[bl * 3], vertices[bl * 3 + 1],
                     vertices[bl * 3 + 2]);
        glm::vec3 v2(vertices[tr * 3], vertices[tr * 3 + 1],
                     vertices[tr * 3 + 2]);

        float t;
        if (rayTriangleIntersect(rayOrigin, rayDir, v0, v1, v2, t)) {
          if (t < closest) {
            closest = t;
            outX = x;
            outZ = z;
            hit = true;
          }
        }

        v0 = glm::vec3(vertices[tr * 3], vertices[tr * 3 + 1],
                       vertices[tr * 3 + 2]);
        v1 = glm::vec3(vertices[bl * 3], vertices[bl * 3 + 1],
                       vertices[bl * 3 + 2]);
        v2 = glm::vec3(vertices[br * 3], vertices[br * 3 + 1],
                       vertices[br * 3 + 2]);

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

  void recalcNormals() {
    std::fill(normals.begin(), normals.end(), 0.0f);

    for (size_t i = 0; i < indices.size(); i += 3) {
      unsigned int i0 = indices[i];
      unsigned int i1 = indices[i + 1];
      unsigned int i2 = indices[i + 2];

      glm::vec3 v0(vertices[i0 * 3], vertices[i0 * 3 + 1],
                   vertices[i0 * 3 + 2]);
      glm::vec3 v1(vertices[i1 * 3], vertices[i1 * 3 + 1],
                   vertices[i1 * 3 + 2]);
      glm::vec3 v2(vertices[i2 * 3], vertices[i2 * 3 + 1],
                   vertices[i2 * 3 + 2]);

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

  void raise(int x, int z) {
    int tl = z * (GRID_SIZE + 1) + x;
    int tr = tl + 1;
    int bl = (z + 1) * (GRID_SIZE + 1) + x;
    int br = bl + 1;

    for (int v : {tl, tr, bl, br})
      vertices[v * 3 + 1] += 0.3f;

    recalcNormals();

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float),
                    vertices.data());

    glBindBuffer(GL_ARRAY_BUFFER, NBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, normals.size() * sizeof(float),
                    normals.data());
  }

  void draw() {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
  }
  void setColor(int x, int z, glm::vec3 c) {
    int tl = z * (GRID_SIZE + 1) + x;
    int tr = tl + 1;
    int bl = (z + 1) * (GRID_SIZE + 1) + x;
    int br = bl + 1;

    for (int v : {tl, tr, bl, br}) {
      colors[v * 3] = c.r;
      colors[v * 3 + 1] = c.g;
      colors[v * 3 + 2] = c.b;
    }

    glBindBuffer(GL_ARRAY_BUFFER, CBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, colors.size() * sizeof(float),
                    colors.data());
  }
};

//////////////////////////////////////////////////////////////

int main() {
  glfwInit();
  GLFWwindow *window =
      glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Terrain", NULL, NULL);
  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

  glEnable(GL_DEPTH_TEST);

  const char *vs = R"(
        #version 330 core
        layout(location=0) in vec3 aPos;
        layout(location=1) in vec3 aNormal;
        layout(location=2) in vec3 aColor;

        out vec3 FragPos;
        out vec3 Normal;
        out vec3 Color;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        void main()
        {
            FragPos = vec3(model * vec4(aPos,1.0));
            Normal = mat3(transpose(inverse(model))) * aNormal;
            Color = aColor;
            gl_Position = projection * view * vec4(FragPos,1.0);
        }
    )";

  const char *fs = R"(
        #version 330 core
        in vec3 FragPos;
        in vec3 Normal;
        in vec3 Color;

        out vec4 FragColor;

        void main()
        {
            vec3 lightDir = normalize(vec3(-1.0,2.0,1.0));
            float diff = max(dot(normalize(Normal),lightDir),0.0);
            vec3 lighting = (0.2 + diff) * Color;
            FragColor = vec4(lighting,1.0);
        }
    )";

  GLuint shader = createShader(vs, fs);

  RenderGrid grid(32);

  glm::mat4 model = glm::mat4(1.0f);
  glm::mat4 view =
      glm::lookAt(glm::vec3(0, 20, 20), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
  glm::mat4 projection = glm::perspective(
      glm::radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);

  while (!glfwWindowShouldClose(window)) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
      if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        float x = (2.0f * xpos) / SCR_WIDTH - 1.0f;
        float y = 1.0f - (2.0f * ypos) / SCR_HEIGHT;

        glm::vec4 ray_clip(x, y, -1.0f, 1.0f);

        glm::vec4 ray_eye = glm::inverse(projection) * ray_clip;
        ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f);

        glm::vec3 ray_world =
            glm::normalize(glm::vec3(glm::inverse(view) * ray_eye));

        glm::vec3 cameraPos = glm::vec3(0, 20, 20);

        int sqX, sqZ;
        if (grid.pickSquare(cameraPos, ray_world, sqX, sqZ)) {
          grid.raise(sqX, sqZ);

          // also change color
          grid.setColor(sqX, sqZ, glm::vec3(1, 0, 0));
        }
      }
    }

    glClearColor(0.2, 0.3, 0.5, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE,
                       glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE,
                       glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE,
                       glm::value_ptr(projection));

    grid.draw();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}

```