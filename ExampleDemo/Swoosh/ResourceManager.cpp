#include "pch.h"
#include "ResourceManager.h"

ResourceManager::ResourceManager()
{
  progress = 0;
}


ResourceManager::~ResourceManager()
{
  delete layerZero;
  delete layerOne;
  delete layerTwo;   // collision
  delete layerThree; // spawn areas

  delete playerTexture;
  delete owPlayerTexture;
  delete wildTexture;
  delete battleAreaTexture;
  delete battlePadFGTexture;
  delete battlePadBGTexture;
  delete textboxTexture;
  delete playerStatusTexture;
  delete enemyStatusTexture;
  delete particleTexture;
  delete fireTexture;
  delete titleTexture;
  delete ddownTexture;
  delete aupTexture;

  delete wildRoarBuffer;
  delete playerRoarBuffer;
  delete statsFallBuffer;
  delete statsRiseBuffer;
  delete selectBuffer;
  delete attackWeakBuffer;
  delete attackNormalBuffer;
  delete attackSuperBuffer;
  delete xpBuffer;
  delete tailWhipBuffer;
  delete flyBuffer;
  delete faintBuffer;
  delete flameBuffer;
  delete buzzerBuffer;
  delete levelupBuffer;

  delete menuFont;
}

void ResourceManager::load() {
  progress = 0;

  map.load(OVERWORLD_TMX_PATH); progress++;
  layerZero = new MapLayer(map, 0); progress++;
  layerOne = new MapLayer(map, 1); progress++;
  layerTwo = new MapLayer(map, 2); progress++;
  layerThree = new MapLayer(map, 3); progress++;

  statsFallBuffer = loadSound(STATS_LOWER_SFX); progress++;
  statsRiseBuffer = loadSound(STATS_RISE_SFX); progress++;
  selectBuffer = loadSound(SELECT_SFX); progress++;
  attackWeakBuffer = loadSound(HIT_WEAK_SFX); progress++;
  attackNormalBuffer = loadSound(HIT_NORMAL_SFX); progress++;
  attackSuperBuffer = loadSound(HIT_SUPER_SFX); progress++;
  xpBuffer = loadSound(XP_SFX); progress++;
  tailWhipBuffer = loadSound(TAIL_WHIP_SFX); progress++;
  flyBuffer = loadSound(FLY_SFX); progress++;
  faintBuffer = loadSound(FAINT_FX); progress++;
  flameBuffer = loadSound(FLAME_FX); progress++;
  buzzerBuffer = loadSound(BUZZER_FX); progress++;
  levelupBuffer = loadSound(LEVEL_UP_FX); progress++;

  owPlayerTexture = loadTexture(PLAYER_OW_PATH); progress++;
  battleAreaTexture = loadTexture(GRASS_AREA); progress++;
  battlePadFGTexture = loadTexture(GRASS_PAD_FG); progress++;
  battlePadBGTexture = loadTexture(GRASS_PAD_BG); progress++;
  textboxTexture = loadTexture(TEXTBOX_PATH); progress++;
  playerStatusTexture = loadTexture(PLAYER_STATUS_PATH); progress++;
  enemyStatusTexture = loadTexture(ENEMY_STATUS_PATH); progress++;
  particleTexture = loadTexture(PARTICLE_PATH); progress++;
  fireTexture = loadTexture(FIREBALL_PATH); progress++;
  titleTexture = loadTexture(TITLE_PATH); progress++;
  ddownTexture = loadTexture(DEFENSE_DOWN_PATH); progress++;
  aupTexture = loadTexture(ATTACK_UP_PATH); progress++;

  menuFont = loadFont(GAME_FONT); progress++;
}
