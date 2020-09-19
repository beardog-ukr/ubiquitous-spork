#pragma once

#include "cocos2d.h"
#include "SixCatsLoggerLoggable.h"

namespace anti_airborne {

class ParatrooperNode;
class PlaneNode;
class TankNode;

class BattleScene : public cocos2d::Scene,  virtual public SixCatsLoggerLoggable {
public:
  static cocos2d::Scene* createScene(std::shared_ptr<SixCatsLogger> c6);

protected:
  BattleScene();
  virtual ~BattleScene();

  virtual bool init();
  bool initBackground();
  bool initOther();
  bool initTank();
  bool initPlane();
  bool initKeyboardProcessing();

  void processFireRequest();

  void onKeyPressedScene(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *);
  void onKeyReleasedScene(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *);

  TankNode* tankNode;

  std::list<ParatrooperNode*> paratroopers;
  PlaneNode* plane;

  void doDropTroopers(float unused);

  static bool loadSpriteCache(std::shared_ptr<SixCatsLogger> c6);
  static void unloadSpriteCache();
};

}
