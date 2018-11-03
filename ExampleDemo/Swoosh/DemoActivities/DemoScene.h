#pragma once
#include <Swoosh\ActivityController.h>
#include <Swoosh\Game.h>

#include <SFML\Graphics.hpp>
#include <tmxlite\Map.hpp>

#include "MainMenuScene.h"
#include "..\TMXMapLayer.h"
#include "..\TextureLoader.h"
#include "..\Particle.h"
#include "..\ResourcePaths.h"

#include <Segues\Checkerboard.h>
#include <Segues\WhiteWashFade.h>

#include <iostream>
#include <assert.h>

using namespace swoosh;
using namespace swoosh::game;
using namespace swoosh::intent;

class DemoScene : public Activity {
private:
  tmx::Map map;
  MapLayer* layerZero;
  MapLayer* layerOne;
  sf::View view;

  sf::Texture* playerTexture;
  sf::Texture overworldSnapshot;
  sf::Sprite player;

  bool inFocus;
public:
  DemoScene(ActivityController& controller) : Activity(controller) { 
    inFocus = false;

    map.load(OVERWORLD_TMX_PATH);

    layerZero = new MapLayer(map, 0);
    layerOne = new MapLayer(map, 1);
    //MapLayer layerTwo(map, 2);

    playerTexture = loadTexture(PLAYER_OW_PATH);
    player = sf::Sprite(*playerTexture);

    auto windowSize = getController().getInitialWindowSize();

    setOrigin(player, 0.5, 0.5);
    player.setPosition(windowSize.x / 2, windowSize.y / 2);

    restart();
  }

  void restart() {
    inFocus = false;
    view = sf::View(sf::FloatRect(20.0f, 20.0f, 400.0f, 300.0f));
  }

  virtual void onStart() {
    std::cout << "DemoScene onStart called" << std::endl;
  }

  virtual void onUpdate(double elapsed) {
    sf::RenderWindow& window = getController().getWindow();

    if (inFocus && sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
      getController().push<segue<WhiteWashFade>::to<MainMenuScene>>(overworldSnapshot, false); // Pass into next scene
    }

    if (inFocus) {
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        view.setCenter(view.getCenter() - sf::Vector2f(0.0f, 2.0f));
      } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        view.setCenter(view.getCenter() + sf::Vector2f(0.0f, 2.0f));
      } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        view.setCenter(view.getCenter() - sf::Vector2f(2.0f, 0.0f));
      } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        view.setCenter(view.getCenter() + sf::Vector2f(2.0f, 0.0f));
      }
    }
  }

  virtual void onLeave() {
    inFocus = false;
    std::cout << "DemoScene onLeave called" << std::endl;

  }

  virtual void onExit() {
    std::cout << "DemoScene onExit called" << std::endl;
    restart();
  }

  virtual void onEnter() {
    std::cout << "DemoScene onEnter called" << std::endl;

  }

  virtual void onResume() {
    inFocus = true;
    std::cout << "DemoScene onResume called" << std::endl;
  }

  virtual void onDraw(sf::RenderTexture& surface) {
    sf::RenderWindow& window = getController().getWindow();
    setView(view);

    surface.draw(*layerZero);
    surface.draw(*layerOne);

    player.setPosition(getView().getCenter());
    surface.draw(player);

    overworldSnapshot = sf::Texture(surface.getTexture()); // copy
  }

  virtual void onEnd() {
    std::cout << "DemoScene onEnd called" << std::endl;
  }

  virtual ~DemoScene() { delete layerOne; delete layerZero; }
};