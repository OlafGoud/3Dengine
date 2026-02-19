#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>
#include "glm/ext/matrix_transform.hpp"
#include "camera.h"
#include "render.h"
#include "model.h"
#include "userinput.h"
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const int GRID_SIZE = 100;

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
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  Shader terrainShader("bin/resources/shaders/terrain.vs", "bin/resources/shaders/terrain.fs");
  Shader textShader("bin/resources/shaders/textshader.vs", "bin/resources/shaders/textshader.fs");

  user.setInputCallbacks(window);
  user.renderObjects.push_back(new RenderTerrain(&terrainShader));
  user.renderObjects.push_back(new RenderUI(&textShader));

  
  Shader modelshader("bin/resources/shaders/model.vs","bin/resources/shaders/model.fs");
  
  /** models */
  Model castle("bin/resources/models/castle.obj");

  std::vector<glm::mat4> transforms;

  for(int i=0;i<5;i++) {
    glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(rand() %10 - 5,0,rand()%10 - 5));
    m = glm::scale(m, glm::vec3(3.0f));
    transforms.push_back(m);
  }
  glDisable(GL_CULL_FACE);

  // ---------------- Main Loop ----------------
  while(!glfwWindowShouldClose(window)) {

    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
//    std::cout << "framerate: " << (1/deltaTime) << "\n";

    glClearColor(0.1f,0.1f,0.1f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    user.keyboardInput(window, deltaTime);

    /** models */
    modelshader.use();
    glUniformMatrix4fv(glGetUniformLocation(modelshader.getID(), "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(modelshader.getID(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    castle.drawInstanced(transforms, modelshader.getID());

    for(RenderObject* renderObject : user.renderObjects) {
      renderObject->render(user.camera);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}