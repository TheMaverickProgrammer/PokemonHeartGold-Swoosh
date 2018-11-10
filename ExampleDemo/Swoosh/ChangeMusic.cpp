#include "pch.h"
#include "ChangeMusic.h"

ChangeMusic::ChangeMusic(sf::Music& music, std::string path) : music(music), path(path) {
}

void ChangeMusic::update(double elapsed) {
  music.openFromFile(path);
  music.play();
  markDone();
}

void ChangeMusic::draw(sf::RenderTexture& surface) {
}