#pragma once
#include <Swoosh\Ease.h>
#include "ActionList.h"
#include "ResourceManager.h"
#include "Shaders.h"

class DefenseDownAction : public BlockingActionItem {
private:
  sf::Sprite& ref;
  sf::Shader shader;
  sf::Texture* ddown;
  sf::SoundBuffer& buffer;
  sf::Sound& sound;
  double total;
  bool playedOnce;
public:
  DefenseDownAction(sf::Sprite& ref, sf::Texture& texture, sf::SoundBuffer& buffer, sf::Sound& sound);
  ~DefenseDownAction();
  virtual void update(double elapsed);
  virtual void draw(sf::RenderTexture& surface);
};
