#pragma once

#include "cocos2d.h"

#include "SixCatsLoggerLoggable.h"

namespace anti_airborne {

class ParatrooperNode;

class PlaneNode : public cocos2d::Sprite, virtual public SixCatsLoggerLoggable {
public:
  static PlaneNode* create(std::shared_ptr<SixCatsLogger> c6);

  std::list<ParatrooperNode*> doOnePass();

protected:
  PlaneNode();
  virtual ~PlaneNode();

  int dropsPerOperation;

  bool initSelf();
};

}
