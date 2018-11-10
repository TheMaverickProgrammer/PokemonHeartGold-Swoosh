#pragma once
#include "ActionList.h"

class BattleScene;

class SignalRoundOver : public ActionItem {
private:
  BattleScene* ref;
public:
  SignalRoundOver(BattleScene* ref);

  virtual void update(double elapsed);

  virtual void draw(sf::RenderTexture& surface);
};