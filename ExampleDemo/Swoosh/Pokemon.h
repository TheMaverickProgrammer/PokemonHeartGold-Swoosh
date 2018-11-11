#pragma once

namespace pokemon {
  enum facing : int {
    BACK,
    FRONT
  };

  enum element : int {
    N_A, // Not applicable
    ELECTRIC,
    GROUND,
    FIRE,
    WATER,
    FLYING,
    GRASS,
    NORMAL
  };

  struct moves {
    const char* name;
    element type;
    int damage;
    int speed;
  };

  struct monster {
    const char* name;
    const moves* move1;
    const moves* move2;
    const moves* move3;
    const moves* move4;
    element type1;
    element type2;
    int level;
    int hp;
    int maxhp;
    int xp;
    bool isFlying;
  };


  const moves tackle       = { "tackle", element::NORMAL, 30, 30 };
  const moves thundershock = { "thunder", element::ELECTRIC, 40, 20 };
  const moves flamethrower = { "flamethrower", element::FIRE, 50, 10 };
  const moves fly     = { "fly", element::FLYING, 40, 10 };
  const moves tailwhip = { "tail whip", element::NORMAL, 0, 100 };
  const moves roar =   { "roar", element::NORMAL, 0, 30 };
  const moves nomove = { "-", element::NORMAL, 30, 0 };

  const monster pidgey = {
    "pidgey",
    &tackle,
    &tailwhip,
    nullptr,
    &fly,
    element::N_A,
    element::NORMAL,
    5,
    100,
    100,
    0,
    false
  };

  const monster geodude = {
   "geodude",
   &tackle,
   &roar,
   nullptr,
   nullptr,
   element::N_A,
   element::GROUND,
   5,
   200,
   200,
   0,
   false
  };

  const monster ponyta = {
   "ponyta",
   &tackle,
   &tailwhip,
   nullptr,
   &flamethrower,
   element::N_A,
   element::FIRE,
   5,
   150,
   150,
   0,
   false
  };

  const monster onyx = {
   "onix",
   &tackle,
   &tailwhip,
   nullptr,
   nullptr,
   element::N_A,
   element::GROUND,
   5,
   200,
   200,
   0,
   false
  };

  const monster oddish = {
   "oddish",
   &tackle,
   nullptr,
   nullptr,
   nullptr,
   element::N_A,
   element::GRASS,
   5,
   80,
   80,
   0,
   false
  };

  const monster clefairy = {
   "clefairy",
   &tackle,
   &fly,
   &tailwhip,
   &flamethrower,
   element::N_A,
   element::NORMAL,
   5,
   110,
   110,
   0,
   false
  };

  const monster cubone = {
   "cubone",
   &tackle,
   &tailwhip,
   nullptr,
   nullptr,
   element::N_A,
   element::GROUND,
   5,
   100,
   100,
   0,
   false
  };

  const monster pikachu = {
  "pikachu",
  &tackle,
  &tailwhip,
  &thundershock,
  &roar,
  element::N_A,
  element::ELECTRIC,
  5,
  20,
  80,
  0,
  false
  };

  const monster charizard = {
  "charizard",
  &tackle,
  &fly,
  &flamethrower,
  nullptr,
  element::N_A,
  element::FIRE,
  5,
  120,
  120,
  0,
  false
  };

  const monster piplup = {
  "piplup",
  &tackle,
  nullptr,
  nullptr,
  nullptr,
  element::N_A,
  element::WATER,
  5,
  30,
  70,
  0,
  false
  };

  const monster roserade = {
  "roserade",
  &tackle,
  nullptr,
  nullptr,
  nullptr,
  element::N_A,
  element::GRASS,
  5,
  100,
  100,
  0,
  false
  };
}
