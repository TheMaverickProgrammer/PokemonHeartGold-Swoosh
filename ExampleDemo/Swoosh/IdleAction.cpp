#include "pch.h"
#include "IdleAction.h"

IdleAction::IdleAction(sf::Sprite& ref) : ref(ref) {

}

void IdleAction::update(double elapsed) {
}

void IdleAction::draw(sf::RenderTexture& surface) {
  surface.draw(ref);
}