#pragma once
#include "ActionList.h"
#include <SFML\Audio.hpp>
#include <SFML\Graphics.hpp>
#include <Swoosh\Ease.h>
#include <Swoosh\Game.h>

class ThunderboltAction : public BlockingActionItem
{
  sf::Sprite& target;
  sf::SoundBuffer &buffer;
  sf::Sound& sound;
  sf::Texture& bolt;
  sf::Sprite thunderbolt;

  double total;
  bool doOnce;
public:
  ThunderboltAction(sf::Sprite& target, sf::SoundBuffer& buffer, sf::Sound& sound, sf::Texture& bolt);
  ~ThunderboltAction();

  virtual void update(double elapsed);
  virtual void draw(sf::RenderTexture& surface);
};

