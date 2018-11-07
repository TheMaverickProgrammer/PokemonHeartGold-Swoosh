#pragma once
#include <SFML\Graphics.hpp>
#include <SFML\Audio.hpp>
#include "TMXMapLayer.h"
#include "ResourcePaths.h"

#include <atomic>

/*
A very light-weight resource manager 
do not recommend for production
*/
class ResourceManager
{
public:
  tmx::Map map;
  MapLayer *layerZero;
  MapLayer *layerOne;
  MapLayer *layerTwo;   // collision
  MapLayer *layerThree; // spawn areas

  sf::Texture *owPlayerTexture;
  sf::Texture *playerTexture;
  sf::Texture *wildTexture;
  sf::Texture *battleAreaTexture;
  sf::Texture *battlePadFGTexture;
  sf::Texture *battlePadBGTexture;
  sf::Texture *textboxTexture;
  sf::Texture *playerStatusTexture;
  sf::Texture *enemyStatusTexture;
  sf::Texture *particleTexture;
  sf::Texture *titleTexture;
  sf::Texture *ddownTexture;
  sf::Texture *aupTexture;

  sf::SoundBuffer *wildRoarBuffer;
  sf::SoundBuffer *playerRoarBuffer;
  sf::SoundBuffer *statsFallBuffer;
  sf::SoundBuffer *statsRiseBuffer;
  sf::SoundBuffer *selectBuffer;
  sf::SoundBuffer *attackWeakBuffer;
  sf::SoundBuffer *attackNormalBuffer;
  sf::SoundBuffer *attackSuperBuffer;
  sf::SoundBuffer *xpBuffer;
  sf::SoundBuffer *tailWhipBuffer;
  sf::SoundBuffer *flyBuffer;

  sf::Font *menuFont;

  static sf::Texture* loadTexture(const std::string& path) {
    sf::Texture* texture = new sf::Texture();
    if (!texture->loadFromFile(path)) {
      throw std::runtime_error("Texture at " + path + " failed to load");
    }

    return texture;
  }

  static sf::Font* loadFont(const std::string& path) {
    sf::Font* font = new sf::Font();
    if (!font->loadFromFile(path)) {
      throw std::runtime_error("Font at " + path + " failed to load");
    }

    return font;
  }

  static sf::SoundBuffer* loadSound(const std::string& path) {
    sf::SoundBuffer* buffer = new sf::SoundBuffer();
    if (!buffer->loadFromFile(path)) {
      throw std::runtime_error("Sound at " + path + " failed to load");
    }

    return buffer;
  }

  /*
  In a production scenario, the manager would add up the total
  number. We'd queue a resource for loading somewhere else behfore hand.*/
  const int getResourceCount() { return 27;  }

  void load(std::atomic<int>& progress);

  ResourceManager();
  ~ResourceManager();
};

