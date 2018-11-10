#pragma once
#include <Swoosh\Ease.h>
#include "ActionList.h"
#include "ResourceManager.h"
#include "Pokemon.h"

class FlyAction : public BlockingActionItem {
private:
  sf::Sprite& ref;
  pokemon::monster& monster;
  double total;
  sf::SoundBuffer& buffer;
  sf::Sound& sound;
  bool doOnce;
public:
  FlyAction(sf::Sprite& ref, pokemon::monster& monster, sf::SoundBuffer& buffer, sf::Sound& sound);
  ~FlyAction();

  virtual void update(double elapsed);
  virtual void draw(sf::RenderTexture& surface);
};
