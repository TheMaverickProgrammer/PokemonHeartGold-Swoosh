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
  MapLayer* layerTwo;   // collision
  MapLayer* layerThree; // spawn areas
  sf::View view;

  sf::Texture* playerTexture;
  sf::Texture overworldSnapshot;
  sf::Sprite player;

  sf::Music townMusic;

  enum Direction : int {
    DOWN,
    LEFT,
    UP,
    RIGHT
  };

  Timer walkAnim;
  Direction playerDirection;
  bool inFocus;

  std::vector<pokemon::monster> playerMonsters;
public:
  DemoScene(ActivityController& controller) : Activity(controller) { 
    inFocus = false;
    townMusic.openFromFile(TOWN_MUSIC_PATH);

    map.load(OVERWORLD_TMX_PATH);

    layerZero  = new MapLayer(map, 0);
    layerOne   = new MapLayer(map, 1);
    layerTwo   = new MapLayer(map, 2);
    layerThree = new MapLayer(map, 3);

    playerTexture = loadTexture(PLAYER_OW_PATH);
    player = sf::Sprite(*playerTexture);

    auto windowSize = getController().getInitialWindowSize();

    player.setPosition(windowSize.x / 2, windowSize.y / 2);

    restart();
  }

  void restart() {
    inFocus = false;
    view = sf::View(sf::FloatRect(20.0f, 20.0f, 400.0f, 300.0f));
    view.setCenter(37 * 16, 45 * 16);
    playerDirection = Direction::DOWN;

    playerMonsters.clear();
    playerMonsters.push_back(pokemon::monster(pokemon::pikachu));
    playerMonsters.push_back(pokemon::monster(pokemon::charizard));
    playerMonsters.push_back(pokemon::monster(pokemon::piplup));
    playerMonsters.push_back(pokemon::monster(pokemon::roserade));
    playerMonsters.push_back(pokemon::monster(pokemon::onyx));
  }

  virtual void onStart() {
    std::cout << "DemoScene onStart called" << std::endl;
    townMusic.play();
  }

  virtual void onUpdate(double elapsed) {
    sf::RenderWindow& window = getController().getWindow();

    if (inFocus) {
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
        getController().push<segue<WhiteWashFade>::to<MainMenuScene>>(overworldSnapshot, false); // Pass into next scene
      }

      bool isWalking = false;

      walkAnim.start();
      sf::View before = view;

      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        playerDirection = Direction::UP;
        view.setCenter(view.getCenter() - sf::Vector2f(0.0f, 2.0f));
        isWalking = true;
      } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        playerDirection = Direction::DOWN;
        view.setCenter(view.getCenter() + sf::Vector2f(0.0f, 2.0f));
        isWalking = true;
      } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        playerDirection = Direction::LEFT;
        view.setCenter(view.getCenter() - sf::Vector2f(2.0f, 0.0f));
        isWalking = true;
      } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        playerDirection = Direction::RIGHT;
        view.setCenter(view.getCenter() + sf::Vector2f(2.0f, 0.0f));
        isWalking = true;
      }
      else {
        // Do not update animation
        walkAnim.pause();
      }
      
      // Check the collision layer....
      // Down, left, up, right
      std::uint32_t tile[] = {
                      layerTwo->tileIDAtCoord(view.getCenter().x, view.getCenter().y+7),
                      layerTwo->tileIDAtCoord(view.getCenter().x-7, view.getCenter().y),
                      layerTwo->tileIDAtCoord(view.getCenter().x, view.getCenter().y-7),
                      layerTwo->tileIDAtCoord(view.getCenter().x+7, view.getCenter().y)
      };

      if (tile[0] == 1132 || tile[1] == 1132 || tile[2] == 1132 || tile[3] == 1132) {
        view = before;
        isWalking = false; // Do not flag for battles if we haven't moved
      }

      // Check the battle spawn layer....
      // Down, left, up, right
      std::uint32_t spawn_tile = layerThree->tileIDAtCoord(view.getCenter().x, view.getCenter().y);

      if (spawn_tile == 1127 && isWalking) {
        int random_battle = rand() % 100;

        if (random_battle == 99) {
          getController().push<segue<Checkerboard>::to<BattleScene>>(playerMonsters);
        }
      }
    }
    else {
      townMusic.setVolume(townMusic.getVolume() * 0.8);
    }
  }

  virtual void onLeave() {
    inFocus = false;
    std::cout << "DemoScene onLeave called" << std::endl;

  }

  virtual void onExit() {
    std::cout << "DemoScene onExit called" << std::endl;
    townMusic.stop();
  }

  virtual void onEnter() {
    std::cout << "DemoScene onEnter called" << std::endl;

  }

  virtual void onResume() {
    inFocus = true;
    std::cout << "DemoScene onResume called" << std::endl;
    townMusic.play();
    townMusic.setVolume(100);
  }

  virtual void onDraw(sf::RenderTexture& surface) {
    sf::RenderWindow& window = getController().getWindow();
    //setView(view);

    int frame = ((int)walkAnim.getElapsed().asMilliseconds()/200) % 4;

    if (frame == 3) frame = 1; 

    player.setTextureRect(sf::IntRect(21 * frame, (int)playerDirection * 28, 21, 28));
    setOrigin(player, 0.5, 0.5);

    surface.setView(view);
    surface.draw(*layerZero);
    surface.draw(*layerOne);
    // surface.draw(*layerTwo); // collision layer

    player.setPosition(surface.getView().getCenter());
    surface.draw(player);

    overworldSnapshot = sf::Texture(surface.getTexture()); // copy
  }

  virtual void onEnd() {
    std::cout << "DemoScene onEnd called" << std::endl;
  }

  virtual ~DemoScene() { delete layerOne; delete layerZero; }
};