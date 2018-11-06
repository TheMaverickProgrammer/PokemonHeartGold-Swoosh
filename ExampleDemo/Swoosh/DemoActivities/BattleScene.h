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

auto WHITE_SHADER = GLSL
(
  110,
  uniform sampler2D texture;
  uniform float opacity;

  void main()
  {
    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);
    vec4 color = gl_Color * pixel;
    color = vec4(1.0, 1.0, 1.0, color.a)*opacity + (1.0 - opacity)*color;
    gl_FragColor = color;
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
  sf::Texture *particleTexture;

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
  sf::SoundBuffer selectBuffer;
  sf::SoundBuffer attackWeakBuffer;
  sf::SoundBuffer attackNormalBuffer;
  sf::SoundBuffer attackSuperBuffer;
  sf::SoundBuffer xpBuffer;
  sf::SoundBuffer tailWhipBuffer;
  sf::SoundBuffer flyBuffer;

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

  std::vector<particle> particles;

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
    sf::Shader shader;
  public:
    SpawnNewPokemon(BattleScene* ref) : ref(ref), BlockingActionItem() {
      isLoaded = false;
      scale = total = 0;
      shader.loadFromMemory(WHITE_SHADER, sf::Shader::Fragment);
      shader.setUniform("texture", sf::Shader::CurrentTexture);
    }

    virtual void update(double elapsed) {
      total += elapsed;

      if (!isLoaded) {
        this->ref->playerMonsters.erase(this->ref->playerMonsters.begin());
        this->ref->loadPlayerPokemonData();
        isLoaded = true;
        ref->spawnParticles(this->ref->playerSprite.getPosition());
      }

      scale = ease::linear(total, 0.5, 1.0);
      this->ref->playerSprite.setScale(scale, scale);

      shader.setUniform("opacity", 1.0f-(float)scale);

      if (total > 0.5)
        markDone();
    }

    virtual void draw(sf::RenderTexture& surface) { 
      sf::RenderStates states;
      states.shader = &shader;

      surface.draw(this->ref->playerSprite, states);
    }
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
        this->ref->actions.add(new ChangeText(ref->output, std::string() + ref->wild.name + " fainted!"));
        this->ref->actions.add(new WaitForButtonPressAction(sf::Keyboard::Key::Enter, ref->selectBuffer, ref->sound));
        this->ref->actions.add(new GainXPAction(ref->playerMonsters[0], ref->wild, ref->xpBuffer, ref->sound));
        this->ref->actions.add(new WaitForButtonPressAction(sf::Keyboard::Key::Enter, ref->selectBuffer, ref->sound));

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
        this->ref->actions.add(new ChangeText(ref->output, std::string() + ref->playerMonsters[0].name + " fainted!"));
        this->ref->actions.add(new WaitForButtonPressAction(sf::Keyboard::Key::Enter, ref->selectBuffer, ref->sound));

        
        if (ref->playerMonsters.size() - 1 == 0) {
          // we're out of pokemon
          this->ref->actions.add(new ChangeText(ref->output, std::string() + "Trainer whited out!"));
          this->ref->actions.add(new WaitForButtonPressAction(sf::Keyboard::Key::Enter, ref->selectBuffer, ref->sound));

          leaveScene = true;
        }
        else {
          this->ref->actions.add(new ChangeText(ref->output, std::string() + "Go " + ref->playerMonsters[1].name + "!"));
          this->ref->actions.add(new SpawnNewPokemon(this->ref));
          this->ref->actions.add(new RoarAction(ref->playerSprite, ref->playerRoarBuffer, ref->sound));
          this->ref->actions.add(new IdleAction(ref->playerSprite));
          this->ref->actions.add(new WaitForButtonPressAction(sf::Keyboard::Key::Enter, ref->selectBuffer, ref->sound));
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
    selectBuffer.loadFromFile(SELECT_SFX);
    attackWeakBuffer.loadFromFile(HIT_WEAK_SFX);
    attackNormalBuffer.loadFromFile(HIT_NORMAL_SFX);
    attackSuperBuffer.loadFromFile(HIT_SUPER_SFX);
    xpBuffer.loadFromFile(XP_SFX);
    tailWhipBuffer.loadFromFile(TAIL_WHIP_SFX);
    flyBuffer.loadFromFile(FLY_SFX);
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
    particleTexture = loadTexture(PARTICLE_PATH);

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
    menuText.setScale(0.45, 0.45);

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
    setOrigin(playerSprite, 0.5, 1.0);
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
    
    // Our attacks miss
    if (second->isFlying) {
      actions.add(new ChangeText(output, std::string(first->name) + " missed!"));
      actions.add(new WaitForButtonPressAction(sf::Keyboard::Enter, selectBuffer, sound));
      return;
    }

    if (first->isFlying) {
      actions.add(new ChangeText(output, std::string(first->name) + " is flying!"));
      actions.add(new WaitForButtonPressAction(sf::Keyboard::Enter, selectBuffer, sound));
      actions.add(new FlyAction(*firstSprite, *first, flyBuffer, sound));
      actions.add(new ChangeText(output, std::string(first->name) + " attacked\nform above!"));
      actions.add(new TakeDamage(*second, firstchoice->damage, attackNormalBuffer, sound));
      actions.add(new WaitForButtonPressAction(sf::Keyboard::Enter, selectBuffer, sound));
      actions.add(new SignalCheckHP(this));
      actions.add(new WaitForButtonPressAction(sf::Keyboard::Enter, selectBuffer, sound));
    }
    else {
      if (firstchoice->name == "tackle") {
        actions.add(new ChangeText(output, std::string(first->name) + " used tackle!"));
        actions.add(new TackleAction(*firstSprite, facing));
        actions.add(new TakeDamage(*second, firstchoice->damage, attackNormalBuffer, sound));
        actions.add(new SignalCheckHP(this));
        actions.add(new WaitForButtonPressAction(sf::Keyboard::Enter, selectBuffer, sound));

      }
      else if (firstchoice->name == "tail whip") {
        actions.add(new ChangeText(output, std::string(first->name) + " used tail whip!"));
        actions.add(new TailWhipAction(*firstSprite, tailWhipBuffer, sound));
        actions.add(new DefenseDownAction(*secondSprite, statsFallBuffer, sound));
        actions.add(new WaitForButtonPressAction(sf::Keyboard::Enter, selectBuffer, sound));
        actions.add(new ChangeText(output, std::string(second->name) + "'s defense\nfell sharply!"));
        actions.add(new SignalCheckHP(this));
        actions.add(new WaitForButtonPressAction(sf::Keyboard::Enter, selectBuffer, sound));

      }
      else if (firstchoice->name == "roar") {
        actions.add(new ChangeText(output, std::string(first->name) + " used roar!"));
        actions.add(new RoarAction(*firstSprite, *firstRoarBuffer, sound));
        actions.add(new AttackUpAction(*firstSprite, statsRiseBuffer, sound));
        actions.add(new WaitForButtonPressAction(sf::Keyboard::Enter, selectBuffer, sound));
        actions.add(new ChangeText(output, std::string(first->name) + "'s attack\nrose!"));
        actions.add(new SignalCheckHP(this));
        actions.add(new WaitForButtonPressAction(sf::Keyboard::Enter, selectBuffer, sound));

      }
      else if (firstchoice->name == "thunder") {
        actions.add(new ChangeText(output, std::string(first->name) + " used thunder!"));
        actions.add(new TakeDamage(*second, firstchoice->damage, attackNormalBuffer, sound));
        actions.add(new SignalCheckHP(this));
        actions.add(new WaitForButtonPressAction(sf::Keyboard::Enter, selectBuffer, sound));

      }
      else if (firstchoice->name == "fly") {
        actions.add(new FlyAction(*firstSprite, *first, flyBuffer, sound));
        actions.add(new ChangeText(output, std::string(first->name) + " used fly!"));
        actions.add(new WaitForButtonPressAction(sf::Keyboard::Enter, selectBuffer, sound));
        actions.add(new ChangeText(output, std::string(first->name) + " flew\nup into the sky!"));
        actions.add(new SignalCheckHP(this));
        actions.add(new WaitForButtonPressAction(sf::Keyboard::Enter, selectBuffer, sound));

      }
      else if (firstchoice->name == "nomove") {
        actions.add(new ChangeText(output, std::string(first->name) + " is struggling!"));
        actions.add(new TakeDamage(*second, firstchoice->damage, attackNormalBuffer, sound));
        actions.add(new TakeDamage(*first, firstchoice->damage, attackNormalBuffer, sound));
        actions.add(new SignalCheckHP(this));
        actions.add(new WaitForButtonPressAction(sf::Keyboard::Enter, selectBuffer, sound));

      }
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
      wild.xp = 25; // worth 25 xp
      break;
    case 1:
      wild = pokemon::monster(pokemon::clefairy);
      wildTexture = loadTexture(CLEFAIRY_PATH[FACING::FRONT]);
      wildSprite = sf::Sprite(*wildTexture);
      wildRoarBuffer.loadFromFile(CLEFAIRY_PATH[2]);
      wild.xp = 50; // worth 50 xp

      break;
    case 2:
      wild = pokemon::monster(pokemon::geodude);
      wildTexture = loadTexture(GEODUDE_PATH[FACING::FRONT]);
      wildSprite = sf::Sprite(*wildTexture);
      wildRoarBuffer.loadFromFile(GEODUDE_PATH[2]);
      wild.xp = 33; // worth 33 xp

      break;
    case 3:
      wild = pokemon::monster(pokemon::ponyta);
      wildTexture = loadTexture(PONYTA_PATH[FACING::FRONT]);
      wildSprite = sf::Sprite(*wildTexture);
      wildRoarBuffer.loadFromFile(PONYTA_PATH[2]);
      wild.xp = 21; // worth 21 xp

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
      wild.xp = 15; // worth 15 xp

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

  void updateParticles(double elapsed) {
    int i = 0;
    for (auto& p : particles) {
      p.speed = sf::Vector2f(p.speed.x * p.friction.x, p.speed.y * p.friction.y);
      p.pos += sf::Vector2f(p.speed.x * elapsed, p.speed.y * elapsed);

      p.sprite.setPosition(p.pos);
      p.sprite.setScale(2.0*(p.life / p.lifetime), 2.0*(p.life / p.lifetime));
      p.sprite.setColor(sf::Color(p.sprite.getColor().r, p.sprite.getColor().g, p.sprite.getColor().b, 255 * p.life / p.lifetime));
      p.life -= elapsed;

      if (p.life <= 0) {
        particles.erase(particles.begin() + i);
        continue;
      }

      i++;
    }
  }

  void spawnParticles(sf::Vector2f position) {
    for (int i = 100; i > 0; i--) {
      int randNegative = rand() % 2 == 0 ? -1 : 1;
      int randSpeedX = rand() % 220;
      randSpeedX *= randNegative;
      int randSpeedY = rand() % 220;

      particle p;
      p.sprite = sf::Sprite(*particleTexture);
      p.pos = position;
      p.speed = sf::Vector2f(randSpeedX, -randSpeedY);
      p.friction = sf::Vector2f(0.99999f, 0.9999f);
      p.life = 1.5;
      p.lifetime = 1.5;
      p.sprite.setPosition(p.pos);

      particles.push_back(p);
    }
  }


  virtual void onUpdate(double elapsed) {
    updateParticles(elapsed);

    if (fadeMusic) {
      battleMusic.setVolume(battleMusic.getVolume() * 0.90); // quieter
    }

    if (waitTimer.getElapsed().asSeconds() > 2) {
      if (!preBattleSetup) {
        actions.add(new ClearPreviousActions());
        actions.add(new IdleAction(wildSprite));
        actions.add(new BobAction(playerSprite));

        if (!doIntro) {
          actions.add(new ChangeText(output, "A wild " + std::string(wild.name) + " appeard!"));
          actions.add(new RoarAction(wildSprite, wildRoarBuffer, sound));
          actions.add(new WaitForButtonPressAction(sf::Keyboard::Enter, selectBuffer, sound));
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

      if (playerMonsters[0].isFlying) {
        isKeyDown = false; 
        canInteract = false;
        choice = &pokemon::fly;

        actions.add(new ChangeText(output, "Your pokemon is still\nin the sky!"));
        actions.add(new WaitForButtonPressAction(sf::Keyboard::Space, selectBuffer, sound));
        generateBattleActions(*choice);
      }
      else {
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
      }

      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) && !isKeyDown) {
        if (choice != nullptr) {
          isKeyDown = true;
          sound.setBuffer(selectBuffer);
          sound.play();
        } // else ignore or play a buzzer sound
      }
      else if (canInteract && !sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) && isKeyDown) {
        isKeyDown = false; // force player to release key to press again
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

      for (auto& p : particles) {
        surface.draw(p.sprite);
      }

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
    delete particleTexture;
  }
};