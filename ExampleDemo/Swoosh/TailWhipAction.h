#pragma once
#include <Swoosh\Ease.h>
#include "ActionList.h"
#include "ResourceManager.h"

class TailWhipAction : public BlockingActionItem {
private:
  sf::Sprite& ref;
  double total;
  sf::Vector2f original;
  sf::SoundBuffer& buffer;
  sf::Sound& sound;
  bool doOnce;
public:
  TailWhipAction(sf::Sprite& ref, sf::SoundBuffer& buffer, sf::Sound& sound);
  ~TailWhipAction();

  virtual void update(double elapsed);
  virtual void draw(sf::RenderTexture& surface);
};
