#pragma once

#include "cocos2d.h"
#include "SixCatsLoggerLoggable.h"

namespace anti_airborne {

class BattleScene : public cocos2d::Scene,  virtual public SixCatsLoggerLoggable {
public:
  static cocos2d::Scene* createScene(std::shared_ptr<SixCatsLogger> c6);

protected:
  BattleScene();
  virtual ~BattleScene();

  virtual bool init();
  bool initBackground();
  bool initKeyboardProcessing();

  void onKeyPressedScene(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *);
};

}
