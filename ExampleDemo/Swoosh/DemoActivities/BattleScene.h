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

  sf::View view;

  bool fadeMusic;
  bool showInfo;
  bool whiteFlash;
  Timer waitTimer;
  
  bool canInteract;
  ActionList actions;

  enum FACING : int {
    BACK,
    FRONT
  };

  std::vector<pokemon::monster> playerMonsters;
  pokemon::monster wild;

public:
  BattleScene(ActivityController& controller) : Activity(controller) {
    fadeMusic = showInfo = canInteract = false;

    auto windowSize = getController().getInitialWindowSize();
    view = getController().getWindow().getDefaultView();

    // Load sounds
    battleMusic.openFromFile(BATTLE_MUSIC_PATH);

    // Load shader
    shader.loadFromMemory(MONOTONE_SHADER, sf::Shader::Type::Fragment);
    shader.setUniform("texture", sf::Shader::CurrentTexture);

    wild = makeWildPokemon();

    // Replace with reference from previous activity
    playerMonsters.push_back(pokemon::monster(pokemon::pikachu));

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
  }

  pokemon::monster makeWildPokemon() {
    pokemon::monster wild;
    
    int select = rand() % 7;

    switch (select) {
    case 0:
      wild = pokemon::monster(pokemon::pidgey);
      wildTexture = loadTexture(PIDGEY_PATH[FACING::FRONT]);
      wildSprite = sf::Sprite(*wildTexture);
      break;
    case 1:
      wild = pokemon::monster(pokemon::pidgey);
      wildTexture = loadTexture(CLEFAIRY_PATH[FACING::FRONT]);
      wildSprite = sf::Sprite(*wildTexture);
      break;
    case 2:
      wild = pokemon::monster(pokemon::pidgey);
      wildTexture = loadTexture(GEODUDE_PATH[FACING::FRONT]);
      wildSprite = sf::Sprite(*wildTexture);
      break;
    case 3:
      wild = pokemon::monster(pokemon::pidgey);
      wildTexture = loadTexture(PONYTA_PATH[FACING::FRONT]);
      wildSprite = sf::Sprite(*wildTexture);
      break;
    case 4:
      wild = pokemon::monster(pokemon::pidgey);
      wildTexture = loadTexture(CUBONE_PATH[FACING::FRONT]);
      wildSprite = sf::Sprite(*wildTexture);
      break;
    case 5:
      wild = pokemon::monster(pokemon::pidgey);
      wildTexture = loadTexture(ODISH_PATH[FACING::FRONT]);
      wildSprite = sf::Sprite(*wildTexture);
      break;
    case 6:
      wild = pokemon::monster(pokemon::pidgey);
      wildTexture = loadTexture(PIKACHU_PATH[FACING::FRONT]);
      wildSprite = sf::Sprite(*wildTexture);
      break;
    }

      return wild;
  }

  virtual void onStart() {
    std::cout << "BattleScene OnStart called" << std::endl;

    // Start timer 
    waitTimer.start();
    canInteract = true; // TODO: prompt battle menu
  }

  virtual void onUpdate(double elapsed) {
    if (fadeMusic) {
      battleMusic.setVolume(battleMusic.getVolume() * 0.90); // quieter
    }

    if (waitTimer.getElapsed().asSeconds() > 2) {
      if (!showInfo) {
        actions.add(new IdleAction(wildSprite));
        actions.add(new BobAction(playerSprite));
        showInfo = true;
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

    if (showInfo && sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace)) {
      using intent::segue;
      getController().queuePop<segue<BlackWashFade>>();
    }

    if (canInteract && sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
      canInteract = false;
      actions.clear();
      actions.add(new IdleAction(playerSprite));
      actions.add(new TackleAction(wildSprite, FACING::FRONT));
      actions.add(new ClearPreviousActions());
      actions.add(new IdleAction(wildSprite));
      actions.add(new TailWhipAction(playerSprite));
      actions.add(new ClearPreviousActions());
      actions.add(new DefenseDownAction(wildSprite));
      actions.add(new IdleAction(playerSprite));
      actions.add(new ClearPreviousActions());
      actions.add(new IdleAction(wildSprite));
      actions.add(new AttackUpAction(playerSprite));
      actions.add(new TackleAction(playerSprite, FACING::BACK));
      actions.add(new FaintAction(wildSprite));
    }
    else if (!canInteract && actions.isEmpty()) {
      
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
  }
};