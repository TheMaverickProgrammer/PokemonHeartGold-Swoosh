#pragma once
#include <SFML\Graphics.hpp>
#include "ActionList.h"

class BobAction : public ActionItem {
private:
  sf::Sprite& ref;
  double total;
  sf::Vector2f bob;
public:
  BobAction(sf::Sprite& ref);

  ~BobAction();

  virtual void update(double elapsed);
  virtual void draw(sf::RenderTexture& surface);
};
