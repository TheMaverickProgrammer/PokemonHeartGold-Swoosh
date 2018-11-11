#pragma once
#include <Swoosh\Ease.h>
#include "ActionList.h"
#include "ResourceManager.h"
#include "Pokemon.h"

class DisableFlyAction : public ActionItem {
private:
  sf::Sprite& ref;
  pokemon::monster& monster;
public:
  DisableFlyAction(sf::Sprite& ref, pokemon::monster& monster);
  ~DisableFlyAction();

  virtual void update(double elapsed);
  virtual void draw(sf::RenderTexture& surface);
};