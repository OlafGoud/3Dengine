#include "userinput.h"
#include "GLFW/glfw3.h"
#include "camera.h"
#include <iostream>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
  user.mouseScrollInput(yoffset);
}
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
  user.mouseMovementInput(window, static_cast<float>(xposIn), static_cast<float>(yposIn));
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
  /** @todo fix all other things to be updated with the size */
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

bool User::rayTriangleIntersect(const glm::vec3 &orig, const glm::vec3 &dir, const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2, float &t) {
  const float EPSILON = 0.0000001f;
  glm::vec3 edge1 = v1 - v0;
  glm::vec3 edge2 = v2 - v0;
  glm::vec3 h = glm::cross(dir, edge2);
  float a = glm::dot(edge1, h);
  if (a > -EPSILON && a < EPSILON) return false; // parallel
  
  float f = 1.0f / a;
  glm::vec3 s = orig - v0;
  float u = f * glm::dot(s, h);
  if (u < 0.0 || u > 1.0) return false;
  glm::vec3 q = glm::cross(s, edge1);
  float v = f * glm::dot(dir, q);
  if (v < 0.0 || u + v > 1.0) return false;
  t = f * glm::dot(edge2, q);
  if (t > EPSILON) return true;
  return false;
}

void User::calculateClickPosition(GLFWwindow* window) {
  /** check if not in menu etc*/


  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);

  float x = (2.0f * xpos) / SCR_WIDTH - 1.0f;
  float y = 1.0f - (2.0f * ypos) / SCR_HEIGHT;

  glm::vec4 ray_clip = glm::vec4(x, y, -1.0f, 1.0f);
  glm::vec4 ray_eye = glm::inverse(glm::perspective(glm::radians(user.camera.Distance), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f)) * ray_clip;
  ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f);
  glm::vec3 ray_world = glm::normalize(glm::vec3(glm::inverse(this->camera.GetViewMatrix()) * ray_eye));
  float closest_t = 10000.0f;
  int hitTri = -1;

  // Each triangle has 3 unique vertices now
  for (size_t i = 0; i < vertices.size(); i += 9) {
    glm::vec3 v0(vertices[i+0], vertices[i+1], vertices[i+2]);
    glm::vec3 v1(vertices[i+3], vertices[i+4], vertices[i+5]);
    glm::vec3 v2(vertices[i+6], vertices[i+7], vertices[i+8]);

    float t;
    if(this->rayTriangleIntersect(user.camera.Position, ray_world, v0, v1, v2, t)) {
      if(t < closest_t) {
        closest_t = t;
        hitTri = (int)i;
      }
    }
  }

  if(hitTri != -1) {
    // Color the triangle red
    for(int j=0;j<3;j++) {
      colors[hitTri + j*3 + 0] = this->structureColors[structure].r;
      colors[hitTri + j*3 + 1] = this->structureColors[structure].g;
      colors[hitTri + j*3 + 2] = this->structureColors[structure].b;
    }

    // Update color buffer
    glBindBuffer(GL_ARRAY_BUFFER, CBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, colors.size()*sizeof(float), colors.data());
    std::cout << "Triangle clicked: vertices " << hitTri << " to " << hitTri+8 << "\n";
  }
}

const int * User::getStructurePointer() {
  return &this->structure;
}