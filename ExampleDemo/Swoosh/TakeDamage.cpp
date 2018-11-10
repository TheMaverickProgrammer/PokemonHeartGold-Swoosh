#include "pch.h"
#include "TakeDamage.h"

TakeDamage::TakeDamage(pokemon::monster& ref, int damage, sf::SoundBuffer& buffer, sf::Sound& sound) : buffer(buffer), sound(sound), monster(ref), BlockingActionItem() {
  this->damage = damage;
  this->health = ref.hp;
  playOnce = false;
}

TakeDamage::~TakeDamage() {
  monster.hp = health - damage;

}

void TakeDamage::update(double elapsed) {
  if (!playOnce) {
    playOnce = true;
    sound.setBuffer(buffer);
    sound.play();
  }

  if (monster.hp > health - damage) {
    monster.hp -= 1;
  }
  else {
    markDone();
  }
}

void TakeDamage::draw(sf::RenderTexture& surface) {
}