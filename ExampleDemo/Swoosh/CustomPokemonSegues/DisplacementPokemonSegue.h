#pragma once
#include <Swoosh\Segue.h>
#include <Swoosh\Ease.h>
#include <Swoosh\EmbedGLSL.h>

#include "../ResourcePaths.h"

static auto DISPLACEMENT_PATTERN_FRAG_SHADER = GLSL
(
  110,
  uniform sampler2D texture;
  uniform sampler2D pattern;
  uniform float progress;

  void main()
  {
    float direction = mod(floor(gl_FragCoord.y*19.0), 2.0) * 2.0 - 1.0;

    vec4 displacement = texture2D(pattern, gl_TexCoord[0].xy);
    displacement.g = 0.5;

    displacement = ((displacement * 2.0) - 1.0) * progress;

    vec2 newTexCoord = gl_TexCoord[0].xy + displacement.xy;

    //vec2 newTexCoord = gl_TexCoord[0].xy + vec2(direction * progress, 0);
    vec4 pixel = texture2D(texture, newTexCoord);

    if (newTexCoord.x > 1.0 || newTexCoord.x < 0.0) {
      pixel = vec4(0.0, 0.0, 0.0, 1.0);
    }

    vec4 color = gl_Color * pixel;
    gl_FragColor = color;
  }
);

class DisplacementPokemonSegue : public swoosh::Segue {
private:
  sf::Texture* temp;
  sf::Texture pattern;
  sf::Shader shader;
  int count;
  int flashCount;
  double finishFlashSecs;

public:
  virtual void onDraw(sf::RenderTexture& surface) {
    double elapsed = getElapsed().asMilliseconds();
    double duration = getDuration().asMilliseconds();

    // The effect should be 50% of the duration. 
    // If flashCount = 2 then we must divide the duration by 4
    // If flashCount = 5 then we must divide the duration by 10 etc.
    double alpha = swoosh::ease::linear(elapsed - finishFlashSecs, (duration / 2.0), 1.0);

    if (count < flashCount) {
      double alpha = swoosh::ease::inOut(elapsed - finishFlashSecs, duration / (flashCount * 2));

      this->drawLastActivity(surface);

      sf::RectangleShape whiteout;
      whiteout.setSize(sf::Vector2f(surface.getTexture().getSize().x, surface.getTexture().getSize().y));
      whiteout.setFillColor(sf::Color(255, 255, 255, alpha * 255));
      surface.draw(whiteout);

      if (elapsed - finishFlashSecs > duration / (flashCount * 2)) {
        count++;
        // Pickup when we left the flashes
        finishFlashSecs = elapsed;
      }
      return;
    }

    this->drawLastActivity(surface);

    surface.display(); // flip and ready the buffer

    if (temp) delete temp;
    temp = new sf::Texture(surface.getTexture()); // Make a copy of the source texture
    temp->setRepeated(false);

    sf::Sprite left(*temp);
    shader.setUniform("progress", (float)alpha);

    sf::RenderStates states;
    states.shader = &shader;

    surface.clear(sf::Color::Transparent);
    surface.draw(left, states);
    surface.display();

    sf::Texture* temp2 = new sf::Texture(surface.getTexture()); // Make a copy of the source texture
    left = sf::Sprite(*temp2);

    surface.clear(sf::Color::Transparent);

    this->drawNextActivity(surface);

    surface.display(); // flip and ready the buffer
    sf::Sprite right(surface.getTexture());

    getController().getWindow().draw(right);
    getController().getWindow().draw(left);

    delete temp2;
    surface.clear(sf::Color::Transparent);
  }

  DisplacementPokemonSegue(sf::Time duration, swoosh::Activity* last, swoosh::Activity* next) : swoosh::Segue(duration, last, next) {
    /* ... */
    temp = nullptr;

    pattern.loadFromFile(DISPLACEMENT_PATTERN_PATH);
    pattern.setRepeated(true);

    shader.loadFromMemory(DISPLACEMENT_PATTERN_FRAG_SHADER, sf::Shader::Fragment);
    shader.setUniform("texture", sf::Shader::CurrentTexture);
    shader.setUniform("pattern", pattern);

    flashCount = 2;
    count = finishFlashSecs = 0;
  }

  virtual ~DisplacementPokemonSegue() { ; }
};