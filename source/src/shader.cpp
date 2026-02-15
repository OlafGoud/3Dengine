#include "shader.h"
#include <fstream>
#include <sstream>
#include <iostream>


static GLuint currentShader = 0;


GLuint getCurrentShader() {
  return currentShader;
}


void Shader::use() {
  glUseProgram(this->ID);
}
void Shader::setBool(const std::string &name, bool value) const {
  if(currentShader != this->ID) {
    glUseProgram(this->ID);
  }
  glUniform1i(glGetUniformLocation(this->ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const {
  if(currentShader != this->ID) {
    glUseProgram(this->ID);
  }
  glUniform1i(glGetUniformLocation(this->ID, name.c_str()), value);
}
void Shader::setFloat(const std::string &name, float value) const {
  if(currentShader != this->ID) {
    glUseProgram(this->ID);
  }
  glUniform1f(glGetUniformLocation(this->ID, name.c_str()), value);
}
void Shader::setVec3(const std::string &name, glm::vec3 value) const {
  if(currentShader != this->ID) {
    glUseProgram(this->ID);
  }
  glUniform3fv(glGetUniformLocation(this->ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec3(const std::string &name, float x, float y, float z) const {
  if(currentShader != this->ID) {
    glUseProgram(this->ID);
  }
  glUniform3f(glGetUniformLocation(this->ID, name.c_str()), x, y, z);
}

void Shader::setMat4(const std::string &name, glm::mat4 mat) const {
  if(currentShader != this->ID) {
    glUseProgram(this->ID);
  }

  glUniformMatrix4fv(glad_glGetUniformLocation(this->ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}






Shader::Shader(const char* vertexPath, const char* fragmentPath) {

  std::string vertexCode, fragmentCode;
  std::ifstream vShaderFile, fShaderFile;

  vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
  fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);

  try {
    vShaderFile.open(vertexPath);
    fShaderFile.open(fragmentPath);

    std::stringstream vShaderStream, fShaderStream;

    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();

    vShaderFile.close();
    fShaderFile.close();

    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();
  } catch (std::ifstream::failure &e) {
    std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ\n";
  } 

  const char* vShaderCode = vertexCode.c_str();
  const char* fshaderCode = fragmentCode.c_str();

  unsigned int vertex, fragment;
  int success;
  char infoLog[512];

  vertex = glad_glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vShaderCode, NULL);
  glCompileShader(vertex);

  glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
  if(!success) {
    glGetShaderInfoLog(vertex, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
  }

  fragment = glad_glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fshaderCode, NULL);
  glCompileShader(fragment);

  glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
  if(!success) {
    glGetShaderInfoLog(fragment, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
  }

  this->ID = glCreateProgram();
  glAttachShader(ID, vertex);
  glAttachShader(ID, fragment);
  glLinkProgram(ID);

  glGetProgramiv(ID, GL_LINK_STATUS, &success);
  if(!success)
  {
    glGetProgramInfoLog(ID, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
  }

  glDeleteShader(vertex);
  glDeleteShader(fragment);
}