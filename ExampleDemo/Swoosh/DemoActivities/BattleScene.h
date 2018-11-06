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

auto STATUS_BAR_SHADER = GLSL
(
  110,
  uniform sampler2D texture;
  uniform float hp;
  uniform float xp;

  void main()
  {
    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);

    // hp is the red color coded pixels in the sample
    if (pixel.g == 0.0 && pixel.b == 0.0) {
      float normal = (hp*137.0) + (1.0 - hp)*255.0;
      normal = normal / 255.0;

      if (pixel.r > normal) {
        pixel = vec4(0.0, 1.0, 0.0, 1.0);
      }
      else {
        pixel = vec4(0.5, 0.5, 0.5, 1.0);
      }
    }  else if (pixel.r == 0.0 && pixel.g == 0.0) {
      // xp is the blue color coded pixels in the sample

      float normal = (xp*12.0) + (1.0 - xp)*255.0;
      normal = normal / 255.0;

      if (pixel.b > normal) {
        pixel = vec4(0.04, 1.0, 0.95, 1.0);
      }
      else {
        pixel = vec4(0.5, 0.5, 0.5, 1.0);
      }
    }

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
  sf::Texture *playerStatusTexture;
  sf::Texture *enemyStatusTexture;

  sf::Sprite wildSprite;
  sf::Sprite playerSprite;
  sf::Sprite battleAreaSprite;
  sf::Sprite battlePadFGSprite;
  sf::Sprite battlePadBGSprite;
  sf::Sprite textboxSprite;
  sf::Sprite playerStatusSprite;
  sf::Sprite enemyStatusSprite;

  sf::Shader shader;
  sf::Shader statusShader;

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
  bool isKeyDown;

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

  class LeaveBattleScene : public BlockingActionItem {
  private:
    BattleScene* ref;
  public:
    LeaveBattleScene(BattleScene* ref) : BlockingActionItem() {
      this->ref = ref;
    }

    virtual void update(double elapsed) {
      using intent::segue;

      // we whited out
      if (this->ref->playerMonsters[0].hp <= 0) {
        this->ref->getController().queuePop<segue<WhiteWashFade>>();
      }
      else {
        // we won or ran away
        this->ref->getController().queuePop<segue<BlackWashFade>>();
      }

      markDone();
    }

    virtual void draw(sf::RenderTexture& surface) { ; }
  };

  class SpawnNewPokemon : public BlockingActionItem {
  private:
    BattleScene* ref;
    bool isLoaded;
    double scale;
    double total;
  public:
    SpawnNewPokemon(BattleScene* ref) : ref(ref), BlockingActionItem() {
      isLoaded = false;
      scale = total = 0;
    }

    virtual void update(double elapsed) {
      total += elapsed;

      if (!isLoaded) {
        this->ref->playerMonsters.erase(this->ref->playerMonsters.begin());
        this->ref->loadPlayerPokemonData();
        isLoaded = true;
      }

      scale = ease::linear(total, 0.5, 1.0);
      this->ref->playerSprite.setScale(scale, scale);

      if (total > 0.5)
        markDone();
    }

    virtual void draw(sf::RenderTexture& surface) { ; }
  };

  // This is an interrupting action that clears the list, making room to add a new set of action items
  // And prevents us from returning to the main battle loop
  class SignalCheckHP : public ClearAllActions {
  private: 
    BattleScene* ref;
  public:
    SignalCheckHP(BattleScene* ref) : ClearAllActions() { this->ref = ref; }

    virtual void update(double elapsed) {
      bool leaveScene = false;
      bool clearedList = false;

      // Add new events
      if (ref->wild.hp <= 0) {
        ClearAllActions::update(elapsed);
        clearedList = true;

        this->ref->actions.add(new IdleAction(ref->playerSprite));
        this->ref->actions.add(new RoarAction(ref->wildSprite, ref->wildRoarBuffer, ref->sound, false));
        this->ref->actions.add(new FaintAction(ref->wildSprite));
        this->ref->actions.add(new GainXPAction(ref->playerMonsters[0], ref->wild));
        this->ref->actions.add(new WaitForButtonPressAction(this->ref->output, std::string() + ref->wild.name + " fainted!", sf::Keyboard::Key::Enter));
        leaveScene = true;
      }
      
      if (ref->playerMonsters[0].hp <= 0) {
        if (!clearedList) {
          // Prevent from clearing the recent additions...
          ClearAllActions::update(elapsed);
        }

        this->ref->actions.add(new IdleAction(ref->wildSprite));
        this->ref->actions.add(new RoarAction(ref->playerSprite, ref->playerRoarBuffer, ref->sound, false));
        this->ref->actions.add(new FaintAction(ref->playerSprite));
        this->ref->actions.add(new WaitForButtonPressAction(this->ref->output, std::string() + ref->playerMonsters[0].name + " fainted!", sf::Keyboard::Key::Enter));
        
        if (ref->playerMonsters.size() - 1 == 0) {
          // we're out of pokemon
          this->ref->actions.add(new WaitForButtonPressAction(this->ref->output, std::string() + "Trainer whited out!", sf::Keyboard::Key::Enter));
          leaveScene = true;
        }
        else {
          this->ref->actions.add(new WaitForButtonPressAction(this->ref->output, std::string() + "Go " + ref->playerMonsters[1].name + "!", sf::Keyboard::Key::Enter));
          this->ref->actions.add(new SpawnNewPokemon(this->ref));
          this->ref->actions.add(new RoarAction(ref->playerSprite, ref->playerRoarBuffer, ref->sound));
          this->ref->actions.add(new SignalRoundOver(this->ref));

        }

      }

      if (leaveScene) {
        this->ref->actions.add(new LeaveBattleScene(ref));
      }

      markDone();
    }

    virtual void draw(sf::RenderTexture& surface) { ; }
  };

public:

  BattleScene(ActivityController& controller) : Activity(controller) {
    fadeMusic = preBattleSetup = canInteract = isKeyDown = doIntro = false;

    // Load sounds
    battleMusic.openFromFile(BATTLE_MUSIC_PATH);
    statsFallBuffer.loadFromFile(STATS_LOWER_SFX);
    statsRiseBuffer.loadFromFile(STATS_RISE_SFX);
    sound.setVolume(30);

    // Load shader
    shader.loadFromMemory(MONOTONE_SHADER, sf::Shader::Type::Fragment);
    shader.setUniform("texture", sf::Shader::CurrentTexture);

    statusShader.loadFromMemory(STATUS_BAR_SHADER, sf::Shader::Type::Fragment);
    statusShader.setUniform("texture", sf::Shader::CurrentTexture);

    wild = makeWildPokemon();
    //std::string name = std::string("Wild ") + std::string(wild.name);
    //wild.name = name.data();

    // Replace with reference from previous activity
    playerMonsters.push_back(pokemon::monster(pokemon::pikachu));
    playerMonsters.push_back(pokemon::monster(pokemon::charizard));

    loadPlayerPokemonData();

    // Load graphics
    battleAreaTexture = loadTexture(GRASS_AREA);
    battlePadFGTexture = loadTexture(GRASS_PAD_FG);
    battlePadBGTexture = loadTexture(GRASS_PAD_BG);
    textboxTexture = loadTexture(TEXTBOX_PATH);
    playerStatusTexture = loadTexture(PLAYER_STATUS_PATH);
    enemyStatusTexture = loadTexture(ENEMY_STATUS_PATH);

    battleAreaSprite = sf::Sprite(*battleAreaTexture);
    battlePadBGSprite = sf::Sprite(*battlePadBGTexture);
    battlePadFGSprite = sf::Sprite(*battlePadFGTexture);
    textboxSprite = sf::Sprite(*textboxTexture);
    playerStatusSprite = sf::Sprite(*playerStatusTexture);
    enemyStatusSprite = sf::Sprite(*enemyStatusTexture);

    setOrigin(textboxSprite, 0.0, 1.0);
    textboxSprite.setScale(1.59f, 1.75f);

    // Load font
    menuFont.loadFromFile(GAME_FONT);
    menuText.setFont(menuFont);
    menuText.setScale(0.5, 0.5);

    colSelect = rowSelect = 0;
  }

  void loadPlayerPokemonData() {
    if (playerMonsters[0].name == "pikachu") {
      playerRoarBuffer.loadFromFile(PIKACHU_PATH[2]);
      if (playerTexture) delete playerTexture;
      playerTexture = loadTexture(PIKACHU_PATH[FACING::BACK]);
    } else if (playerMonsters[0].name == "charizard") {
      playerRoarBuffer.loadFromFile(CHARIZARD_PATH[2]);
      if (playerTexture) delete playerTexture;
      playerTexture = loadTexture(CHARIZARD_PATH[FACING::BACK]);
    }

    playerSprite.setTexture(*playerTexture, true);
    setOrigin(playerSprite, 0, 1.0);
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
      actions.add(new SignalCheckHP(this));
    }
    else if (firstchoice->name == "tail whip") {
      actions.add(new WaitForButtonPressAction(output, std::string(first->name) + " used tail whip!", sf::Keyboard::Enter));
      actions.add(new TailWhipAction(*firstSprite));
      actions.add(new DefenseDownAction(*secondSprite, statsFallBuffer, sound));
      actions.add(new WaitForButtonPressAction(output, std::string(second->name) + "'s defense\nfell sharply!", sf::Keyboard::Enter));
      actions.add(new SignalCheckHP(this));
    }
    else if (firstchoice->name == "roar") {
      actions.add(new WaitForButtonPressAction(output, std::string(first->name) + " used roar!", sf::Keyboard::Enter));
      actions.add(new RoarAction(*firstSprite, *firstRoarBuffer, sound));
      actions.add(new AttackUpAction(*firstSprite, statsRiseBuffer, sound));
      actions.add(new WaitForButtonPressAction(output, std::string(first->name) + "'s attack\nrose!", sf::Keyboard::Enter));
      actions.add(new SignalCheckHP(this));
    }
    else if (firstchoice->name == "thunder") {
      actions.add(new WaitForButtonPressAction(output, std::string(first->name) + " used thunder!", sf::Keyboard::Enter));
      actions.add(new TakeDamage(*second, firstchoice->damage));
      actions.add(new SignalCheckHP(this));
    }
    else if (firstchoice->name == "fly") {
      actions.add(new WaitForButtonPressAction(output, std::string(first->name) + " used fly!", sf::Keyboard::Enter));
      actions.add(new WaitForButtonPressAction(output, std::string(first->name) + " flew\nup into the sky!", sf::Keyboard::Enter));
      actions.add(new SignalCheckHP(this));
    }
    else if (firstchoice->name == "nomove") {
      actions.add(new WaitForButtonPressAction(output, std::string(first->name) + " is struggling!", sf::Keyboard::Enter));
      actions.add(new TakeDamage(*second, firstchoice->damage));
      actions.add(new TakeDamage(*first, firstchoice->damage));
      actions.add(new SignalCheckHP(this));
    }
  }

  pokemon::monster makeWildPokemon() {
    pokemon::monster wild;
    
    int select = rand() % 6;

    switch (select) {
    case 0:
      wild = pokemon::monster(pokemon::pidgey);
      wildTexture = loadTexture(PIDGEY_PATH[FACING::FRONT]);
      wildSprite = sf::Sprite(*wildTexture);
      wildRoarBuffer.loadFromFile(PIDGEY_PATH[2]);
      wild.xp = 5; // worth 5 xp
      break;
    case 1:
      wild = pokemon::monster(pokemon::clefairy);
      wildTexture = loadTexture(CLEFAIRY_PATH[FACING::FRONT]);
      wildSprite = sf::Sprite(*wildTexture);
      wildRoarBuffer.loadFromFile(CLEFAIRY_PATH[2]);
      wild.xp = 10; // worth 10 xp

      break;
    case 2:
      wild = pokemon::monster(pokemon::geodude);
      wildTexture = loadTexture(GEODUDE_PATH[FACING::FRONT]);
      wildSprite = sf::Sprite(*wildTexture);
      wildRoarBuffer.loadFromFile(GEODUDE_PATH[2]);
      wild.xp = 13; // worth 13 xp

      break;
    case 3:
      wild = pokemon::monster(pokemon::ponyta);
      wildTexture = loadTexture(PONYTA_PATH[FACING::FRONT]);
      wildSprite = sf::Sprite(*wildTexture);
      wildRoarBuffer.loadFromFile(PONYTA_PATH[2]);
      wild.xp = 11; // worth 11 xp

      break;
    case 4:
      wild = pokemon::monster(pokemon::cubone);
      wildTexture = loadTexture(CUBONE_PATH[FACING::FRONT]);
      wildSprite = sf::Sprite(*wildTexture);
      wildRoarBuffer.loadFromFile(CUBONE_PATH[2]);
      wild.xp = 20; // worth 20 xp

      break;
    case 5:
      wild = pokemon::monster(pokemon::oddish);
      wildTexture = loadTexture(ODISH_PATH[FACING::FRONT]);
      wildSprite = sf::Sprite(*wildTexture);
      wildRoarBuffer.loadFromFile(ODISH_PATH[2]);
      wild.xp = 5; // worth 5 xp

      break;
    case 6:
      wild = pokemon::monster(pokemon::pikachu);
      wildTexture = loadTexture(PIKACHU_PATH[FACING::FRONT]);
      wildSprite = sf::Sprite(*wildTexture);
      wildRoarBuffer.loadFromFile(PIKACHU_PATH[2]);
      wild.xp = 12; // worth 12 xp

      break;
    }
    
    setOrigin(wildSprite, 0.5, 1.0);
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
        actions.add(new ClearPreviousActions());
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

      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) && !isKeyDown) {
        if (choice != nullptr) {
          isKeyDown = true;
          canInteract = false;
          generateBattleActions(*choice);
        } // else ignore or play a buzzer sound
      }
      else if (canInteract && !sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) && isKeyDown) {
        isKeyDown = false; // force player to release key to press again
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

      // Position the ui
      setOrigin(playerStatusSprite, 1.0, 1.0); // bottom-right corner
      playerStatusSprite.setPosition(getView().getSize().x, textboxSprite.getPosition().y-textboxSprite.getGlobalBounds().height);
      setOrigin(enemyStatusSprite, 0.0, 0.0); // top-left corner
      enemyStatusSprite.setPosition(0, 0);

      // draw the ui
      states.shader = &statusShader;

      float hp = (float)playerMonsters[0].hp / (float)playerMonsters[0].maxhp;
      statusShader.setUniform("hp", hp);
      statusShader.setUniform("xp", (float)playerMonsters[0].xp / 100.0f);
      surface.draw(playerStatusSprite, states);

      statusShader.setUniform("hp", (float)wild.hp / (float)wild.maxhp);
      surface.draw(enemyStatusSprite, states);

      // Draw a menu of the current pokemon's abilities
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

        if(playerMonsters[0].move1 == nullptr) {
          menuText.setString("-");
        }
        else {
          menuText.setString(playerMonsters[0].move1->name);
        }

        surface.draw(menuText);

        menuText.setPosition(10 + getView().getSize().x / 2.0, 10 + getView().getSize().y / 4.0 * 3.0);
        if (playerMonsters[0].move2 == nullptr) {
          menuText.setString("-");
        }
        else {
          menuText.setString(playerMonsters[0].move2->name);
        }

        surface.draw(menuText);

        menuText.setPosition(22, 10 + getView().getSize().y / 4.0 * 3.5);
        if (playerMonsters[0].move3 == nullptr) {
          menuText.setString("-");
        }
        else {
          menuText.setString(playerMonsters[0].move3->name);
        }

        surface.draw(menuText);

        menuText.setPosition(10 + getView().getSize().x / 2.0, 10 + getView().getSize().y / 4.0 * 3.5);
        if (playerMonsters[0].move4 == nullptr) {
          menuText.setString("-");
        }
        else {
          menuText.setString(playerMonsters[0].move4->name);
        }

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