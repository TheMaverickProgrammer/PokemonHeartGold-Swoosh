#include "pch.h"
#include "TackleAction.h"

TackleAction::TackleAction(sf::Sprite& ref, int facing) : ref(ref), facing(facing) {
  total = 0;
  original = ref.getPosition();
}

TackleAction::~TackleAction() { ref.setPosition(original); }

void TackleAction::update(double elapsed) {
  total += elapsed;

  double alpha = swoosh::ease::inOut(total, 0.50);
  int offsetx = 50.0 * alpha * (facing ? -1 : 1);
  ref.setPosition(original.x + offsetx, original.y);

  if (total > 0.50) {
    markDone();
  }
}

void TackleAction::draw(sf::RenderTexture& surface) {
  surface.draw(ref);
}