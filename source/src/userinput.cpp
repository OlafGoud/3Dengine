#include "userinput.h"
#include "GLFW/glfw3.h"
#include "camera.h"
#include <iostream>


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
  user.mouseScrollInput(yoffset);
}
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
  user.mouseMovementInput(window, static_cast<float>(xposIn), static_cast<float>(yposIn));
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
  user.camera.SCR_WIDTH = width;
  user.camera.SCR_HEIGHT = height;
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
  user.mouseButtonInput(window, button, action, mods);
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
    this->structure == 1 ? this->eventManager.removeEvent("Color_Blue") : this->eventManager.setEvent("Color_Blue");
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

void User::mouseScrollInput(double yoffset) {
  this->camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void User::mouseMovementInput(GLFWwindow* window, float xposIn, float yposIn) {
  if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
    this->camera.ProcessMouseRotation(xposIn - lastX, lastY - yposIn);
  }
  if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
    this->camera.ProcessMouseXYMovement(yposIn - lastY, lastX - xposIn);
  }

  lastX = static_cast<float>(xposIn);
  lastY = static_cast<float>(yposIn);

}

void User::mouseButtonInput(GLFWwindow* window, int button, int action, int mods) {
  if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    if(this->structure > 0 && this->structure < 3) {
      /** build structure */
      //calculateClickPosition(window);
      double xpos, ypos;
      glfwGetCursorPos(window, &xpos, &ypos);



      /** @todo iterate over all begin ui and end at terrain */
      this->renderObjects.at(0)->checkForClick(xpos, ypos, user.camera);
    }
  }
  
  if(button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {

  } 
  
  if(button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
    
  }
}

const int * User::getStructurePointer() {
  return &this->structure;
}