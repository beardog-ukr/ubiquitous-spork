#include "BattleScene.h"

#include "ZOrderConstTypes.h"
#include "ZOrderConstValues.h"

// Six Cats logger defines
#include "SixCatsLogger.h"
#include "SixCatsLoggerMacro.h"
#include <sstream>

// #include "AudioEngine.h"

using namespace anti_airborne;
using namespace std;
USING_NS_CC;

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

static const struct {
  string main;
  string terrain;
} kBackgroundSpriteFileNames = {
  .main = "background.png",
  .terrain = "mountain.png"
};

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

BattleScene::BattleScene() {
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

BattleScene::~BattleScene() {
  // unloadSpriteCache();
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

Scene* BattleScene::createScene(std::shared_ptr<SixCatsLogger> inC6) {
  BattleScene *pRet = new(std::nothrow) BattleScene();

  do {
    if (pRet == nullptr) {
      break;
    }

    // if (!loadSpriteCache(inC6)) {
    // break;
    // }

    pRet->setLogger(inC6);

    if (pRet->init()) {
      pRet->autorelease();
      return pRet; //successful exit
    }

  }while (false);

  // else {
  delete pRet;
  pRet = nullptr;
  return nullptr;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool BattleScene::init() {
  if ( !Scene::init() ) {
    return false;
  }

  if (!initBackground() ) {
    return false;
  }

  if (!initKeyboardProcessing()) {
    return false;
  }

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool BattleScene::initKeyboardProcessing() {
  // keyboard processing
  EventListenerKeyboard* sceneKeyboardListener = EventListenerKeyboard::create();
  sceneKeyboardListener->onKeyPressed = CC_CALLBACK_2(BattleScene::onKeyPressedScene, this);
  _eventDispatcher->addEventListenerWithSceneGraphPriority(sceneKeyboardListener, this);

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool BattleScene::initBackground() {

  Sprite* bgs = Sprite::create(kBackgroundSpriteFileNames.main);
  if (bgs == nullptr) {
    C6_C2(c6, "Failed to find ", kBackgroundSpriteFileNames.main);
    return false;
  }

  const Size cs = getContentSize();
  bgs->setPosition(Vec2(cs.width/2, cs.height/2));
  addChild(bgs, kBattleSceneZO.background);

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

// bool BattleScene::loadSpriteCache(std::shared_ptr<SixCatsLogger> c6) {
//   SpriteFrameCache* const sfc = SpriteFrameCache::getInstance();

//   sfc->addSpriteFramesWithFile(kPlistFileName);
//   if (!sfc->isSpriteFramesWithFileLoaded(kPlistFileName)) {
//     C6_C2(c6, "Failed to find ", kPlistFileName);
//     return false;
//   }

//   // AnimationCache * const ac = AnimationCache::getInstance();
//   // ac->addAnimationsWithFile(kAnimationsPlistFileName);

//   return true;
// }

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void BattleScene::onKeyPressedScene(EventKeyboard::KeyCode keyCode, Event *) {
  // C6_D3(c6, "Key '", (int)keyCode, "' was pressed");

  if (EventKeyboard::KeyCode::KEY_BACKSPACE == keyCode) {
    // C6_D1(c6, "That was KEY_BACKSPACE, it stops game");
  }
  else if (EventKeyboard::KeyCode::KEY_LEFT_ARROW == keyCode) {
  }
  else if (EventKeyboard::KeyCode::KEY_RIGHT_ARROW == keyCode) {
  }
  else if (EventKeyboard::KeyCode::KEY_X == keyCode) {
    // c6->d(__c6_MN__, "Need to get out (debug, x pressed).");

    // Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();
  }
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

// void BattleScene::unloadSpriteCache() {
//   // ActorNode::unloadAnimations();
//   SpriteFrameCache::getInstance()->removeSpriteFramesFromFile(kPlistFileName);
// }

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
