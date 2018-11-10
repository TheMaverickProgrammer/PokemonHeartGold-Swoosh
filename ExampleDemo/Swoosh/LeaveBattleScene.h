#pragma once
#include "ActionList.h"

class BattleScene;

class LeaveBattleScene : public BlockingActionItem {
private:
  BattleScene* ref;
public:
  LeaveBattleScene(BattleScene* ref);

  virtual void update(double elapsed);

  virtual void draw(sf::RenderTexture& surface);
};