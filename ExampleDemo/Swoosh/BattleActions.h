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
    double alpha = 1.0 - ease::linear(total, 1.0, 1.0);

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

    double alpha = ease::inOut(total, 1.0);
    int offsetx = 20.0 * alpha * (facing ? -1 : 1);
    ref.setPosition(original.x + offsetx, original.y);

    if (total > 1.0) {
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
public:
  TailWhipAction(sf::Sprite& ref) : ref(ref) {
    total = 0;
  }

  virtual void update(double elapsed) {
    total += elapsed;
    double alpha = ease::wideParabola(total, 1.0, 1.0);
    double scale = 1.0 + (alpha * 0.25);
    ref.setScale(scale, scale);

    if (total >= 2.0)
      markDone();
  }

  virtual void draw(sf::RenderTexture& surface) {
    surface.draw(ref);
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
  double total;
public:
  DefenseDownAction(sf::Sprite& ref) : ref(ref) {
    shader.loadFromMemory(MOVING_TEXTURE_SHADER, sf::Shader::Type::Fragment);
    shader.setUniform("texture", sf::Shader::CurrentTexture);

    ddown = loadTexture(DEFENSE_DOWN_PATH);
    ddown->setRepeated(true);
    shader.setUniform("pattern", *ddown);

    total = 0;
  }

  ~DefenseDownAction() {
    delete ddown;
  }

  virtual void update(double elapsed) {
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
  double total;
public:
  AttackUpAction(sf::Sprite& ref) : ref(ref) {
    shader.loadFromMemory(MOVING_TEXTURE_SHADER, sf::Shader::Type::Fragment);
    shader.setUniform("texture", sf::Shader::CurrentTexture);

    aup = loadTexture(ATTACK_UP_PATH);
    aup->setRepeated(true);
    shader.setUniform("pattern", *aup);

    total = 0;
  }

  ~AttackUpAction() {
    delete aup;
  }

  virtual void update(double elapsed) {
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