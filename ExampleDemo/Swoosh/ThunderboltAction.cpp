#include "pch.h"
#include "ThunderboltAction.h"

ThunderboltAction::ThunderboltAction(sf::Sprite& target, sf::SoundBuffer& buffer, sf::Sound& sound, sf::Texture& bolt) :
  target(target), buffer(buffer), sound(sound), bolt(bolt)
{
  total = 0;
  doOnce = false;
}

void ThunderboltAction::update(double elapsed) {
  if (!doOnce) {
    doOnce = true;
    sound.setBuffer(buffer);
    sound.play();
    bolt.setRepeated(true);
    thunderbolt.setTexture(bolt);
  }

  total += elapsed;

  double alpha = swoosh::ease::inOut(total, 1.0);
  target.setColor(sf::Color(255 * (1.0 - alpha), 255 * (1.0 - alpha), 255 * (1.0 - alpha), 255)); // turn them black

  auto rect = thunderbolt.getTextureRect();

  if(total < 0.3)
    thunderbolt.setTextureRect(sf::IntRect(rect.left, rect.top, rect.width, rect.height+10));
  else if(total < 0.6)
    thunderbolt.setTextureRect(sf::IntRect(rect.left, rect.top + 10, rect.width, rect.height + 10));
  else 
    thunderbolt.setTextureRect(sf::IntRect(rect.left, rect.top + 10, rect.width, rect.height));

  thunderbolt.setColor(sf::Color(255, 255, 255, alpha * 255));

  if (total >= 1)
    markDone();
}

void ThunderboltAction::draw(sf::RenderTexture& surface) {
  surface.draw(thunderbolt);
}

ThunderboltAction::~ThunderboltAction()
{
}
