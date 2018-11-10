#include "pch.h"
#include "FlyAction.h"

FlyAction::FlyAction(sf::Sprite& ref, pokemon::monster& monster, sf::SoundBuffer& buffer, sf::Sound& sound) : monster(monster), buffer(buffer), sound(sound), ref(ref) {
  total = 0;
  doOnce = false;
}

FlyAction::~FlyAction() { ; }

void FlyAction::update(double elapsed) {
  if (!doOnce) {
    sound.setBuffer(buffer);
    sound.play();
    doOnce = true;
  }

  if (!monster.isFlying) {
    total += elapsed;
    double alpha = swoosh::ease::linear(total, 0.25, 1.0);
    ref.setScale(1.0 - alpha, 1.0 + (alpha * 2));

    if (total >= 0.25) {
      markDone();
      ref.setScale(0, 0); // hide
      monster.isFlying = true;
    }
  }
  else {
    ref.setScale(1, 1);
    monster.isFlying = false;
    markDone();
  }
}

void FlyAction::draw(sf::RenderTexture& surface) {
  surface.draw(ref);
}