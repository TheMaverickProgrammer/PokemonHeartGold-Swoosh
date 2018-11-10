#include "pch.h"
#include "BobAction.h"

BobAction::BobAction(sf::Sprite& ref) : ref(ref) {
  total = 0;
}

BobAction::~BobAction() {
}

void BobAction::update(double elapsed) {
  total += elapsed;

  bob.x = std::sin(total * 2) * 4;
  bob.y = std::abs(std::sin(total * 2)) * 5;
}

void BobAction::draw(sf::RenderTexture& surface) {
  auto original = ref.getPosition();
  ref.setPosition(original.x + bob.x, original.y + bob.y);
  surface.draw(ref);
  ref.setPosition(original);
}