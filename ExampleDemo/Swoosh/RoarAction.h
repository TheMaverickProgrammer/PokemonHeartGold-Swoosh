#pragma once
#include <Swoosh\Ease.h>
#include "ActionList.h"
#include <SFML\Audio.hpp>

class RoarAction : public BlockingActionItem {
private:
  sf::Sprite& ref;
  double total;
  bool swell;
  bool doOnce;
  sf::SoundBuffer** buffer;
  sf::Sound& sound;
public:
  RoarAction(sf::Sprite& ref, sf::SoundBuffer** buffer, sf::Sound& roar, bool swell = true);

  virtual void update(double elapsed);

  virtual void draw(sf::RenderTexture& surface);
};


