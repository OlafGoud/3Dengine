#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "camera.h"
#include "userinput.h"

#include "event/inputevents.h"
#include "eventmanager.h"

User user = User();
inline User& getUser() {
  return user;
}

inline Camera& getUserCamera() {
  return user.getCamera();
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
  getEventManager().activateEvent(MouseScrollEvent(window, &getUserCamera(), static_cast<float>(xoffset), static_cast<float>(yoffset)));
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
  getEventManager().activateEvent(MouseMoveEvent(window, &getUserCamera(), static_cast<float>(xposIn), static_cast<float>(yposIn)));
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
  getEventManager().activateEvent(MouseButtonEvent(window, &getUserCamera(), button, action, mods));
}

void keyboard_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  getEventManager().activateEvent(KeyBoardEvent(window, key, scancode, action, mods));
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
  getUserCamera().SCR_WIDTH = width;
  getUserCamera().SCR_HEIGHT = height;
}

User::User() {
  this->camera = Camera(glm::vec3(0.0f, 10.0f, 1.0f));

  /** Red Green Blue */
  this->structureColors[1] = glm::vec3(0.2f, 0.8f, 0.1);
  this->structureColors[2] = glm::vec3(0.05f, 0.1f, 1.0f);
}

void User::setInputCallbacks(GLFWwindow *window) {
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetKeyCallback(window, keyboard_key_callback);
}

void User::addRenderObject(RenderObject* obj) {
  renderObjects.push_back(obj); 
}



void User::keyboardInput(GLFWwindow *window, float deltaTime) {

  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    /** settings menu / quit */
    glfwSetWindowShouldClose(window, true);
  }

  /** movement */
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    this->camera.ProcessKeyboard(FORWARD, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    this->camera.ProcessKeyboard(BACKWARD, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    this->camera.ProcessKeyboard(LEFT, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    this->camera.ProcessKeyboard(RIGHT, deltaTime);
  }

  if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && this->pressedKeys[GLFW_KEY_P] == 0) {
    static int polyMode = 0;
    if(polyMode == 1) {
      glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
      polyMode = 0;
    } else {
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
      polyMode = 1;
    }
    this->pressedKeys[GLFW_KEY_P] = 1;
  }

  
  if(glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && this->pressedKeys[GLFW_KEY_1] == 0) {
    

    this->structure == 1 ? this->structure = -1 : this->structure = 1;
    std::cout << "number: " << this->structure << "\n";
    this->pressedKeys[GLFW_KEY_1] = 1;
    
  }
  if(glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && this->pressedKeys[GLFW_KEY_2] == 0) {
    this->structure == 2 ? this->structure = -1 : this->structure = 2;
    std::cout << "number: " << this->structure << "\n";
    this->pressedKeys[GLFW_KEY_2] = 1;
  }

  if(glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE) {
    this->pressedKeys[GLFW_KEY_1] = 0;
  }
  if(glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE) {
    this->pressedKeys[GLFW_KEY_2] = 0;
  }
  if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE) {
    this->pressedKeys[GLFW_KEY_P] = 0;
  }

}


inline Camera& User::getCamera() {
  return camera;
}

const int* User::getStructurePointer() {
  return &this->structure;
}