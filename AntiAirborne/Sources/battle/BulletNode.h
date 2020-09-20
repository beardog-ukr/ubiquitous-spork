#pragma once

#include "cocos2d.h"

#include "SixCatsLoggerLoggable.h"

namespace anti_airborne {

//class ParatrooperNode;

class BulletNode : public cocos2d::Sprite, virtual public SixCatsLoggerLoggable {
public:
  static BulletNode* create(std::shared_ptr<SixCatsLogger> c6);

  void setRoute(const cocos2d::Vec2& startPos, const cocos2d::Vec2& destinationPos);
  cocos2d::Vec2 getDestinationPoint() const;

  void reevaluatePosition(const cocos2d::Vec2& basePos);

  void doGo(cocos2d::CallFunc* evaluateDamageCB);

  static void unloadAnimations();
//  std::list<ParatrooperNode*> doOnePass();

protected:
  BulletNode();
  virtual ~BulletNode();

  void doExplode();

  cocos2d::Vec2 destinationPoint;

  bool initSelf();
};

}
