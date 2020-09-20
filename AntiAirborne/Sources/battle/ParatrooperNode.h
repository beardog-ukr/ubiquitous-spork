#pragma once

//#include <utility> // std::pair

#include "cocos2d.h"

#include "SixCatsLoggerLoggable.h"

namespace anti_airborne {

class ParatrooperNode : public cocos2d::Node, virtual public SixCatsLoggerLoggable {
public:
  static ParatrooperNode* create(std::shared_ptr<SixCatsLogger> c6);

  void setDropParameters(const float travelDelay, const cocos2d::Vec2& dropPoint);
  void doDrop(cocos2d::CallFunc* escapeNotifier);

  void doDie();

  bool isAlive()const;

protected:
  ParatrooperNode();
  virtual ~ParatrooperNode();

  bool initSelf();
  bool initBaseSprites();

  void launchRunningAnimation();

  float dropDelay;
  cocos2d::Vec2 dropPoint;

  cocos2d::Sprite* body;
  cocos2d::Sprite* parachute;

  bool alive;
//  cocos2d::Sprite* turret;
//  cocos2d::Sprite* crosshair;

};

}
