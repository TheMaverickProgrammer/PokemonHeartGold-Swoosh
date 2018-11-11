#include "pch.h"
#include "SpawnNewPokemon.h"

SpawnNewPokemon::SpawnNewPokemon(BattleScene* ref) : ref(ref), BlockingActionItem() {
  isLoaded = false;
  scale = total = 0;
  shader.loadFromMemory(WHITE_SHADER, sf::Shader::Fragment);
  shader.setUniform("texture", sf::Shader::CurrentTexture);
}

void SpawnNewPokemon::update(double elapsed) {
  total += elapsed;

  if (!isLoaded) {
    this->ref->playerMonsters.erase(this->ref->playerMonsters.begin());
    this->ref->loadPlayerPokemonData();
    isLoaded = true;
    ref->spawnPokeballParticles(this->ref->resources.particleTexture, this->ref->playerSprite.getPosition());
  }

  scale = ease::linear(total, 0.5, 1.0);
  this->ref->playerSprite.setScale(scale, scale);

  // linger and fade in from white
  scale = ease::linear(total, 1.0, 1.0);
  shader.setUniform("opacity", 1.0f - (float)scale);

  if (total > 1.0)
    markDone();
}

void SpawnNewPokemon::draw(sf::RenderTexture& surface) {
  sf::RenderStates states;
  states.shader = &shader;

  surface.draw(this->ref->playerSprite, states);
}