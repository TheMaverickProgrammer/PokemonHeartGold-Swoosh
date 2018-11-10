#pragma once
#include <Swoosh\Ease.h>

#include "ResourceManager.h"
#include "ActionList.h"
#include "Shaders.h"

class AttackUpAction : public BlockingActionItem {
private:
  sf::Sprite& ref;
  sf::Shader shader;
  sf::Texture* aup;
  sf::SoundBuffer& buffer;
  sf::Sound& sound;
  double total;
  bool playedOnce;
public:
  AttackUpAction(sf::Sprite& ref, sf::Texture& texture, sf::SoundBuffer& buffer, sf::Sound& sound);

  ~AttackUpAction();

  virtual void update(double elapsed);

  virtual void draw(sf::RenderTexture& surface);
};