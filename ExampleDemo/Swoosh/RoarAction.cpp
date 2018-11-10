#include "pch.h"
#include "RoarAction.h"

RoarAction::RoarAction(sf::Sprite& ref, sf::SoundBuffer** buffer, sf::Sound& roar, bool swell) : buffer(buffer), sound(roar), swell(swell), ref(ref) {
  total = 0;
  doOnce = false;
}

void RoarAction::update(double elapsed) {
  if (!doOnce) {
    sound.setBuffer(**buffer);
    sound.play();
    doOnce = true;
  }

  total += elapsed;

  if (swell) {
    double alpha = swoosh::ease::wideParabola(total, 1.0, 1.0);
    double scale = 1.0 + (alpha * 0.25);
    ref.setScale(scale, scale);
  }

  if (swell) { // intro is longer
    if (total >= 2.0)
      markDone();
  }
  else { // fainting is quicker
    if (total >= 0.75)
      markDone();
  }
}

void RoarAction::draw(sf::RenderTexture& surface) {
  surface.draw(ref);
}