#pragma once
#include <string>
#include "shader.h"
#include "render/textrenderer.h"
/**
 * UI
 */

enum UIElementType {
  NONE,
  LABEL,
  IMAGE,
  FPSCOUNTER,
  NUMBER
};


class UIElement {
/** abstract */
public:
  virtual void render(Shader* shader) = 0;
 // virtual bool hit() = 0;
  
  const UIElementType type = NONE;
  int x, y;
protected:
  TextRenderer textRenderer;
};

class UIElementLabel : public UIElement {

public:
  UIElementLabel(int x, int y, std::string contents);

  void render(Shader* shader);
  void update(std::string contents);
  const UIElementType type = LABEL;

private:
  std::string contents;

};

class UIElementFPSCounter : public UIElement {

public:
  UIElementFPSCounter(int x, int y, float* fps);

  void render(Shader* shader);
  void update(std::string contents);
  const UIElementType type = FPSCOUNTER;

private:
  float* fps;

};

class UIElementNumber : public UIElement {

public:
  UIElementNumber(int x, int y, const int* number);

  void render(Shader* shader);
  void update(std::string contents);
  const UIElementType type = NUMBER;

private:
  const int* number;

};
