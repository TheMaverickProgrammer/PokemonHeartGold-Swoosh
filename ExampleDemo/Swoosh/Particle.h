#pragma once
#include <SFML/Graphics.hpp>

struct particle {
  sf::Sprite sprite;
  sf::Vector2f pos;
  sf::Vector2f speed;
  sf::Vector2f friction;
  sf::Vector2f acceleration;
  double life;
  double lifetime;

  particle() {
    pos = speed = acceleration = sf::Vector2f(0.0f, 0.0f);
    life = lifetime = 1.0;
    friction = sf::Vector2f(1.0f, 1.0f);
  }

  particle(const particle& rhs) {
    sprite = rhs.sprite;
    pos = rhs.pos;
    acceleration = rhs.acceleration;
    speed = rhs.speed;
    friction = rhs.friction;
    life = rhs.life;
    lifetime = rhs.lifetime;
  }
};