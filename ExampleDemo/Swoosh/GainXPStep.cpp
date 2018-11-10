#include "pch.h"
#include "GainXPStep.h"
#include "ChangeText.h"
#include "WaitForButtonPressAction.h"

GainXPStep::GainXPStep(std::string& output, pokemon::monster& ref, pokemon::monster& defeated,
  sf::SoundBuffer& buttonBuffer, sf::SoundBuffer& buffer, sf::SoundBuffer& levelupBuffer, sf::Sound& sound, ActionList& actions)
  : output(output), buttonBuffer(buttonBuffer), buffer(buffer), levelupBuffer(levelupBuffer), sound(sound), monster(ref),
  defeated(defeated), actions(actions), BlockingActionItem() {
  // In a real game, increase xp by level and other factors
  // In ours, multiply the xp by the gap in levels and a deginerative value
  this->xp = this->gainedxp = std::ceil((defeated.xp*0.98*((double)defeated.level / (double)ref.level)));
  playOnce = false;
}

GainXPStep::~GainXPStep() {
  monster.xp = this->xp;
}

void GainXPStep::overwriteXP(int xp) { this->xp = xp; }

const int GainXPStep::getXP() const { return this->xp; }

void GainXPStep::update(double elapsed) {
  if (!playOnce) {
    playOnce = true;
    sound.setBuffer(buffer);
    sound.play();
  }

  if (this->xp > 0) {
    monster.xp += 1;
    this->xp--;

    if (monster.xp == 100) {
      monster.xp = 0;
      monster.level++;
      monster.maxhp += (int)(monster.maxhp * 0.10);
      monster.hp = monster.maxhp;
      sound.stop(); // interupt
      sound.setBuffer(levelupBuffer);
      sound.play();
      markDone();

      actions.insert(getIndex() + 1, new ChangeText(output, std::string() + monster.name + " leveled up!"));
      actions.insert(getIndex() + 2, new WaitForButtonPressAction(sf::Keyboard::Key::Enter, buttonBuffer, sound));
      GainXPStep* xpAction = new GainXPStep(output, monster, defeated, buttonBuffer, buffer, levelupBuffer, sound, actions);
      xpAction->overwriteXP(this->xp);
      actions.insert(getIndex() + 3, xpAction);
    }
  }
  else {
    sound.stop(); // interupt
    markDone();
  }
}

void GainXPStep::draw(sf::RenderTexture& surface) {
}