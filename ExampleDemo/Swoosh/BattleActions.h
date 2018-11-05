#pragma once
#include <Swoosh\Ease.h>
#include <Swoosh\Game.h>
#include "ResourcePaths.h"

#include <iostream>

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
  sf::Vector2f original;
public:
  BobAction(sf::Sprite& ref) : ref(ref) {
    total = 0;
    original = ref.getPosition();
  }

  ~BobAction() {
    ref.setPosition(original);
  }

  virtual void update(double elapsed) {
    total += elapsed;

    int bobx = std::sin(total  * 2) * 4;
    int boby = std::abs(std::sin(total * 2)) * 5;
    ref.setPosition(original.x + bobx, original.y + boby);
  }

  virtual void draw(sf::RenderTexture& surface) {
    surface.draw(ref);
  }
};

class TakeDamage : public ActionItem {
private:
  pokemon::monster& monster;
  int damage;
public:
  TakeDamage(pokemon::monster& ref, int damage) : monster(ref) {
    this->damage = damage;
  }

  ~TakeDamage() {
  }

  virtual void update(double elapsed) {
    monster.hp -= damage;
    markDone();
  }

  virtual void draw(sf::RenderTexture& surface) {
  }
};

class FaintAction : public ActionItem {
private:
  sf::Sprite& ref;
  double total;
  sf::IntRect original;
public:
  FaintAction(sf::Sprite& ref) : ref(ref) {
    total = 0;
    original = ref.getTextureRect();
  }

  ~FaintAction() {
  }

  virtual void update(double elapsed) {
    total += elapsed;
    double alpha = 1.0 - ease::linear(total, 0.5, 1.0);

    ref.setTextureRect(sf::IntRect(0, 0, original.width, original.height*alpha));
    game::setOrigin(ref, 0.5f, 1.0f); // update origin pos
  }

  virtual void draw(sf::RenderTexture& surface) {
    surface.draw(ref);
  }
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

public:
  TailWhipAction(sf::Sprite& ref) : ref(ref) {
    total = 0;
    original = ref.getPosition();
  }

  ~TailWhipAction() { ref.setPosition(original); }

  virtual void update(double elapsed) {
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

class RoarAction : public BlockingActionItem {
private:
  sf::Sprite& ref;
  double total;
  bool swell;
  bool doOnce;
  sf::SoundBuffer& buffer;
  sf::Sound& sound;
public:
  RoarAction(sf::Sprite& ref, sf::SoundBuffer& buffer, sf::Sound& roar, bool swell=true) : buffer(buffer), sound(roar), swell(swell), ref(ref) {
    total = 0;
    doOnce = false;
  }

  virtual void update(double elapsed) {
    if (!doOnce) {
      sound.setBuffer(buffer);
      sound.play();
      doOnce = true;
    }

    total += elapsed;

    if (swell) {
      double alpha = ease::wideParabola(total, 1.0, 1.0);
      double scale = 1.0 + (alpha * 0.25);
      ref.setScale(scale, scale);
    }

    if (total >= 2.0)
      markDone();
  }

  virtual void draw(sf::RenderTexture& surface) {
    surface.draw(ref);
  }
};


class WaitForButtonPressAction : public BlockingActionItem {
private:
  sf::Keyboard::Key button;
  std::string& output;
  std::string input;
  bool isPressed;
public:
  WaitForButtonPressAction(std::string& output, std::string input, sf::Keyboard::Key button) : output(output), button(button) {
    this->input = input;
    isPressed = false;
  }

  virtual void update(double elapsed) {
    output = input;
    if (!sf::Keyboard::isKeyPressed(button) && isPressed)
      markDone();
    else if (sf::Keyboard::isKeyPressed(button) && !isPressed)
      isPressed = true;
  }

  virtual void draw(sf::RenderTexture& surface) {
  }
};

auto MOVING_TEXTURE_SHADER = GLSL
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
  DefenseDownAction(sf::Sprite& ref, sf::SoundBuffer& buffer, sf::Sound& sound) : buffer(buffer), sound(sound), ref(ref) {
    shader.loadFromMemory(MOVING_TEXTURE_SHADER, sf::Shader::Type::Fragment);
    shader.setUniform("texture", sf::Shader::CurrentTexture);

    ddown = loadTexture(DEFENSE_DOWN_PATH);
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
  AttackUpAction(sf::Sprite& ref, sf::SoundBuffer& buffer, sf::Sound& sound) : buffer(buffer), sound(sound), ref(ref) {
    shader.loadFromMemory(MOVING_TEXTURE_SHADER, sf::Shader::Type::Fragment);
    shader.setUniform("texture", sf::Shader::CurrentTexture);

    aup = loadTexture(ATTACK_UP_PATH);
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