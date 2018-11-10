#include "pch.h"
#include "WaitForButtonPressAction.h"

WaitForButtonPressAction::WaitForButtonPressAction(sf::Keyboard::Key button, sf::SoundBuffer& buffer, sf::Sound& sound)
  : buffer(buffer), sound(sound), button(button), BlockingActionItem() {
  isPressed = false;
  total = 0;
  wasHeldBefore = false;
}

void WaitForButtonPressAction::update(double elapsed) {
  if (total == 0) {
    wasHeldBefore = sf::Keyboard::isKeyPressed(button);
  }

  total += elapsed;

  if (!sf::Keyboard::isKeyPressed(button)) {
    if (wasHeldBefore) {
      wasHeldBefore = false;
      isPressed = false;
    }
    else if (isPressed) {
      markDone();
    }
    else {
      isPressed = false;
    }
  }
  else if (sf::Keyboard::isKeyPressed(button) && !isPressed && !wasHeldBefore) {
    sound.setBuffer(buffer);
    sound.play();
    isPressed = true;
  }
}

void WaitForButtonPressAction::draw(sf::RenderTexture& surface) {
}