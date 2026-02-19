#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>
#include "render.h"
#include "userinput.h"

// ---------------- Globals ----------------
glm::mat4 projection;
glm::mat4 view;

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

  GLFWwindow* window = glfwCreateWindow(user.camera.SCR_WIDTH, user.camera.SCR_HEIGHT, "Click Grid",nullptr,nullptr);
  if(!window){ std::cout<<"Failed\n"; glfwTerminate(); return -1;}
  glfwMakeContextCurrent(window);

  if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){ std::cout<<"Failed GLAD\n"; return -1;}

  glViewport(0, 0, user.camera.SCR_WIDTH, user.camera.SCR_HEIGHT);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glEnable(GL_BLEND); /* text */
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); /* text*/

  Shader terrainShader("bin/resources/shaders/terrain.vs", "bin/resources/shaders/terrain.fs");
  Shader textShader("bin/resources/shaders/textshader.vs", "bin/resources/shaders/textshader.fs");
  Shader modelshader("bin/resources/shaders/model.vs","bin/resources/shaders/model.fs");

  user.setInputCallbacks(window);
  user.renderObjects.push_back(new RenderTerrain(&terrainShader));
  user.renderObjects.push_back(new RenderUI(&textShader));
  user.renderObjects.push_back(new RenderModel(&modelshader));
    
  // ---------------- Main Loop ----------------
  while(!glfwWindowShouldClose(window)) {

    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    glClearColor(0.1f,0.1f,0.1f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    user.keyboardInput(window, deltaTime);

    for(RenderObject* renderObject : user.renderObjects) {
      renderObject->render(user.camera);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}