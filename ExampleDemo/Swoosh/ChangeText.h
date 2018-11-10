#pragma once
#include "ActionList.h"

class ChangeText : public ActionItem {
private:
  std::string input;
  std::string& output;
public:
  ChangeText(std::string& output, std::string input);

  virtual void update(double elapsed);

  virtual void draw(sf::RenderTexture& surface);
};
