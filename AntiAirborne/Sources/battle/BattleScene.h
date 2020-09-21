#pragma once

#include "cocos2d.h"
#include "SixCatsLoggerLoggable.h"

namespace anti_airborne {

class ParatrooperNode;
class PlaneNode;
class TankNode;

class BattleScene : public cocos2d::Scene,  virtual public SixCatsLoggerLoggable {
public:
  static cocos2d::Scene* createScene(const bool showHelp, std::shared_ptr<SixCatsLogger> c6);

protected:
  BattleScene();
  virtual ~BattleScene();

  virtual bool init(const bool showHelp);
  bool initBackground();
  bool initTank();
  bool initPlane(const bool immediate);
  bool initHelpMessages();
  bool initKeyboardProcessing();

  void processFireRequest();

  void onKeyPressedScene(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *);
  void onKeyReleasedScene(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *);

  TankNode* tankNode;

  void evaluateDamage(const cocos2d::Vec2 explosionPoint);
  std::list<ParatrooperNode*> paratroopers;// TODO: WARN: this list never clears
  PlaneNode* plane;

  void doDropTroopers(float unused);

  static bool loadSpriteCache(std::shared_ptr<SixCatsLogger> c6);
  static void unloadSpriteCache();

  int escapeCounter;
  void increaseEscapeCounter();

  int planeAttacksCounter;
  void increasePlaneAttacksCounter();
};

}
