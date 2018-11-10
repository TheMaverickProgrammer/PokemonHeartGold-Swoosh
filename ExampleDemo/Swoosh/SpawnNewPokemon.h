#pragma once
#include "DemoActivities\BattleScene.h"
#include "ActionList.h"

class SpawnNewPokemon : public BlockingActionItem {
private:
  BattleScene* ref;
  bool isLoaded;
  double scale;
  double total;
  sf::Shader shader;
public:
  SpawnNewPokemon(BattleScene* ref);

  virtual void update(double elapsed);
  virtual void draw(sf::RenderTexture& surface);
};