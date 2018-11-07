#pragma once
#include <SFML\Graphics.hpp>
#include <SFML\Audio.hpp>
#include <Swoosh\Ease.h>
#include <Swoosh\Game.h>
#include <Swoosh\EmbedGLSL.h>
#include "ActionList.h"

#include "Pokemon.h"

#include <iostream>

using namespace swoosh;

class IdleAction : public ActionItem {
private:
  sf::Sprite& ref;

public:
  IdleAction(sf::Sprite& ref) : ref(ref) {

  }

  virtual void update(double elapsed) {
  }

  virtual void draw(sf::RenderTexture& surface) {
    surface.draw(ref);
  }
};

class BobAction : public ActionItem {
private:
  sf::Sprite& ref;
  double total;
  sf::Vector2f bob;
public:
  BobAction(sf::Sprite& ref) : ref(ref) {
    total = 0;
  }

  ~BobAction() {
  }

  virtual void update(double elapsed) {
    total += elapsed;

    bob.x = std::sin(total  * 2) * 4;
    bob.y = std::abs(std::sin(total * 2)) * 5;
  }

  virtual void draw(sf::RenderTexture& surface) {
    auto original = ref.getPosition();
    ref.setPosition(original.x + bob.x, original.y + bob.y);
    surface.draw(ref);
    ref.setPosition(original);
  }
};

class TakeDamage : public BlockingActionItem {
private:
  pokemon::monster& monster;
  int damage;
  int health;
  sf::SoundBuffer& buffer;
  sf::Sound& sound;
  bool playOnce;
public:

  TakeDamage(pokemon::monster& ref, int damage, sf::SoundBuffer& buffer, sf::Sound& sound) : buffer(buffer), sound(sound), monster(ref), BlockingActionItem() {
    this->damage = damage;
    this->health = ref.hp;
    playOnce = false;
  }

  ~TakeDamage() {
    monster.hp = health - damage;

  }

  virtual void update(double elapsed) {
    if (!playOnce) {
      playOnce = true;
      sound.setBuffer(buffer);
      sound.play();
    }

    if (monster.hp > health - damage) {
      monster.hp -= 1;
    }
    else {
      markDone();
    }
  }

  virtual void draw(sf::RenderTexture& surface) {
  }
};

class GainXPAction : public BlockingActionItem {
private:
  pokemon::monster& monster;
  pokemon::monster& defeated;
  sf::SoundBuffer& buffer;
  sf::Sound& sound;
  int xp;
  bool playOnce;
public:

  GainXPAction(pokemon::monster& ref, pokemon::monster& defeated, sf::SoundBuffer& buffer, sf::Sound& sound) : buffer(buffer), sound(sound), monster(ref), defeated(defeated), BlockingActionItem() {
    // In a real game, increase xp by level and other factors
    // In ours, guess the level by the difference in max hp. 
    this->xp = (ref.xp + defeated.xp + std::ceil((defeated.xp*0.5*(defeated.level / ref.level))))/2.0;
    playOnce = false;
  }

  ~GainXPAction() {
    monster.xp = this->xp;
  }

  const int getXP() const { return this->xp; }

  virtual void update(double elapsed) {
    if (!playOnce) {
      playOnce = true;
      sound.setBuffer(buffer);
      sound.play();
    }

    if (this->xp > 0) {
      monster.xp += 1;
      this->xp--;

      if (monster.xp == 100) {
        monster.xp = 0;
        monster.level++;
        monster.maxhp += (int)(monster.maxhp * 0.10);
        monster.hp = monster.maxhp;
      }
    }
    else {
      sound.stop(); // interupt
      markDone();
    }
  }

  virtual void draw(sf::RenderTexture& surface) {
  }
};

class FaintAction : public BlockingActionItem {
private:
  sf::Sprite& ref;
  sf::SoundBuffer &buffer;
  sf::Sound& sound;

  double total;
  sf::IntRect original;

  bool doOnce;
public:
  FaintAction(sf::Sprite& ref, sf::SoundBuffer& buffer, sf::Sound& sound) 
    : ref(ref), buffer(buffer), sound(sound), BlockingActionItem() {
    total = 0;
    original = ref.getTextureRect();
    doOnce = false;
  }

  ~FaintAction() {
  }

  virtual void update(double elapsed) {
    if (!doOnce) {
      sound.setBuffer(buffer);
      sound.play();
      doOnce = true;
    }

    total += elapsed;
    double alpha = 1.0 - ease::linear(total, 0.25, 1.0);

    ref.setTextureRect(sf::IntRect(0, 0, original.width, original.height*alpha));
    game::setOrigin(ref, ref.getOrigin().x/ref.getTexture()->getSize().x, 1.0f); // update origin pos

    if (total >= 0.25)
      markDone();
  }

  virtual void draw(sf::RenderTexture& surface) {
    surface.draw(ref);
  }
};

