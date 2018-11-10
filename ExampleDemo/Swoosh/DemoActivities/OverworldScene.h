#pragma once
#include <Swoosh\ActivityController.h>
#include <Swoosh\Game.h>

#include <SFML\Graphics.hpp>
#include <tmxlite\Map.hpp>

#include "MainMenuScene.h"
#include "BattleScene.h"
#include "..\TMXMapLayer.h"
#include "..\Particle.h"
#include "..\ResourceManager.h"
#include "..\Pokemon.h"

#include <Segues\Checkerboard.h>
#include <Segues\WhiteWashFade.h>
#include "..\CustomPokemonSegues/GreyScalePatternPokemonSegue.h"
#include "..\CustomPokemonSegues/DisplacementPokemonSegue.h"

#include <iostream>
#include <assert.h>

using namespace swoosh;
using namespace swoosh::game;
using namespace swoosh::intent;

class OverworldScene : public Activity {
private:
  ResourceManager &resources;

  sf::View view;

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
  bool isWalking;
  int moveSpacesLeft;

  std::vector<pokemon::monster> playerMonsters;
public:
  OverworldScene(ActivityController& controller, ResourceManager &resources) 
    : resources(resources), Activity(controller) {
    inFocus = false;
    isWalking = false;
    moveSpacesLeft = 0;
    townMusic.openFromFile(TOWN_MUSIC_PATH);

    player = sf::Sprite(*resources.owPlayerTexture);

    auto windowSize = getController().getInitialWindowSize();

    player.setPosition(windowSize.x / 2, windowSize.y / 2);

    restart();
  }

  void restart() {
    inFocus = false;
    view = sf::View(sf::FloatRect(20.0f, 20.0f, 400.0f, 300.0f));
    view.setCenter(37 * 16 + 8, 45 * 16 + 8); // place on tile row 37 col 45. +8 to center sprite in middle of tile.
    playerDirection = Direction::DOWN;

    playerMonsters.clear();
    playerMonsters.push_back(pokemon::monster(pokemon::charizard));
    playerMonsters.push_back(pokemon::monster(pokemon::pikachu));
    playerMonsters.push_back(pokemon::monster(pokemon::onyx));
    playerMonsters.push_back(pokemon::monster(pokemon::piplup));
    playerMonsters.push_back(pokemon::monster(pokemon::roserade));
  }

  virtual void onStart() {
    std::cout << "OverworldScene onStart called" << std::endl;
    townMusic.play();
  }

  virtual void onUpdate(double elapsed) {
    sf::RenderWindow& window = getController().getWindow();

    if (inFocus) {
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
        getController().push<segue<WhiteWashFade>::to<MainMenuScene>>(resources, overworldSnapshot, false); // Pass into next scene
      }

      sf::View before = view;

      if (moveSpacesLeft == 0) {
        if (isWalking == true) {
          isWalking = false;

          // Do not update animation
          walkAnim.pause();

          // Check the battle spawn layer....
          // Down, left, up, right
          std::uint32_t spawn_tile = resources.layerThree->tileIDAtCoord(view.getCenter().x, view.getCenter().y);

          if (spawn_tile == 1127) {
            int random_battle = rand() % 100;

            if (random_battle >= 79) {
              int random_segue = 2; // rand() % 2;

              if (random_segue == 0) {
                getController().push<segue<PokeBallCircle, sec<3>>::to<BattleScene>>(resources, playerMonsters);
              }
              else if (random_segue == 1) {
                getController().push<segue<PokemonRetroBlit, sec<3>>::to<BattleScene>>(resources, playerMonsters);
              }
              else {
                getController().push<segue<DisplacementPokemonSegue, sec<3>>::to<BattleScene>>(resources, playerMonsters);
              }
            }
          }
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
          playerDirection = Direction::UP;

          // Check the collision layer....
          std::uint32_t tile = resources.layerTwo->tileIDAtCoord(view.getCenter().x, view.getCenter().y - 16);
          if (tile != 1132) {
            moveSpacesLeft = 8;
          }
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
          playerDirection = Direction::DOWN;

          // Check the collision layer....
          std::uint32_t tile = resources.layerTwo->tileIDAtCoord(view.getCenter().x, view.getCenter().y + 16);
          if (tile != 1132) {
            moveSpacesLeft = 8;
          }
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
          playerDirection = Direction::LEFT;

          // Check the collision layer....
          std::uint32_t tile = resources.layerTwo->tileIDAtCoord(view.getCenter().x - 16, view.getCenter().y);
          if (tile != 1132) {
            moveSpacesLeft = 8;
          }
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
          playerDirection = Direction::RIGHT;

          // Check the collision layer....
          std::uint32_t tile = resources.layerTwo->tileIDAtCoord(view.getCenter().x + 16, view.getCenter().y);
          if (tile != 1132) {
            moveSpacesLeft = 8;
          }
        }

        if (moveSpacesLeft > 0) {
          walkAnim.start();
        }
      }
      else {
        if (playerDirection == Direction::UP) {
          view.setCenter(view.getCenter() - sf::Vector2f(0.0f, 2.0f));
        }
        else if (playerDirection == Direction::DOWN) {
          view.setCenter(view.getCenter() + sf::Vector2f(0.0f, 2.0f));
        }
        else if (playerDirection == Direction::LEFT) {
          view.setCenter(view.getCenter() - sf::Vector2f(2.0f, 0.0f));
        }
        else if (playerDirection == Direction::RIGHT) {
          view.setCenter(view.getCenter() + sf::Vector2f(2.0f, 0.0f));
        }

        isWalking = true;
        moveSpacesLeft--;
      }
    }
    else {
      townMusic.setVolume(townMusic.getVolume() * 0.8);
    }
  }

  virtual void onLeave() {
    inFocus = false;
    std::cout << "OverworldScene onLeave called" << std::endl;

  }

  virtual void onExit() {
    std::cout << "OverworldScene onExit called" << std::endl;
    townMusic.stop();
  }

  virtual void onEnter() {
    std::cout << "OverworldScene onEnter called" << std::endl;
  }

  virtual void onResume() {
    inFocus = true;
    std::cout << "OverworldScene onResume called" << std::endl;
    townMusic.play();
    townMusic.setVolume(100);
  }

  virtual void onDraw(sf::RenderTexture& surface) {
    int frame = ((int)walkAnim.getElapsed().asMilliseconds() / 200) % 4;

    if (frame == 3) frame = 1;

    player.setTextureRect(sf::IntRect(21 * frame, (int)playerDirection * 28, 21, 28));
    setOrigin(player, 0.5, 0.5);

    surface.setView(view);
    surface.draw(*resources.layerZero);
    surface.draw(*resources.layerOne);
    // surface.draw(*layerTwo); // collision layer

    player.setPosition(surface.getView().getCenter());
    surface.draw(player);

    overworldSnapshot = sf::Texture(surface.getTexture()); // copy
  }

  virtual void onEnd() {
    std::cout << "OverworldScene onEnd called" << std::endl;
  }

  virtual ~OverworldScene() { ; }
};