#include "pch.h"
#include "LeaveBattleScene.h"
#include "DemoActivities\BattleScene.h"

LeaveBattleScene::LeaveBattleScene(BattleScene* ref) : BlockingActionItem() {
  this->ref = ref;
}

void LeaveBattleScene::update(double elapsed) {
  using intent::segue;

  // we whited out
  if (this->ref->playerMonsters[0].hp <= 0) {
    this->ref->getController().queuePop<segue<WhiteWashFade>>();
  }
  else {
    // we won or ran away
    this->ref->getController().queuePop<segue<BlackWashFade>>();
  }

  markDone();
}

void LeaveBattleScene::draw(sf::RenderTexture& surface) { ; }