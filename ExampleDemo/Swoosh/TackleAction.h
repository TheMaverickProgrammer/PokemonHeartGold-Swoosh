#pragma once
#include <Swoosh\Ease.h>
#include "ResourceManager.h"
#include "ActionList.h"

class TackleAction : public BlockingActionItem {
private:
  sf::Sprite& ref;
  int facing;
  double total;
  sf::Vector2f original;
public:
  TackleAction(sf::Sprite& ref, int facing);
  ~TackleAction();

  virtual void update(double elapsed);
  virtual void draw(sf::RenderTexture& surface);
};