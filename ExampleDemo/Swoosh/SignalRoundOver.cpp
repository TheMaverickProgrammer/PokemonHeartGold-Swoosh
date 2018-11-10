#include "pch.h"
#include "SignalRoundOver.h"
#include "DemoActivities\BattleScene.h"

SignalRoundOver::SignalRoundOver(BattleScene* ref) : ActionItem() {
  this->ref = ref;
}

void SignalRoundOver::update(double elapsed) {
  this->ref->canInteract = true;
  this->ref->preBattleSetup = false;
  markDone();
}

void SignalRoundOver::draw(sf::RenderTexture& surface) { ; }