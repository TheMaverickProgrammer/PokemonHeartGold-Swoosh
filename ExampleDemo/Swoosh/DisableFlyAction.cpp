#include "pch.h"
#include "DisableFlyAction.h"

DisableFlyAction::DisableFlyAction(sf::Sprite& ref, pokemon::monster& monster) : monster(monster),ref(ref) {

}

DisableFlyAction::~DisableFlyAction() { ; }

void DisableFlyAction::update(double elapsed) {
  monster.isFlying = false;
  ref.setScale(1.0, 1.0); // make us visible again
  markDone();
}

void DisableFlyAction::draw(sf::RenderTexture& surface) {
}