#pragma once

namespace anti_airborne {

struct BattleSceneZOrderValues {
  int background;
  int terrain;

  int tank;
  int tankTracks;
  int tankTurret;
  int tankBody;

  int paratrooper;
  int paraBody;
  int parachute;

  int plane;

  int bullet;
  int explosion;

  int crosshair;
  int helpLabel;
};

struct GameEndSceneZOrderValues {
  int background;
  int terrain;

  int menu;

  int image;
  int label;
};

}
