#pragma once
#include "pch.h"
#include "BattleActions.h"
#include "DemoActivities\BattleScene.h"

FlamethrowerAction::FlamethrowerAction(BattleScene& scene, sf::Sprite& ref, int facing, sf::SoundBuffer& buffer, sf::Sound& sound, sf::Sprite& target, sf::Texture& flame)
    : scene(scene), facing(facing), ref(ref), buffer(buffer), sound(sound), target(target), flame(flame), BlockingActionItem() {
    total = 0;
    original = ref.getPosition();
    doOnce = false;
}

FlamethrowerAction::~FlamethrowerAction() {
  ref.setPosition(original);
}

void FlamethrowerAction::update(double elapsed) {
  if (!doOnce) {
    sound.setBuffer(buffer);
    sound.play();
    doOnce = true;
  }

  total += elapsed;

  if (total < 0.75) {
    double alpha = ease::linear(total, 0.25, 1.0);
    int offsetx = 50.0 * alpha * (facing ? -1 : 1);
    ref.setPosition(original.x + offsetx, original.y);

    if (total > 0.10) {
      // If facing back, raise the origin of flame
      sf::Vector2f position = ref.getPosition();
      if (facing == 0) {
        position = sf::Vector2f(ref.getPosition().x + 20, ref.getPosition().y - 50);
      }

      // the positions of the sprites do not align with the center due to orings. offset the target pos and add some jitter.
      sf::Vector2f direction = game::direction<float>(target.getPosition() - sf::Vector2f(rand()%50 * (facing? -1 : 1), facing? 70 : 90), position);
      sf::Vector2f speed = sf::Vector2f(direction.x * 600, direction.y * 200);
      sf::Vector2f friction = sf::Vector2f(0.80f, 1.0f); //  make the fire stop short
      sf::Vector2f accel = sf::Vector2f(0, -12.0f); // smoke travels up
      scene.spawnParticles(&flame, position, speed, friction, accel, 2);
    }
  }
  else {
    double alpha = 1.0 - ease::linear(total, 1.0, 1.0);
    int offsetx = 50.0 * alpha * (facing ? -1 : 1);
    ref.setPosition(original.x + offsetx, original.y);
  }

  double alpha = ease::inOut(total, 1.55);
  target.setColor(sf::Color(255, 255 * (1.0 - alpha), 255 * (1.0 - alpha), 255)); // turn them red


  if (total >= 2)
    markDone();
}

void FlamethrowerAction::draw(sf::RenderTexture& surface) {
  surface.draw(ref);
}
