#include "render/textrenderer.h"

#include <ft2build.h>
#include <string>
#ifndef FT_FREETYPE_H
#include FT_FREETYPE_H
#endif
#include <iostream>

Character characterList[128];

TextRenderer::TextRenderer() {

  if (FT_Init_FreeType(&ft)) { std::cerr << "Could not init FreeType\n"; return;}

  FT_Face face;
  if (FT_New_Face(ft, "bin/resources/fonts/OpenSans-Regular.ttf", 0, &face)) {
    std::cerr << "Failed to load font\n"; return;
  }
  FT_Set_Pixel_Sizes(face, 0, 48);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  for (unsigned char c = 0; c < 128; ++c) {
    if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
      std::cerr << "Failed to load Glyph\n"; continue;
    }

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    Character character = {
      tex,
      glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
      glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
      static_cast<GLuint>(face->glyph->advance.x)
    };
    characterList[c] = character;
  }
  FT_Done_Face(face);
  FT_Done_FreeType(ft);

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

}

void TextRenderer::renderText(Shader* shader, std::string text, glm::vec3 color, int x, int y, int scale) {
  shader->use();
  shader->setVec3("textColor", color);
  glm::mat4 projection = glm::ortho(0.0f, 1920.0f, 0.0f, 1080.0f);
  shader->setMat4("projection", projection);
  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(VAO);
  for (char c : text) {
    Character ch = characterList[c];

    GLfloat xpos = x + ch.Bearing.x * scale;
    GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

    GLfloat w = ch.Size.x * scale;
    GLfloat h = ch.Size.y * scale;

    GLfloat vertices[6][4] = {
      { xpos,     ypos + h,   0.0f, 0.0f },
      { xpos,     ypos,       0.0f, 1.0f },
      { xpos + w, ypos,       1.0f, 1.0f },

      { xpos,     ypos + h,   0.0f, 0.0f },
      { xpos + w, ypos,       1.0f, 1.0f },
      { xpos + w, ypos + h,   1.0f, 0.0f }
    };

    glBindTexture(GL_TEXTURE_2D, ch.TextureID);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    x += (ch.Advance >> 6) * scale; // advance in pixels
  }

  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}