#pragma once
#include <SFML\Audio.hpp>
#include "ActionList.h"
#include "Pokemon.h"

class TakeDamage : public BlockingActionItem {
private:
  pokemon::monster& monster;
  int damage;
  int health;
  sf::SoundBuffer& buffer;
  sf::Sound& sound;
  bool playOnce;
public:

  TakeDamage(pokemon::monster& ref, int damage, sf::SoundBuffer& buffer, sf::Sound& sound);
  ~TakeDamage();
  virtual void update(double elapsed);
  virtual void draw(sf::RenderTexture& surface);
};