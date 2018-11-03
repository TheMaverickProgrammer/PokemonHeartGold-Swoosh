#pragma once
#include <Swoosh\ActivityController.h>
#include <Swoosh\Game.h>
#include <SFML\Graphics.hpp>
#include <SFML\Audio.hpp>
#include "TextureLoader.h"
#include "Particle.h"
#include "ResourcePaths.h"

#include <Segues\SlideIn.h>
#include <Segues\BlendFadeIn.h>

#include <iostream>

#define TEXT_BLOCK_INFO "Swoosh is an Activity and Segue mini library\n" \
                        "designed to make complex screen transitions\n" \
                        "a thing of the past.\n" \
                        "This is a proof-of-concept demo showcasing\n" \
                        "its features and includes helpful utilities\n" \
                        "for your SFML apps or games.\n\n" \
                        "Fork at\ngithub.com/TheMaverickProgrammer/Swoosh"

#define CONTROLS_INFO ">> Left click to shoot\n\n"\
                      ">> Right click to boost and dodge\n\n" \
                      ">> Collect stars for extra life\n\n"

using namespace swoosh;
using namespace swoosh::intent;

class BattleScene : public Activity {
private:
 
public:
  BattleScene(ActivityController& controller) : Activity(controller) {
    
  }

  virtual void onStart() {
  }

  virtual void onUpdate(double elapsed) {

  }

  virtual void onLeave() {
  }

  virtual void onExit() {
  }

  virtual void onEnter() {
    
  }

  virtual void onResume() {
  }

  virtual void onDraw(sf::RenderTexture& surface) {
  }

  virtual void onEnd() {
  }

  virtual ~BattleScene() { ; }
};