class BattleScene;

class FlamethrowerAction : public BlockingActionItem {
private:
  sf::Sprite& ref;
  sf::Sprite& target;
  sf::SoundBuffer &buffer;
  sf::Sound& sound;
  sf::Texture& flame;
  int facing;

  sf::Vector2f original;
  BattleScene& scene;

  double total;
  bool doOnce;
public:
  FlamethrowerAction(BattleScene& scene, sf::Sprite& ref, int facing, sf::SoundBuffer& buffer, sf::Sound& sound, sf::Sprite& target, sf::Texture& flame);
  ~FlamethrowerAction();

  virtual void update(double elapsed);
  virtual void draw(sf::RenderTexture& surface);
};


class TackleAction : public BlockingActionItem {
private:
  sf::Sprite& ref;
  int facing;
  double total;
  sf::Vector2f original;
public:
  TackleAction(sf::Sprite& ref, int facing) : ref(ref), facing(facing) {
    total = 0;
    original = ref.getPosition();
  }

  ~TackleAction() { ref.setPosition(original); }

  virtual void update(double elapsed) {
    total += elapsed;

    double alpha = ease::inOut(total, 0.50);
    int offsetx = 50.0 * alpha * (facing ? -1 : 1);
    ref.setPosition(original.x + offsetx, original.y);

    if (total > 0.50) {
      markDone();
    }
  }

  virtual void draw(sf::RenderTexture& surface) {
    surface.draw(ref);
  }
};

class TailWhipAction : public BlockingActionItem {
private:
  sf::Sprite& ref;
  double total;
  sf::Vector2f original;
  sf::SoundBuffer& buffer;
  sf::Sound& sound;
  bool doOnce;
public:
  TailWhipAction(sf::Sprite& ref, sf::SoundBuffer& buffer, sf::Sound& sound) : buffer(buffer), sound(sound), ref(ref) {
    total = 0;
    original = ref.getPosition();
    doOnce = false;
  }

  ~TailWhipAction() { ref.setPosition(original); }

  virtual void update(double elapsed) {
    if (!doOnce) {
      sound.setBuffer(buffer);
      sound.play();
      doOnce = true;
    }

    total += elapsed;
    double alpha = sin(total*10) * 15;
    ref.setPosition(alpha + original.x, original.y);

    if (total >= 1.5) {
      markDone();
      ref.setPosition(original);
    }
  }

  virtual void draw(sf::RenderTexture& surface) {
    surface.draw(ref);
  }
};


class FlyAction : public BlockingActionItem {
private:
  sf::Sprite& ref;
  pokemon::monster& monster;
  double total;
  sf::SoundBuffer& buffer;
  sf::Sound& sound;
  bool doOnce;
public:
  FlyAction(sf::Sprite& ref, pokemon::monster& monster, sf::SoundBuffer& buffer, sf::Sound& sound) : monster(monster), buffer(buffer), sound(sound), ref(ref) {
    total = 0;
    doOnce = false;
  }

  ~FlyAction() { ; }

  virtual void update(double elapsed) {
    if (!doOnce) {
      sound.setBuffer(buffer);
      sound.play();
      doOnce = true;
    }

    if (!monster.isFlying) {
      total += elapsed;
      double alpha = ease::linear(total, 0.25, 1.0);
      ref.setScale(1.0 - alpha, 1.0 + (alpha * 2));

      if (total >= 0.25) {
        markDone();
        ref.setScale(0, 0); // hide
        monster.isFlying = true;
      }
    }
    else {
      ref.setScale(1, 1);
      monster.isFlying = false;
      markDone();
    }
  }

  virtual void draw(sf::RenderTexture& surface) {
    surface.draw(ref);
  }
};

class RoarAction : public BlockingActionItem {
private:
  sf::Sprite& ref;
  double total;
  bool swell;
  bool doOnce;
  sf::SoundBuffer** buffer;
  sf::Sound& sound;
public:
  RoarAction(sf::Sprite& ref, sf::SoundBuffer** buffer, sf::Sound& roar, bool swell=true) : buffer(buffer), sound(roar), swell(swell), ref(ref) {
    total = 0;
    doOnce = false;
  }

  virtual void update(double elapsed) {
    if (!doOnce) {
      sound.setBuffer(**buffer);
      sound.play();
      doOnce = true;
    }

    total += elapsed;

    if (swell) {
      double alpha = ease::wideParabola(total, 1.0, 1.0);
      double scale = 1.0 + (alpha * 0.25);
      ref.setScale(scale, scale);
    }

    if (swell) { // intro is longer
      if (total >= 2.0)
        markDone();
    }
    else { // fainting is quicker
      if (total >= 0.75)
        markDone();
    }
  }

  virtual void draw(sf::RenderTexture& surface) {
    surface.draw(ref);
  }
};


