#include "render/renderui.h"
#include <string>

//


UIElementLabel::UIElementLabel(int x, int y, std::string contents) {
  this->x = x;
  this->y = y;
  this->contents = contents;

  this->textRenderer = TextRenderer();
}


void UIElementLabel::render(Shader* shader) {
  this->textRenderer.renderText(shader, this->contents, glm::vec3(1.0f, 0.0f, 0.0f), this->x, this->y, 2);
}


UIElementFPSCounter::UIElementFPSCounter(int x, int y, float* fps) {
  this->x = x;
  this->y = y;
  this->fps = fps;

  this->textRenderer = TextRenderer();
}


void UIElementFPSCounter::render(Shader* shader) {

  std::string fpsString = "FPS: " + std::to_string((int)(1 / *fps));
  this->textRenderer.renderText(shader, fpsString, glm::vec3(1.0f, 0.0f, 0.0f), this->x, this->y, 1);
}



UIElementNumber::UIElementNumber(int x, int y, const int* number) {
  this->x = x;
  this->y = y;
  this->number = number;

  this->textRenderer = TextRenderer();
}


void UIElementNumber::render(Shader* shader) {

  std::string fpsString = std::to_string(*number);
  this->textRenderer.renderText(shader, fpsString, glm::vec3(1.0f, 1.0f, 1.0f), this->x, this->y, 1);
}