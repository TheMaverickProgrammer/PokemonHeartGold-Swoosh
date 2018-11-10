#pragma once
#include <Swoosh\Ease.h>
#include "ResourceManager.h"
#include "Pokemon.h"
#include "ActionList.h"

class GainXPStep : public BlockingActionItem {
private:
  pokemon::monster& monster;
  pokemon::monster& defeated;
  sf::SoundBuffer& buttonBuffer;
  sf::SoundBuffer& buffer;
  sf::SoundBuffer& levelupBuffer;
  ActionList& actions;
  sf::Sound& sound;
  std::string& output;
  int xp;
  int gainedxp;
  bool playOnce;
public:

  GainXPStep(std::string& output, pokemon::monster& ref, pokemon::monster& defeated,
    sf::SoundBuffer& buttonBuffer, sf::SoundBuffer& buffer, sf::SoundBuffer& levelupBuffer, sf::Sound& sound, ActionList& actions);

  ~GainXPStep();

  void overwriteXP(int xp);

  const int getXP() const;

  virtual void update(double elapsed);

  virtual void draw(sf::RenderTexture& surface);
};

