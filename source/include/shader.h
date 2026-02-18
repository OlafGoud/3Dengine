
#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <glm/glm.hpp>
#include <glad/glad.h>

GLuint getCurrentShader();

class Shader {
  public:
  Shader(const char* vertexPath, const char* fragmentPath);


  void use();
  void setBool(const std::string &name, bool value) const;
  void setInt(const std::string &name, int value) const;
  void setFloat(const std::string &name, float value) const; 
  void setVec3(const std::string &name, glm::vec3 value) const;
  void setVec3(const std::string &name, float x, float y, float z) const;
  void setMat4(const std::string &name, glm::mat4 mat) const;
  GLuint getID() const;
  private:
  GLuint ID;
  

};