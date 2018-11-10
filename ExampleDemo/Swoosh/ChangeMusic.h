#pragma once
#include <SFML\Audio.hpp>
#include "ActionList.h"

class ChangeMusic : public ActionItem {
private:
  sf::Music& music;
  std::string path;
public:
  ChangeMusic(sf::Music& music, std::string path);
  virtual void update(double elapsed);
  virtual void draw(sf::RenderTexture& surface);
};