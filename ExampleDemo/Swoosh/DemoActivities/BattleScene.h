#pragma once
#include <Swoosh\ActivityController.h>
#include <Swoosh\Game.h>
#include <Swoosh\Ease.h>
#include <Swoosh\EmbedGLSL.h>
#include <SFML\Graphics.hpp>
#include <SFML\Audio.hpp>

#include <Segues\WhiteWashFade.h>
#include <Segues\BlackWashFade.h>

#include "TextureLoader.h"
#include "Particle.h"
#include "ResourcePaths.h"
#include "Pokemon.h"
#include "ActionList.h"
#include "BattleActions.h"

#include <iostream>

using namespace swoosh;
using namespace swoosh::intent;
using namespace swoosh::game;

auto MONOTONE_SHADER = GLSL
(
  110,
  uniform sampler2D texture;
  uniform float amount;

  void main()
  {
    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);
    float avg = (pixel.r + pixel.g + pixel.b) / 3.0;
    pixel = mix(pixel, vec4(avg, avg, avg, pixel.a), amount);
    gl_FragColor = gl_Color * pixel;
  }
);

class BattleScene : public Activity {
private:
  sf::Texture *playerTexture;
  sf::Texture *wildTexture;
  sf::Texture *battleAreaTexture;
  sf::Texture *battlePadFGTexture;
  sf::Texture *battlePadBGTexture;
  sf::Texture *textboxTexture;

  sf::Sprite wildSprite;
  sf::Sprite playerSprite;
  sf::Sprite battleAreaSprite;
  sf::Sprite battlePadFGSprite;
  sf::Sprite battlePadBGSprite;
  sf::Sprite textboxSprite;

  sf::Shader shader;

  sf::Font   menuFont;
  sf::Text   menuText;

  sf::Music battleMusic;

  sf::SoundBuffer wildRoarBuffer;
  sf::SoundBuffer playerRoarBuffer;
  sf::SoundBuffer statsFallBuffer;
  sf::SoundBuffer statsRiseBuffer;
  sf::Sound sound;
  
  sf::View view;

  bool fadeMusic;
  bool preBattleSetup;
  bool doIntro;
  bool whiteFlash;
  Timer waitTimer;
  
  bool canInteract;
  ActionList actions;
  std::string output;

  int rowSelect;
  int colSelect;

  enum FACING : int {
    BACK,
    FRONT
  };

  std::vector<pokemon::monster> playerMonsters;
  pokemon::monster wild;

  class SignalRoundOver : public ActionItem {
  private: 
    BattleScene* ref;
  public:
    SignalRoundOver(BattleScene* ref) : ActionItem() {
      this->ref = ref;
    }

    virtual void update(double elapsed) {
      this->ref->canInteract = true;
      this->ref->preBattleSetup = false;
      markDone();
    }

    virtual void draw(sf::RenderTexture& surface) { ; }
  };

public:

  BattleScene(ActivityController& controller) : Activity(controller) {
    fadeMusic = preBattleSetup = canInteract = doIntro = false;

    // Load sounds
    battleMusic.openFromFile(BATTLE_MUSIC_PATH);
    statsFallBuffer.loadFromFile(STATS_LOWER_SFX);
    statsRiseBuffer.loadFromFile(STATS_RISE_SFX);
    sound.setVolume(30);

    // Load shader
    shader.loadFromMemory(MONOTONE_SHADER, sf::Shader::Type::Fragment);
    shader.setUniform("texture", sf::Shader::CurrentTexture);

    wild = makeWildPokemon();
    //std::string name = std::string("Wild ") + std::string(wild.name);
    //wild.name = name.data();

    // Replace with reference from previous activity
    playerMonsters.push_back(pokemon::monster(pokemon::pikachu));
    playerRoarBuffer.loadFromFile(PIKACHU_PATH[2]);

    // Load graphics
    playerTexture = loadTexture(PIKACHU_PATH[FACING::BACK]);
    battleAreaTexture = loadTexture(GRASS_AREA);
    battlePadFGTexture = loadTexture(GRASS_PAD_FG);
    battlePadBGTexture = loadTexture(GRASS_PAD_BG);
    textboxTexture = loadTexture(TEXTBOX_PATH);

    playerSprite = sf::Sprite(*playerTexture);
    battleAreaSprite = sf::Sprite(*battleAreaTexture);
    battlePadBGSprite = sf::Sprite(*battlePadBGTexture);
    battlePadFGSprite = sf::Sprite(*battlePadFGTexture);
    textboxSprite = sf::Sprite(*textboxTexture);

    setOrigin(textboxSprite, 0.0, 1.0);
    textboxSprite.setScale(1.59f, 1.75f);

    // Load font
    menuFont.loadFromFile(GAME_FONT);
    menuText.setFont(menuFont);
    menuText.setScale(0.5, 0.5);

    colSelect = rowSelect = 0;
  }

