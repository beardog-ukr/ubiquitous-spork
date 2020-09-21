#include "ZOrderConstTypes.h"
#include "ZOrderConstValues.h"

const struct anti_airborne::BattleSceneZOrderValues anti_airborne::kBattleSceneZO = {
  .background = 10,
  .terrain = 11,

  .tank = 20,
  .tankTracks = 21,
  .tankTurret = 21,
  .tankBody = 22,

  .paratrooper = 30,
  .paraBody = 32,
  .parachute = 31,

  .plane = 40,

  .bullet = 50,
  .explosion = 55,

  .crosshair = 60,
  .helpLabel = 61
};

const struct anti_airborne::GameEndSceneZOrderValues anti_airborne::kGameEndSceneZO = {
  .background = 10,
  .terrain = 11,

  .menu = 20,

  .image = 30,
  .label = 31
};
