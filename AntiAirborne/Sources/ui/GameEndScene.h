#pragma once

#include "cocos2d.h"
#include "SixCatsLoggerLoggable.h"

namespace anti_airborne {

class GameEndScene : public cocos2d::Scene,  virtual public SixCatsLoggerLoggable {
public:
  static cocos2d::Scene* createScene(const bool gameResult, std::shared_ptr<SixCatsLogger> c6);

protected:
  GameEndScene();
  virtual ~GameEndScene();

  virtual bool init();
  bool initBackground();
  bool initMenu();

  bool initVictoryMessage();
  bool initFailureMessage();

  bool initKeyboardProcessing();

  void onKeyPressedScene(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *);

  static bool loadSpriteCache(std::shared_ptr<SixCatsLogger> c6);
  static void unloadSpriteCache();

  cocos2d::Menu* menu;
  void mcExit(cocos2d::Ref *pSender);
  void mcPlayAgain(cocos2d::Ref *pSender);

  bool gameResult;
};

}
