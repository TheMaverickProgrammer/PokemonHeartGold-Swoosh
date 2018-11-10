#pragma once
#include "ActionList.h"

class BattleScene;

// This is an interrupting action that clears the list, making room to add a new set of action items
// And prevents us from returning to the main battle loop
class SignalCheckHP : public ClearAllActions {
private:
  BattleScene* ref;
public:
  SignalCheckHP(BattleScene* ref);
  virtual ~SignalCheckHP(); 
  virtual void update(double elapsed);
  virtual void draw(sf::RenderTexture& surface);
};