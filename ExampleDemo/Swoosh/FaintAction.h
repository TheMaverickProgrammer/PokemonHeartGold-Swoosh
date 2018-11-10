#pragma once
#include <Swoosh\Ease.h>
#include <Swoosh\Game.h>
#include "ActionList.h"
#include "ResourceManager.h"

class FaintAction : public BlockingActionItem {
private:
  sf::Sprite& ref;
  sf::SoundBuffer &buffer;
  sf::Sound& sound;

  double total;
  sf::IntRect original;

  bool doOnce;
public:
  FaintAction(sf::Sprite& ref, sf::SoundBuffer& buffer, sf::Sound& sound);
  ~FaintAction();
  virtual void update(double elapsed);
  virtual void draw(sf::RenderTexture& surface);
};
