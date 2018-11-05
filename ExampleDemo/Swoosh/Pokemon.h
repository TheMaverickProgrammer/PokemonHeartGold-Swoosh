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


  const moves tackle       = { "tackle", element::NORMAL };
  const moves thundershock = { "thundershock", element::ELECTRIC };
  const moves fly     = { "fly", element::FLYING };
  const moves agility = { "agility", element::NORMAL };

  const monster pidgey = {
    "pidgey",
    &tackle,
    &agility,
    nullptr,
    nullptr,
    element::N_A,
    element::NORMAL,
    200,
    200,
    0
  };

  const monster pikachu = {
  "pikachu",
  &tackle,
  &agility,
  &thundershock,
  nullptr,
  element::N_A,
  element::ELECTRIC,
  210,
  210,
  0
  };
}