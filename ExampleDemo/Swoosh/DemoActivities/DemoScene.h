#pragma once
#include <Swoosh\ActivityController.h>
#include <Swoosh\Game.h>

#include <SFML\Graphics.hpp>
#include "..\TextureLoader.h"
#include "..\Particle.h"
#include "..\ResourcePaths.h"

#include <Segues\Checkerboard.h>

#include <iostream>
#include <assert.h>

using namespace swoosh;
using namespace swoosh::game;
using namespace swoosh::intent;

class DemoScene : public Activity {
private:
 
public:
  DemoScene(ActivityController& controller) : Activity(controller) { 
   
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
    std::cout << "DemoScene OnEnd called" << std::endl;
  }

  virtual ~DemoScene() { ; }
};