class ChangeText : public ActionItem {
private:
  std::string input;
  std::string& output;
public:
  ChangeText(std::string& output, std::string input) : output(output), ActionItem() {
    this->input = input;
  }

  virtual void update(double elapsed) {
    output = input;
    markDone();
  }

  virtual void draw(sf::RenderTexture& surface) {
  }
};

class WaitForButtonPressAction : public BlockingActionItem {
private:
  sf::Keyboard::Key button;
  sf::SoundBuffer& buffer;
  sf::Sound& sound;
  bool isPressed;
  bool wasHeldBefore;
  double total;
public:
  WaitForButtonPressAction(sf::Keyboard::Key button, sf::SoundBuffer& buffer, sf::Sound& sound)
    : buffer(buffer), sound(sound), button(button), BlockingActionItem() {
    isPressed = false;
    total = 0;
    wasHeldBefore = false;
  }

  virtual void update(double elapsed) {
    if (total == 0) {
      wasHeldBefore = sf::Keyboard::isKeyPressed(button);
    }

    total += elapsed;

    if (!sf::Keyboard::isKeyPressed(button)) {
      if (wasHeldBefore) {
        wasHeldBefore = false;
        isPressed = false;
      }
      else if(isPressed) {
        markDone();
      }
      else {
        isPressed = false;
      }
    }
    else if (sf::Keyboard::isKeyPressed(button) && !isPressed && !wasHeldBefore) {
      sound.setBuffer(buffer);
      sound.play();
      isPressed = true;
    }
  }

  virtual void draw(sf::RenderTexture& surface) {
  }
};

static auto MOVING_TEXTURE_SHADER = GLSL
(
  110,
  uniform sampler2D texture;
  uniform sampler2D pattern;
  uniform float progress;

  void main()
  {
    vec4 pixel = texture2D(texture, vec2(gl_TexCoord[0].xy));
    vec4 other = texture2D(pattern, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y+progress));

    vec4 color = gl_Color * pixel;

    if (pixel.a > 0.0) {
      color = gl_Color * mix(pixel, other, 0.5);
    } 

    gl_FragColor = color;
  }
);

class DefenseDownAction : public BlockingActionItem {
private:
  sf::Sprite& ref;
  sf::Shader shader;
  sf::Texture* ddown;
  sf::SoundBuffer& buffer;
  sf::Sound& sound;
  double total;
  bool playedOnce;
public:
  DefenseDownAction(sf::Sprite& ref, sf::Texture& texture, sf::SoundBuffer& buffer, sf::Sound& sound) : buffer(buffer), sound(sound), ref(ref) {
    shader.loadFromMemory(MOVING_TEXTURE_SHADER, sf::Shader::Type::Fragment);
    shader.setUniform("texture", sf::Shader::CurrentTexture);

    ddown = &texture;
    ddown->setRepeated(true);
    shader.setUniform("pattern", *ddown);

    total = 0;

    playedOnce = false;
  }

  ~DefenseDownAction() {
    delete ddown;
  }

  virtual void update(double elapsed) {
    if (!playedOnce) {
      sound.setBuffer(buffer);
      sound.play();
      playedOnce = true;
    }

    total += elapsed;
    double alpha = ease::wideParabola(total, 2.0, 1.0);

    if (total >= 2.0)
      markDone();

    shader.setUniform("progress", (float)total*-2);
  }

  virtual void draw(sf::RenderTexture& surface) {
    sf::RenderStates states;
    states.shader = &shader;
    surface.draw(ref, states);
  }
};

class AttackUpAction : public BlockingActionItem {
private:
  sf::Sprite& ref;
  sf::Shader shader;
  sf::Texture* aup;
  sf::SoundBuffer& buffer;
  sf::Sound& sound;
  double total;
  bool playedOnce;
public:
  AttackUpAction(sf::Sprite& ref, sf::Texture& texture, sf::SoundBuffer& buffer, sf::Sound& sound) : buffer(buffer), sound(sound), ref(ref) {
    shader.loadFromMemory(MOVING_TEXTURE_SHADER, sf::Shader::Type::Fragment);
    shader.setUniform("texture", sf::Shader::CurrentTexture);

    aup = &texture;
    aup->setRepeated(true);
    shader.setUniform("pattern", *aup);

    total = 0;

    playedOnce = false;
  }

  ~AttackUpAction() {
    delete aup;
  }

  virtual void update(double elapsed) {
    if (!playedOnce) {
      sound.setBuffer(buffer);
      sound.play();
      playedOnce = true;
    }

    total += elapsed;
    double alpha = ease::wideParabola(total, 2.0, 1.0);

    if (total >= 2.0)
      markDone();

    shader.setUniform("progress", (float)total*2);
  }

  virtual void draw(sf::RenderTexture& surface) {
    sf::RenderStates states;
    states.shader = &shader;
    surface.draw(ref, states);
  }
};