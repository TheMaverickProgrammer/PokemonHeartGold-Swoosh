#pragma once
#include <Swoosh\ActivityController.h>
#include <Swoosh\Game.h>
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

using namespace swoosh::intent;

class MainMenuScene : public Activity {
private:
  sf::Texture* titleTexture;
  sf::Texture* overworldTexture;

  sf::Sprite title;

  sf::Font   menuFont;
  sf::Text   menuText;

  sf::Music themeMusic;

  bool fadeMusic;
public:
  MainMenuScene(ActivityController& controller) : Activity(controller) { 
    fadeMusic = false;

    auto windowSize = getController().getInitialWindowSize();

    titleTexture = loadTexture(TITLE_PATH);
    title = sf::Sprite(*titleTexture);
    setOrigin(title, 0.5, 0.5);
    title.setPosition(windowSize.x / 2, windowSize.y / 2);

    //menuFont.loadFromFile(GAME_FONT);
    //menuText.setFont(menuFont);

    //menuText.setFillColor(sf::Color::White); 

    // Load sounds
    themeMusic.openFromFile(THEME_MUSIC_PATH);
  }

  virtual void onStart() {
    std::cout << "MainMenuScene OnStart called" << std::endl;
    themeMusic.play();
  }

  virtual void onUpdate(double elapsed) {
    if (fadeMusic) {
      themeMusic.setVolume(themeMusic.getVolume() * 0.90); // quieter
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
    // We were coming from demo, the music changes
    if (fadeMusic) {
      themeMusic.play();
      themeMusic.setVolume(100);
    }

    fadeMusic = false;

    std::cout << "MainMenuScene OnResume called" << std::endl;
  }

  virtual void onDraw(sf::RenderTexture& surface) {
    sf::RenderWindow& window = getController().getWindow();

    surface.draw(title);
  }

  virtual void onEnd() {
    std::cout << "MainMenuScene OnEnd called" << std::endl;
  }

  virtual ~MainMenuScene() {
    delete titleTexture;
  }
};