  void generateBattleActions(const pokemon::moves& playerchoice) {
    const pokemon::moves* monsterchoice = nullptr;

    if (playerMonsters[0].move1 == nullptr && 
        playerMonsters[0].move2 == nullptr && 
        playerMonsters[0].move3 == nullptr && 
        playerMonsters[0].move4 == nullptr) {
      monsterchoice = &pokemon::nomove;
    }

    while (monsterchoice == nullptr) {
      int randmove = rand() % 4;

      switch (randmove) {
      case 0:
        monsterchoice = wild.move1;
        break;
      case 1:
        monsterchoice = wild.move2;
        break;
      case 2:
        monsterchoice = wild.move3;
        break;
      case 3:
        monsterchoice = wild.move4;
        break;
      }
    }

    actions.clear();

    // default behavior always plays (non-blocking)
    actions.add(new IdleAction(wildSprite));
    actions.add(new IdleAction(playerSprite));

    if (monsterchoice->speed > playerchoice.speed) {
      decideBattleOrder(&wild, monsterchoice, &wildSprite, &wildRoarBuffer, &playerMonsters[0], &playerchoice, &playerSprite, &playerRoarBuffer);
      decideBattleOrder(&playerMonsters[0], &playerchoice, &playerSprite, &playerRoarBuffer, &wild, monsterchoice, &wildSprite, &wildRoarBuffer);
    }
    else {
      decideBattleOrder(&playerMonsters[0], &playerchoice, &playerSprite, &playerRoarBuffer, &wild, monsterchoice, &wildSprite, &wildRoarBuffer);
      decideBattleOrder(&wild, monsterchoice, &wildSprite, &wildRoarBuffer, &playerMonsters[0], &playerchoice, &playerSprite, &playerRoarBuffer);
    }

    actions.add(new SignalRoundOver(this));
  }

  void decideBattleOrder(pokemon::monster* first, const pokemon::moves* firstchoice, sf::Sprite* firstSprite, sf::SoundBuffer* firstRoarBuffer, pokemon::monster* second, const pokemon::moves* secondchoice, sf::Sprite* secondSprite, sf::SoundBuffer* secondRoarBuffer) {
    FACING facing = FACING::BACK;

    if (first == &wild)
      facing = FACING::FRONT;
    
    if (firstchoice->name == "tackle") {
      actions.add(new WaitForButtonPressAction(output, std::string(first->name) + " used tackle!", sf::Keyboard::Enter));
      actions.add(new TackleAction(*firstSprite, facing));
      actions.add(new TakeDamage(*second, firstchoice->damage));
    }
    else if (firstchoice->name == "tail whip") {
      actions.add(new WaitForButtonPressAction(output, std::string(first->name) + " used tail whip!", sf::Keyboard::Enter));
      actions.add(new TailWhipAction(*firstSprite));
      actions.add(new DefenseDownAction(*secondSprite, statsFallBuffer, sound));
      actions.add(new WaitForButtonPressAction(output, std::string(second->name) + "'s defense\nfell sharply!", sf::Keyboard::Enter));

    }
    else if (firstchoice->name == "roar") {
      actions.add(new WaitForButtonPressAction(output, std::string(first->name) + " used roar!", sf::Keyboard::Enter));
      actions.add(new RoarAction(*firstSprite, *firstRoarBuffer, sound));
      actions.add(new AttackUpAction(*firstSprite, statsRiseBuffer, sound));
      actions.add(new WaitForButtonPressAction(output, std::string(second->name) + "'s attack\nrose!", sf::Keyboard::Enter));
    }
    else if (firstchoice->name == "thunder") {
      actions.add(new WaitForButtonPressAction(output, std::string(first->name) + " used thunder!", sf::Keyboard::Enter));
    }
    else if (firstchoice->name == "fly") {
      actions.add(new WaitForButtonPressAction(output, std::string(first->name) + " used fly!", sf::Keyboard::Enter));
      actions.add(new WaitForButtonPressAction(output, std::string(first->name) + " flew up into the sky!", sf::Keyboard::Enter));
    }
    else if (firstchoice->name == "nomove") {
      actions.add(new WaitForButtonPressAction(output, std::string(first->name) + " is struggling!", sf::Keyboard::Enter));
    }
  }

