#pragma once

//#include <utility> // std::pair

#include "cocos2d.h"

#include "SixCatsLoggerLoggable.h"

namespace anti_airborne {

class TankNode : public cocos2d::Node, virtual public SixCatsLoggerLoggable {
public:
  static TankNode* create(std::shared_ptr<SixCatsLogger> c6);

  void startIncreasingAngle();
  void stopIncreasingAngle();
  void startDecreasingAngle();
  void stopDecreasingAngle();

  void startIncreasingDistance();
  void stopIncreasingDistance();
  void startDecreasingDistance();
  void stopDecreasingDistance();

protected:
  TankNode();
  virtual ~TankNode();

  bool initSelf();
  bool initBodySprites();

  cocos2d::Vec2 calculateAnglePos(const float angle, const float distance) const;

  float angle;
  void changeAngle(const bool doIncrease);

  float distance;
  void changeDistance(const bool doIncrease);

  cocos2d::Sprite* body;
  cocos2d::Sprite* tracks;
  cocos2d::Sprite* turret;
  cocos2d::Sprite* crosshair;

};

}
