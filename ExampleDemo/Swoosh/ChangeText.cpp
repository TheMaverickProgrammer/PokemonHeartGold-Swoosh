#include "pch.h"
#include "ChangeText.h"

ChangeText::ChangeText(std::string& output, std::string input) : output(output), ActionItem() {
  this->input = input;
}

void ChangeText::update(double elapsed) {
  output = input;
  markDone();
}

void ChangeText::draw(sf::RenderTexture& surface) {
}