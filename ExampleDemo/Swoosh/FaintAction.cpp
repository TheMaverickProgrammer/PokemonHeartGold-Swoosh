#include "pch.h"
#include "FaintAction.h"

FaintAction::FaintAction(sf::Sprite& ref, sf::SoundBuffer& buffer, sf::Sound& sound)
  : ref(ref), buffer(buffer), sound(sound), BlockingActionItem() {
  total = 0;
  original = ref.getTextureRect();
  doOnce = false;
}

FaintAction::~FaintAction() {
}

void FaintAction::update(double elapsed) {
  if (!doOnce) {
    sound.setBuffer(buffer);
    sound.play();
    doOnce = true;
  }

  total += elapsed;
  double alpha = 1.0 - swoosh::ease::linear(total, 0.25, 1.0);

  ref.setTextureRect(sf::IntRect(0, 0, original.width, original.height*alpha));
  swoosh::game::setOrigin(ref, ref.getOrigin().x / ref.getTexture()->getSize().x, 1.0f); // update origin pos

  if (total >= 0.25)
    markDone();
}

void FaintAction::draw(sf::RenderTexture& surface) {
  surface.draw(ref);
}