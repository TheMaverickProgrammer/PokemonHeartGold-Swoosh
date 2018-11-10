#pragma once
#include <SFML\Graphics.hpp>
#include "ActionList.h"

class IdleAction : public ActionItem {
private:
  sf::Sprite& ref;

public:
  IdleAction(sf::Sprite& ref);
  virtual void update(double elapsed);
  virtual void draw(sf::RenderTexture& surface);
};