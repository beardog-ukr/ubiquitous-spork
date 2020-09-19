#include "BattleScene.h"


#include "battle/TankNode.h"
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

static const struct {
  string plane;
  string paratrooper;
  string tank;
} kElementsSpriteFileNames = {
  .plane = "plane.png",
  .paratrooper = "paratrooper.png",
  .tank = "tank.png"
};

static const string kPlistFileName = "battle_scene.plist";

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

BattleScene::BattleScene() {
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

BattleScene::~BattleScene() {
  unloadSpriteCache();
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

Scene* BattleScene::createScene(std::shared_ptr<SixCatsLogger> inC6) {
  BattleScene *pRet = new(std::nothrow) BattleScene();

  do {
    if (pRet == nullptr) {
      break;
    }

    if (!loadSpriteCache(inC6)) {
      break;
    }

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

  if (!initOther() ) {
    return false;
  }

  if (!initTank()) {
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
  sceneKeyboardListener->onKeyReleased = CC_CALLBACK_2(BattleScene::onKeyReleasedScene, this);
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

bool BattleScene::initOther() {
  Sprite* planeSprite = Sprite::create(kElementsSpriteFileNames.plane);
  if (planeSprite == nullptr) {
    C6_C2(c6, "Failed to find ", kElementsSpriteFileNames.plane);
    return false;
  }

  const Size cs = getContentSize();
  planeSprite->setPosition(Vec2(cs.width/2, cs.height/2 + cs.height/4));
  addChild(planeSprite, kBattleSceneZO.terrain);

  Sprite* paratrooperSprite = Sprite::create(kElementsSpriteFileNames.paratrooper);
  if (paratrooperSprite == nullptr) {
    C6_C2(c6, "Failed to find ", kElementsSpriteFileNames.paratrooper);
    return false;
  }


  paratrooperSprite->setPosition(Vec2(cs.width/2, cs.height/2));
  addChild(paratrooperSprite, kBattleSceneZO.terrain);

//  Sprite* tankSprite = Sprite::create(kElementsSpriteFileNames.tank);
//  if (tankSprite == nullptr) {
//    C6_C2(c6, "Failed to find ", kElementsSpriteFileNames.tank);
//    return false;
//  }


//  tankSprite->setPosition(Vec2(cs.width/2, cs.height/8));
//  addChild(tankSprite, kBattleSceneZO.terrain);

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool BattleScene::initTank() {
  tankNode = TankNode::create(c6);
  if (tankNode == nullptr) {
    return false;
  }

  const Size cs = getContentSize();
  tankNode->setPosition(Vec2(cs.width/2, 50));
  addChild(tankNode, kBattleSceneZO.tank);


  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool BattleScene::loadSpriteCache(std::shared_ptr<SixCatsLogger> c6) {
  SpriteFrameCache* const sfc = SpriteFrameCache::getInstance();

  sfc->addSpriteFramesWithFile(kPlistFileName);
  if (!sfc->isSpriteFramesWithFileLoaded(kPlistFileName)) {
    C6_C2(c6, "Failed to find ", kPlistFileName);
    return false;
  }

  // AnimationCache * const ac = AnimationCache::getInstance();
  // ac->addAnimationsWithFile(kAnimationsPlistFileName);

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void BattleScene::onKeyPressedScene(EventKeyboard::KeyCode keyCode, Event *) {
  C6_D3(c6, "Key '", (int)keyCode, "' was pressed");

  if (EventKeyboard::KeyCode::KEY_BACKSPACE == keyCode) {
    C6_D1(c6, "That was KEY_BACKSPACE, it stops game");
  }
  else if (EventKeyboard::KeyCode::KEY_LEFT_ARROW == keyCode) {
    tankNode->startIncreasingAngle();
  }
  else if (EventKeyboard::KeyCode::KEY_RIGHT_ARROW == keyCode) {
    tankNode->startDecreasingAngle();
  }
  else if (EventKeyboard::KeyCode::KEY_UP_ARROW == keyCode) {
    tankNode->startIncreasingDistance();
  }
  else if (EventKeyboard::KeyCode::KEY_DOWN_ARROW == keyCode) {
    tankNode->startDecreasingDistance();
  }
  else if (EventKeyboard::KeyCode::KEY_X == keyCode) {
    C6_D1(c6, "Need to get out (debug, x pressed).");

    // Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();
  }
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void BattleScene::onKeyReleasedScene(EventKeyboard::KeyCode keyCode, Event *) {
  C6_D3(c6, "Key '", (int)keyCode, "' was released");

  if (EventKeyboard::KeyCode::KEY_BACKSPACE == keyCode) {
    C6_D1(c6, "That was KEY_BACKSPACE, it stops game");
  }
  else if (EventKeyboard::KeyCode::KEY_LEFT_ARROW == keyCode) {
    tankNode->stopIncreasingAngle();
  }
  else if (EventKeyboard::KeyCode::KEY_RIGHT_ARROW == keyCode) {
    tankNode->stopDecreasingAngle();
  }
  else if (EventKeyboard::KeyCode::KEY_UP_ARROW == keyCode) {
    tankNode->stopIncreasingDistance();
  }
  else if (EventKeyboard::KeyCode::KEY_DOWN_ARROW == keyCode) {
    tankNode->stopDecreasingDistance();
  }
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void BattleScene::unloadSpriteCache() {
  // ActorNode::unloadAnimations();
  SpriteFrameCache::getInstance()->removeSpriteFramesFromFile(kPlistFileName);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
