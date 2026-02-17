#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>
#include "shader.h"
#include "camera.h"
#include "userinput.h"
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const int GRID_SIZE = 40;

// ---------------- Globals ----------------
glm::mat4 projection;
glm::mat4 view;
std::vector<float> vertices; // x,y,z
std::vector<float> colors;   // r,g,b
std::vector<float> normals;  // nx,ny,nz

GLuint VAO, VBO, CBO, NBO;
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;


/** globals */
User user = User();


// ---------------- Main ----------------
int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
  glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(SCR_WIDTH,SCR_HEIGHT,"Click Grid",nullptr,nullptr);
  if(!window){ std::cout<<"Failed\n"; glfwTerminate(); return -1;}
  glfwMakeContextCurrent(window);

  if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){ std::cout<<"Failed GLAD\n"; return -1;}

  glViewport(0,0,SCR_WIDTH,SCR_HEIGHT);
  glEnable(GL_DEPTH_TEST);
  
  user.setInputCallbacks(window);

  // ---------------- Generate Grid (Non-Shared Vertices) ----------------
  float half = GRID_SIZE / 2.0f;
  std::vector<glm::vec3> gridPositions;
  for(int z=0; z<=GRID_SIZE; z++){
    for(int x=0; x<=GRID_SIZE; x++){
      gridPositions.push_back(glm::vec3(x-half, (float)(rand()%10)/20.0f, z-half));
    }
  }

  for(int z=0; z<GRID_SIZE; z++){
    for(int x=0; x<GRID_SIZE; x++){
      int topLeft     =  z      * (GRID_SIZE + 1) + x;
      int topRight    =  topLeft + 1;
      int bottomLeft  = (z + 1) * (GRID_SIZE + 1) + x;
      int bottomRight =  bottomLeft + 1;

      // First triangle
      std::vector<glm::vec3> tri1 = { gridPositions[topLeft], gridPositions[bottomLeft], gridPositions[topRight] };
      glm::vec3 normal1 = glm::normalize(glm::cross(tri1[1]-tri1[0], tri1[2]-tri1[0]));
      for(auto &v : tri1){
        vertices.push_back(v.x); vertices.push_back(v.y); vertices.push_back(v.z);
        colors.push_back(1.0f); colors.push_back(1.0f); colors.push_back(1.0f);
        normals.push_back(normal1.x); normals.push_back(normal1.y); normals.push_back(normal1.z);
      }

      // Second triangle
      std::vector<glm::vec3> tri2 = { gridPositions[topRight], gridPositions[bottomLeft], gridPositions[bottomRight] };
      glm::vec3 normal2 = glm::normalize(glm::cross(tri2[1]-tri2[0], tri2[2]-tri2[0]));
      for(auto &v : tri2){
        vertices.push_back(v.x); vertices.push_back(v.y); vertices.push_back(v.z);
        colors.push_back(1.0f); colors.push_back(1.0f); colors.push_back(1.0f);
        normals.push_back(normal2.x); normals.push_back(normal2.y); normals.push_back(normal2.z);
      }
    }
  }

  // ---------------- Buffers ----------------
  glGenVertexArrays(1,&VAO);
  glGenBuffers(1,&VBO);
  glGenBuffers(1,&CBO);
  glGenBuffers(1,&NBO);

  glBindVertexArray(VAO);

  // Vertex positions
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), vertices.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
  glEnableVertexAttribArray(0);

  // Colors
  glBindBuffer(GL_ARRAY_BUFFER, CBO);
  glBufferData(GL_ARRAY_BUFFER, colors.size()*sizeof(float), colors.data(), GL_DYNAMIC_DRAW);
  glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
  glEnableVertexAttribArray(1);

  // Normals
  glBindBuffer(GL_ARRAY_BUFFER, NBO);
  glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(float), normals.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
  glEnableVertexAttribArray(2);

  glBindVertexArray(0);
  // ---------------- Shader ----------------
  Shader shader("bin/resources/shaders/shader.vs","bin/resources/shaders/shader.fs");
 // glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

  // ---------------- Main Loop ----------------
  while(!glfwWindowShouldClose(window)) {

    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    glClearColor(0.1f,0.1f,0.1f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    user.keyboardInput(window, deltaTime);

    shader.use();
    glm::mat4 model = glm::mat4(1.0f);
    view = user.camera.GetViewMatrix();
    projection = glm::perspective(glm::radians(user.camera.Distance), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    shader.setMat4("uMVP", projection * view * model);
    shader.setMat4("uModel", model);

    // Lighting
    shader.setVec3("lightPos", glm::vec3(30.0f, 30.0f, 30.0f));
    shader.setVec3("viewPos", user.camera.Position);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertices.size()/3);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}