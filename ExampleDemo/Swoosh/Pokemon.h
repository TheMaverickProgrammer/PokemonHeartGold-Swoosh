#pragma once

namespace pokemon {
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
    int hp;
    int maxhp;
    int xp;
  };


  const moves tackle       = { "tackle", element::NORMAL, 30, 30 };
  const moves thundershock = { "thunder", element::ELECTRIC, 40, 10 };
  const moves fly     = { "fly", element::FLYING, 40, 10 };
  const moves tailwhip = { "tail whip", element::NORMAL, 0, 100 };
  const moves roar =   { "roar", element::NORMAL, 0, 30 };
  const moves nomove = { "-", element::NORMAL, 0, 0 };

  const monster pidgey = {
    "pidgey",
    &tackle,
    &tailwhip,
    nullptr,
    nullptr,
    element::N_A,
    element::NORMAL,
    200,
    200,
    0
  };

  const monster geodude = {
   "geodude",
   &tackle,
   &roar,
   nullptr,
   nullptr,
   element::N_A,
   element::GROUND,
   200,
   200,
   0
  };

  const monster ponyta = {
   "ponyta",
   &tackle,
   &tailwhip,
   nullptr,
   nullptr,
   element::N_A,
   element::FIRE,
   200,
   200,
   0
  };

  const monster onyx = {
   "onyx",
   &tackle,
   &tailwhip,
   nullptr,
   nullptr,
   element::N_A,
   element::GROUND,
   200,
   200,
   0
  };

  const monster oddish = {
   "oddish",
   &tackle,
   nullptr,
   nullptr,
   nullptr,
   element::N_A,
   element::GRASS,
   200,
   200,
   0
  };

  const monster clefairy = {
   "clefairy",
   &tackle,
   &tailwhip,
   &fly,
   nullptr,
   element::N_A,
   element::NORMAL,
   200,
   200,
   0
  };

  const monster cubone = {
   "cubone",
   &tackle,
   &tailwhip,
   nullptr,
   nullptr,
   element::N_A,
   element::GROUND,
   200,
   200,
   0
  };

  const monster pikachu = {
  "pikachu",
  &tackle,
  &tailwhip,
  &thundershock,
  &roar,
  element::N_A,
  element::ELECTRIC,
  210,
  210,
  0
  };
}