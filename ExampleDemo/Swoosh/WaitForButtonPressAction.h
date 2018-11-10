#pragma once
#include <SFML\Graphics.hpp>
#include "ResourceManager.h"
#include "ActionList.h"

class WaitForButtonPressAction : public BlockingActionItem {
private:
  sf::Keyboard::Key button;
  sf::SoundBuffer& buffer;
  sf::Sound& sound;
  bool isPressed;
  bool wasHeldBefore;
  double total;
public:
  WaitForButtonPressAction(sf::Keyboard::Key button, sf::SoundBuffer& buffer, sf::Sound& sound);

  virtual void update(double elapsed);

  virtual void draw(sf::RenderTexture& surface);
};