  pokemon::monster makeWildPokemon() {
    pokemon::monster wild;
    
    int select = rand() % 7;

    switch (select) {
    case 0:
      wild = pokemon::monster(pokemon::pidgey);
      wildTexture = loadTexture(PIDGEY_PATH[FACING::FRONT]);
      wildSprite = sf::Sprite(*wildTexture);
      wildRoarBuffer.loadFromFile(PIDGEY_PATH[2]);
      break;
    case 1:
      wild = pokemon::monster(pokemon::clefairy);
      wildTexture = loadTexture(CLEFAIRY_PATH[FACING::FRONT]);
      wildSprite = sf::Sprite(*wildTexture);
      wildRoarBuffer.loadFromFile(CLEFAIRY_PATH[2]);
      break;
    case 2:
      wild = pokemon::monster(pokemon::geodude);
      wildTexture = loadTexture(GEODUDE_PATH[FACING::FRONT]);
      wildSprite = sf::Sprite(*wildTexture);
      wildRoarBuffer.loadFromFile(GEODUDE_PATH[2]);
      break;
    case 3:
      wild = pokemon::monster(pokemon::ponyta);
      wildTexture = loadTexture(PONYTA_PATH[FACING::FRONT]);
      wildSprite = sf::Sprite(*wildTexture);
      wildRoarBuffer.loadFromFile(PONYTA_PATH[2]);
      break;
    case 4:
      wild = pokemon::monster(pokemon::cubone);
      wildTexture = loadTexture(CUBONE_PATH[FACING::FRONT]);
      wildSprite = sf::Sprite(*wildTexture);
      wildRoarBuffer.loadFromFile(CUBONE_PATH[2]);
      break;
    case 5:
      wild = pokemon::monster(pokemon::oddish);
      wildTexture = loadTexture(ODISH_PATH[FACING::FRONT]);
      wildSprite = sf::Sprite(*wildTexture);
      wildRoarBuffer.loadFromFile(ODISH_PATH[2]);
      break;
    case 6:
      wild = pokemon::monster(pokemon::pikachu);
      wildTexture = loadTexture(PIKACHU_PATH[FACING::FRONT]);
      wildSprite = sf::Sprite(*wildTexture);
      wildRoarBuffer.loadFromFile(PIKACHU_PATH[2]);
      break;
    }

    return wild;
  }

  virtual void onStart() {
    std::cout << "BattleScene OnStart called" << std::endl;

    // Start timer 
    waitTimer.start();
  }

  virtual void onUpdate(double elapsed) {
    if (fadeMusic) {
      battleMusic.setVolume(battleMusic.getVolume() * 0.90); // quieter
    }

    if (waitTimer.getElapsed().asSeconds() > 2) {
      if (!preBattleSetup) {
        actions.add(new IdleAction(wildSprite));
        actions.add(new BobAction(playerSprite));

        if (!doIntro) {
          actions.add(new RoarAction(wildSprite, wildRoarBuffer, sound));
          actions.add(new WaitForButtonPressAction(output, "A wild " + std::string(wild.name) + " appeard!", sf::Keyboard::Enter));
          actions.add(new SignalRoundOver(this)); // Prepare for keyboard interaction
          doIntro = true;
        }

        preBattleSetup = true;
      }
      actions.update(elapsed);
    }
    else {
      auto windowSize = getView().getSize();
      double alpha = ease::linear(waitTimer.getElapsed().asSeconds(), 2.0f, 1.0f);

      setOrigin(wildSprite, 0.5, 1.0);
      setOrigin(playerSprite, 0, 1.0);
      setOrigin(battlePadBGSprite, 0.5, 0.5);
      setOrigin(battlePadFGSprite, 0.35, 1.0);

      // All this does is divide the screen into 4 parts and places the pokemon into one of the corners.
      // I offset the position by accounting for the origin placement so that the pokemon appear offscreen
      // but when alpha reaches = 1, then they should be at those corners.
      // This could be reduced and look easier to follow but am lazy
      float backx = ((((float)windowSize.x / 4.0f)*3.0f + wildSprite.getTexture()->getSize().x*2.0f)*alpha) - (wildSprite.getTexture()->getSize().x*2.0f);
      float frontx = (float)windowSize.x*(1.0 - alpha) + ((float)windowSize.x / 4.0f);
      wildSprite.setPosition(backx, (float)windowSize.y / 2.0f);
      playerSprite.setPosition(frontx, ((float)windowSize.y / 4.0f) * 3.0f);

      battlePadBGSprite.setPosition(backx, (float)windowSize.y / 2.0f);
      battlePadFGSprite.setPosition(frontx, ((float)windowSize.y / 4.0f) * 3.0f);
    }

    if (preBattleSetup && sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace)) {
      using intent::segue;
      getController().queuePop<segue<BlackWashFade>>();
    }

