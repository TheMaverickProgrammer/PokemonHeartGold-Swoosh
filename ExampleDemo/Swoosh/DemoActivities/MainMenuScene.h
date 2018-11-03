#pragma once
#include <Swoosh\ActivityController.h>
#include <Swoosh\Game.h>
#include <Swoosh\Ease.h>
#include <SFML\Graphics.hpp>
#include <SFML\Audio.hpp>

#include <Segues\WhiteWashFade.h>
#include <Segues\SlideIn.h>
#include <Segues\BlendFadeIn.h>
#include <Segues\PageTurn.h>
#include <Segues\ZoomOut.h>
#include <Segues\ZoomIn.h>
#include <Segues\HorizontalSlice.h>
#include <Segues\VerticalSlice.h>
#include <Segues\HorizontalOpen.h>
#include <Segues\VerticalOpen.h>

#include "DemoScene.h"
#include "BattleScene.h"
#include "TextureLoader.h"
#include "Particle.h"

#include <iostream>

using namespace swoosh;
using namespace swoosh::intent;
using namespace swoosh::game;

class MainMenuScene : public Activity {
private:
  sf::Texture* titleTexture;
  sf::Texture overworldTexture;

  sf::Sprite title;
  sf::Sprite overworldSnapshot;

  sf::Font   menuFont;
  sf::Text   menuText;

  sf::Music themeMusic;

  sf::View view;

  bool fadeMusic;
  bool showInfo;
  bool whiteFlash;
  Timer waitTimer;
public:
  MainMenuScene(ActivityController& controller, sf::Texture& overworldTexture, bool whiteFlash = true) : whiteFlash(whiteFlash), Activity(controller) { 
    fadeMusic = showInfo = false;

    this->overworldTexture = sf::Texture(overworldTexture);

    auto windowSize = getController().getInitialWindowSize();
    view = getController().getWindow().getDefaultView();

    titleTexture = loadTexture(TITLE_PATH);
    title = sf::Sprite(*titleTexture);
    setOrigin(title, 0.5, 0.5);
    title.setPosition(windowSize.x / 2, windowSize.y / 2);

    overworldSnapshot = sf::Sprite(overworldTexture);

    // Load sounds
    themeMusic.openFromFile(THEME_MUSIC_PATH);
  }

  virtual void onStart() {
    std::cout << "MainMenuScene OnStart called" << std::endl;
    themeMusic.play();

    // Start timer 
    waitTimer.start();
  }

  virtual void onUpdate(double elapsed) {
    if (fadeMusic) {
      themeMusic.setVolume(themeMusic.getVolume() * 0.90); // quieter
    }

    if (waitTimer.getElapsed().asSeconds() > 2) {
      showInfo = true;
    }

    if (showInfo && (sf::Keyboard::isKeyPressed(sf::Keyboard::Return) || sf::Keyboard::isKeyPressed(sf::Keyboard::Space))) {
      using intent::segue;
      getController().queuePop<segue<BlendFadeIn>>();
    }
  }

  virtual void onLeave() {
    std::cout << "MainMenuScene OnLeave called" << std::endl;
  }

  virtual void onExit() {
    std::cout << "MainMenuScene OnExit called" << std::endl;

    if (fadeMusic) {
      themeMusic.stop();
    }
  }

  virtual void onEnter() {
    std::cout << "MainMenuScene OnEnter called" << std::endl;

  }


  virtual void onResume() {
    std::cout << "MainMenuScene OnResume called" << std::endl;
  }

  virtual void onDraw(sf::RenderTexture& surface) {
    sf::RenderWindow& window = getController().getWindow();
    setView(view);

    surface.draw(overworldSnapshot);
    surface.draw(title);

    double alpha = 1.0 - ease::linear(waitTimer.getElapsed().asSeconds(), 1.0f, 1.0f);

    if (whiteFlash) {
      sf::RectangleShape whiteout;
      whiteout.setSize(sf::Vector2f(getController().getWindow().getSize().x, getController().getWindow().getSize().y));
      whiteout.setFillColor(sf::Color(255, 255, 255, alpha * 255));
      surface.draw(whiteout);
    }
  }

  virtual void onEnd() {
    std::cout << "MainMenuScene OnEnd called" << std::endl;
  }

  virtual ~MainMenuScene() {
    delete titleTexture;
  }
};