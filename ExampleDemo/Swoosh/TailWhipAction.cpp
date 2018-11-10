#include "pch.h"
#include "TailWhipAction.h"

TailWhipAction::TailWhipAction(sf::Sprite& ref, sf::SoundBuffer& buffer, sf::Sound& sound) : buffer(buffer), sound(sound), ref(ref) {
  total = 0;
  original = ref.getPosition();
  doOnce = false;
}

TailWhipAction::~TailWhipAction() { ref.setPosition(original); }

void TailWhipAction::update(double elapsed) {
  if (!doOnce) {
    sound.setBuffer(buffer);
    sound.play();
    doOnce = true;
  }

  total += elapsed;
  double alpha = sin(total * 10) * 15;
  ref.setPosition(alpha + original.x, original.y);

  if (total >= 1.5) {
    markDone();
    ref.setPosition(original);
  }
}

void TailWhipAction::draw(sf::RenderTexture& surface) {
  surface.draw(ref);
}