    if (canInteract) {
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        rowSelect--;
      }
      else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        rowSelect++;
      }
      else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        colSelect--;
      }
      else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        colSelect++;
      }

      rowSelect = std::max(0, rowSelect);
      rowSelect = std::min(rowSelect, 1);
      colSelect = std::max(0, colSelect);
      colSelect = std::min(colSelect, 1);

      const pokemon::moves* choice = &pokemon::nomove;

      if (colSelect == 0 && rowSelect == 0) {
        choice = playerMonsters[0].move1;
      }

      if (colSelect == 1 && rowSelect == 0) {
        choice = playerMonsters[0].move2;
      }

      if (colSelect == 0 && rowSelect == 1) {
        choice = playerMonsters[0].move3;
      }

      if (colSelect == 1 && rowSelect == 1) {
        choice = playerMonsters[0].move4;
      }

      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
        canInteract = false;
        generateBattleActions(*choice);
      }
    }

    if (waitTimer.getElapsed().asSeconds() > 2) {
      shader.setUniform("amount", 0.0f);
    }
    else {
      shader.setUniform("amount", 1.0f);
    }

    textboxSprite.setPosition(0, getView().getSize().y);
  }

  virtual void onLeave() {
    std::cout << "BattleScene OnLeave called" << std::endl;
  }

  virtual void onExit() {
    std::cout << "BattleScene OnExit called" << std::endl;

    if (fadeMusic) {
      battleMusic.stop();
    }
  }

  virtual void onEnter() {
    std::cout << "BattleScene OnEnter called" << std::endl;
    battleMusic.play();
  }


  virtual void onResume() {
    std::cout << "BattleScene OnResume called" << std::endl;
  }

  virtual void onDraw(sf::RenderTexture& surface) {
    setView(sf::View(sf::FloatRect(0.0f, 0.0f, 400.0f, 300.0f)));

    sf::RenderStates states;
    states.shader = &shader;

    if (waitTimer.getElapsed().asSeconds() > 2) {
      surface.draw(battleAreaSprite);
      surface.draw(battlePadBGSprite);
      surface.draw(battlePadFGSprite);
      actions.draw(surface);
      surface.draw(textboxSprite);

      menuText.setFillColor(sf::Color::Black);

      if (canInteract) {
        menuText.setString("°");

        // Draw the karat
        if (colSelect == 0 && rowSelect == 0) {
          menuText.setPosition(12, 10 + getView().getSize().y / 4.0 * 3.0);
        }

        if (colSelect == 1 && rowSelect == 0) {
          menuText.setPosition(getView().getSize().x/2.0, 10 + getView().getSize().y / 4.0 * 3.0);
        }

        if (colSelect == 0 && rowSelect == 1) {
          menuText.setPosition(12, 10 + getView().getSize().y / 4.0 * 3.5);
        }

        if (colSelect == 1 && rowSelect == 1) {
          menuText.setPosition(getView().getSize().x / 2.0, 10 + getView().getSize().y / 4.0 * 3.5);
        }

        surface.draw(menuText);

        // Now draw the title of the moves
        menuText.setPosition(22, 10 + getView().getSize().y / 4.0 * 3.0);
        menuText.setString(playerMonsters[0].move1->name);
        surface.draw(menuText);

        menuText.setPosition(10 + getView().getSize().x / 2.0, 10 + getView().getSize().y / 4.0 * 3.0);
        menuText.setString(playerMonsters[0].move2->name);
        surface.draw(menuText);

        menuText.setPosition(22, 10 + getView().getSize().y / 4.0 * 3.5);
        menuText.setString(playerMonsters[0].move3->name);
        surface.draw(menuText);

        menuText.setPosition(10 + getView().getSize().x / 2.0, 10 + getView().getSize().y / 4.0 * 3.5);
        menuText.setString(playerMonsters[0].move4->name);
        surface.draw(menuText);

      }
      else {
        menuText.setString(output);
        menuText.setPosition(14, 10 + getView().getSize().y / 4.0 * 3.0);
        surface.draw(menuText);
      }
    }
    else {
      surface.draw(battlePadBGSprite, states);
      surface.draw(battlePadFGSprite, states);
      surface.draw(wildSprite, states);
      surface.draw(playerSprite, states);
    }
  }

  virtual void onEnd() {
    std::cout << "BattleScene OnEnd called" << std::endl;
  }

  virtual ~BattleScene() {
    delete wildTexture;
    delete playerTexture;
    delete battleAreaTexture;
    delete battlePadFGTexture;
    delete battlePadBGTexture;
    delete textboxTexture;
  }
};