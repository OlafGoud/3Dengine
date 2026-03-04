#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "render.h"
#include "userinput.h"

// ---------------- Globals ----------------
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

/** globals */

// ---------------- Main ----------------
int main() {

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
  glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(getUserCamera().SCR_WIDTH, getUserCamera().SCR_HEIGHT, "Click Grid",nullptr,nullptr);
  if(!window){ std::cout<<"Failed\n"; glfwTerminate(); return -1;}
  glfwMakeContextCurrent(window);

  if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){ std::cout<<"Failed GLAD\n"; return -1;}

  glViewport(0, 0, getUserCamera().SCR_WIDTH, getUserCamera().SCR_HEIGHT);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glEnable(GL_BLEND); /* text */
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); /* text*/

  Shader terrainShader("bin/resources/shaders/terrain.vs", "bin/resources/shaders/terrain.fs");
  Shader textShader("bin/resources/shaders/textshader.vs", "bin/resources/shaders/textshader.fs");
  Shader modelshader("bin/resources/shaders/model.vs","bin/resources/shaders/model.fs");

  getUser().setInputCallbacks(window); 
  getUser().addRenderObject(new RenderTerrain(&terrainShader));
  getUser().addRenderObject(new SphereRenderer(&terrainShader));
  getUser().addRenderObject(new RenderModel(&modelshader));
  getUser().addRenderObject(new RenderUI(&textShader));

  // ---------------- Main Loop ----------------
  while(!glfwWindowShouldClose(window)) {

    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    glClearColor(0.1f,0.1f,0.1f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    getUser().keyboardInput(window, deltaTime);

    for(RenderObject* renderObject : getUser().renderObjects) {
      renderObject->render(getUserCamera());
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}