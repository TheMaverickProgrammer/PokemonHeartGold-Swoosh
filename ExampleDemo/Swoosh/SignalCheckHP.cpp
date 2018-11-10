#include "pch.h"
#include "SignalCheckHP.h"
#include "DemoActivities\BattleScene.h"

#include "SignalRoundOver.h"
#include "SpawnNewPokemon.h"
#include "LeaveBattleScene.h"

SignalCheckHP::SignalCheckHP(BattleScene* ref) : ClearAllActions() { this->ref = ref; }
SignalCheckHP::~SignalCheckHP() { ; }

void SignalCheckHP::update(double elapsed) {
  bool leaveScene = false;
  bool clearedList = false;

  // Add new events
  if (ref->wild.hp <= 0) {
    ClearAllActions::update(elapsed);
    clearedList = true;

    this->ref->actions.add(new IdleAction(ref->playerSprite));
    this->ref->actions.add(new RoarAction(ref->wildSprite, &ref->resources.wildRoarBuffer, ref->sound, false));
    this->ref->actions.add(new FaintAction(ref->wildSprite, *ref->resources.faintBuffer, ref->sound));
    this->ref->actions.add(new ChangeText(ref->output, std::string() + ref->wild.name + " fainted!"));
    this->ref->actions.add(new ChangeMusic(ref->battleMusic, VICTORY_MUSIC_PATH));
    this->ref->actions.add(new WaitForButtonPressAction(sf::Keyboard::Key::Enter, *ref->resources.selectBuffer, ref->sound));
    GainXPStep* xpAction = new GainXPStep(ref->output, ref->playerMonsters[0], ref->wild, *ref->resources.selectBuffer, *ref->resources.xpBuffer, *ref->resources.levelupBuffer, ref->sound, ref->actions);
    this->ref->actions.add(xpAction);
    this->ref->actions.add(new ChangeText(ref->output, std::string() + ref->playerMonsters[0].name + " gained " + std::to_string(xpAction->getXP()) + " XP!"));
    this->ref->actions.add(new WaitForButtonPressAction(sf::Keyboard::Key::Enter, *ref->resources.selectBuffer, ref->sound));

    leaveScene = true;
  }

  if (ref->playerMonsters[0].hp <= 0) {
    if (!clearedList) {
      // Prevent from clearing the recent additions...
      ClearAllActions::update(elapsed);
    }

    this->ref->actions.add(new IdleAction(ref->wildSprite));
    this->ref->actions.add(new RoarAction(ref->playerSprite, &ref->resources.playerRoarBuffer, ref->sound, false));
    this->ref->actions.add(new FaintAction(ref->playerSprite, *ref->resources.faintBuffer, ref->sound));
    this->ref->actions.add(new ChangeText(ref->output, std::string() + ref->playerMonsters[0].name + " fainted!"));
    this->ref->actions.add(new WaitForButtonPressAction(sf::Keyboard::Key::Enter, *ref->resources.selectBuffer, ref->sound));


    if (ref->playerMonsters.size() - 1 == 0) {
      // we're out of pokemon
      this->ref->actions.add(new ChangeText(ref->output, std::string() + "Trainer whited out!"));
      this->ref->actions.add(new WaitForButtonPressAction(sf::Keyboard::Key::Enter, *ref->resources.selectBuffer, ref->sound));

      leaveScene = true;
    }
    else {
      this->ref->actions.add(new ChangeText(ref->output, std::string() + "Go " + ref->playerMonsters[1].name + "!"));
      this->ref->actions.add(new SpawnNewPokemon(this->ref));
      this->ref->actions.add(new RoarAction(ref->playerSprite, &ref->resources.playerRoarBuffer, ref->sound));
      this->ref->actions.add(new IdleAction(ref->playerSprite));
      this->ref->actions.add(new WaitForButtonPressAction(sf::Keyboard::Key::Enter, *ref->resources.selectBuffer, ref->sound));
      this->ref->actions.add(new SignalRoundOver(this->ref));

    }

  }

  if (leaveScene) {
    this->ref->actions.add(new LeaveBattleScene(ref));
  }

  markDone();
}

void SignalCheckHP::draw(sf::RenderTexture& surface) { ; }