#include "pch.h"
#include "DefenseDownAction.h"

DefenseDownAction::DefenseDownAction(sf::Sprite& ref, sf::Texture& texture, sf::SoundBuffer& buffer, sf::Sound& sound) : buffer(buffer), sound(sound), ref(ref) {
  shader.loadFromMemory(MOVING_TEXTURE_SHADER, sf::Shader::Type::Fragment);
  shader.setUniform("texture", sf::Shader::CurrentTexture);

  ddown = &texture;
  ddown->setRepeated(true);
  shader.setUniform("pattern", *ddown);

  total = 0;

  playedOnce = false;
}

DefenseDownAction::~DefenseDownAction() {
  delete ddown;
}

void DefenseDownAction::update(double elapsed) {
  if (!playedOnce) {
    sound.setBuffer(buffer);
    sound.play();
    playedOnce = true;
  }

  total += elapsed;
  double alpha = swoosh::ease::wideParabola(total, 2.0, 1.0);

  if (total >= 2.0)
    markDone();

  shader.setUniform("progress", (float)total*-2);
}

void DefenseDownAction::draw(sf::RenderTexture& surface) {
  sf::RenderStates states;
  states.shader = &shader;
  surface.draw(ref, states);
}