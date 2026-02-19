#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"

struct Character {
  GLuint TextureID;
  glm::ivec2 Size;
  glm::ivec2 Bearing;
  GLuint Advance;
};

class TextRenderer {
public:
  TextRenderer();
  void renderText(Shader* shader, std::string text, glm::vec3 color, int x, int y, int scale);
private:
  FT_Library ft;
  GLuint VAO, VBO;
};