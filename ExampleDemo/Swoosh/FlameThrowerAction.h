#pragma once
#include "ActionList.h"
#include <SFML\Audio.hpp>
#include <SFML\Graphics.hpp>

class BattleScene;

class FlamethrowerAction : public BlockingActionItem {
private:
  sf::Sprite& ref;
  sf::Sprite& target;
  sf::SoundBuffer &buffer;
  sf::Sound& sound;
  sf::Texture& flame;
  int facing;

  sf::Vector2f original;
  BattleScene& scene;

  double total;
  bool doOnce;
public:
  FlamethrowerAction(BattleScene& scene, sf::Sprite& ref, int facing, sf::SoundBuffer& buffer, sf::Sound& sound, sf::Sprite& target, sf::Texture& flame);
  ~FlamethrowerAction();

  virtual void update(double elapsed);
  virtual void draw(sf::RenderTexture& surface);
};
