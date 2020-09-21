#include "BattleScene.h"

#include "battle/BulletNode.h"
#include "battle/ParatrooperNode.h"
#include "battle/PlaneNode.h"
#include "battle/TankNode.h"
#include "ui/GameEndScene.h"
#include "ZOrderConstTypes.h"
#include "ZOrderConstValues.h"

// Six Cats logger defines
#include "SixCatsLogger.h"
#include "SixCatsLoggerMacro.h"
#include <sstream>

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

static const float kDropsInterval = 9;// interval between drop oprations in seconds

static const float kExplosionKillingRadius = 125;

static const string kPlistFileName = "battle_scene.plist";
static const string kPlistAnimationsFileName = "battle_animations.plist";

static const int kEscapesLimit = 3;
static const int kPlaneAttacksLimit = 12;

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

BattleScene::BattleScene() {
  escapeCounter = 0;
  planeAttacksCounter = 0;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

BattleScene::~BattleScene() {
  unloadSpriteCache();
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

Scene* BattleScene::createScene(const bool showHelp, std::shared_ptr<SixCatsLogger> inC6) {
  BattleScene *pRet = new(std::nothrow) BattleScene();

  do {
    if (pRet == nullptr) {
      break;
    }

    if (!loadSpriteCache(inC6)) {
      break;
    }

    pRet->setLogger(inC6);

    if (pRet->init(showHelp)) {
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

void BattleScene::doDropTroopers(float) {
  increasePlaneAttacksCounter();
  C6_D1(c6, "Starting new drop operaion");

  list<ParatrooperNode*> newTroopers = plane->doOnePass();

  CallFunc* cf = CallFunc::create([this] {
    this->increaseEscapeCounter();
  });

  for(ParatrooperNode* nt: newTroopers) {
    addChild(nt, kBattleSceneZO.paratrooper);
    nt->doDrop(cf);
    paratroopers.push_back(nt);
  }
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void BattleScene::evaluateDamage(const cocos2d::Vec2 explosionPoint) {
  C6_D4(c6, "Need to evaluate damage at ", explosionPoint.x, ":", explosionPoint.y);

  for(ParatrooperNode* paratrooper: paratroopers) {
    if (!paratrooper->isAlive()) {
      continue;
    }

    Vec2 cpp = paratrooper->getPosition();
    const float distance = sqrt( (cpp.x - explosionPoint.x)*(cpp.x - explosionPoint.x) +
                                 (cpp.y - explosionPoint.y)*(cpp.y - explosionPoint.y));
    if (distance < kExplosionKillingRadius) {
      C6_D5(c6, "Paratrooper at ", cpp.x, ":", cpp.y, " dies");
      paratrooper->doDie();
    }
  }
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void BattleScene::increaseEscapeCounter() {
  escapeCounter++;
  C6_D1(c6, "One more escaped");

  if (escapeCounter >= kEscapesLimit) {
    C6_D1(c6, "Ooops, game lost");
    Scene* geScene = GameEndScene::createScene(false, c6);
    if (geScene == nullptr) {
      return;
    }

    Director::getInstance()->replaceScene(geScene);
  }
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void BattleScene::increasePlaneAttacksCounter() {
  planeAttacksCounter++;
  if (planeAttacksCounter >= kPlaneAttacksLimit) {
    Scene* geScene = GameEndScene::createScene(true, c6);
    if (geScene == nullptr) {
      return;
    }

    Director::getInstance()->replaceScene(geScene);
  }
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool BattleScene::init(const bool showHelp) {
  if ( !Scene::init() ) {
    return false;
  }

  if (!initBackground() ) {
    return false;
  }

  if (showHelp) {
    if (!initHelpMessages() ) {
      return false;
    }
  }

  if (!initPlane(!showHelp)) {
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

bool BattleScene::initHelpMessages() {
  const float kFadeInterval = 1.0;

  Sequence* seq;
  Label* label;
  string message;

  label = Label::createWithTTF("Welcome, soldier", "fonts/GravityBold8.ttf", 48);
  label->setTextColor(Color4B(255, 255, 255, 255));
  label->setAnchorPoint(Vec2(0,0.5));
  label->setPosition(50, 600);
  label->setOpacity(0);
  addChild(label, kBattleSceneZO.helpLabel);

  seq = Sequence::create(FadeIn::create(kFadeInterval), DelayTime::create(2.0),
                         FadeOut::create(kFadeInterval), RemoveSelf::create(), nullptr);
  label->runAction(seq);

  message = "Shoot enemies before they reach the ground";
  label = Label::createWithTTF(message, "fonts/ThaleahFat.ttf", 32);
  label->setTextColor(Color4B(255, 255, 255, 255));
  label->setAnchorPoint(Vec2(0,0.5));
  label->setPosition(50, 400);
  label->setOpacity(0);
  addChild(label, kBattleSceneZO.helpLabel);

  seq = Sequence::create(DelayTime::create(3.0),
                         FadeIn::create(kFadeInterval), DelayTime::create(2.0),
                         FadeOut::create(kFadeInterval), RemoveSelf::create(), nullptr);
  label->runAction(seq);

  message = "Use arrows to aim, space to shoot";
  label = Label::createWithTTF(message, "fonts/ThaleahFat.ttf", 32);
  label->setTextColor(Color4B(255, 255, 255, 255));
  label->setAnchorPoint(Vec2(0,0.5));
  label->setPosition(50, 300);
  label->setOpacity(0);
  addChild(label, kBattleSceneZO.helpLabel);

  seq = Sequence::create(DelayTime::create(6.0),
                         FadeIn::create(kFadeInterval), DelayTime::create(2.0),
                         FadeOut::create(kFadeInterval), RemoveSelf::create(), nullptr);
  label->runAction(seq);

  message = "Area of explosion is quite large, use it to kill few birds with one stone";
  label = Label::createWithTTF(message, "fonts/ThaleahFat.ttf", 32);
  label->setTextColor(Color4B(255, 255, 255, 255));
  label->setAnchorPoint(Vec2(0,0.5));
  label->setPosition(50, 200);
  label->setOpacity(0);
  addChild(label, kBattleSceneZO.helpLabel);

  seq = Sequence::create(DelayTime::create(8.0),
                         FadeIn::create(kFadeInterval), DelayTime::create(2.0),
                         FadeOut::create(kFadeInterval), RemoveSelf::create(), nullptr);
  label->runAction(seq);


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

bool BattleScene::initPlane(const bool immediate) {
  plane = PlaneNode::create(c6);
  if (plane == nullptr) {
    return false;
  }

  addChild(plane, kBattleSceneZO.plane);

  schedule(CC_SCHEDULE_SELECTOR(BattleScene::doDropTroopers), kDropsInterval,
           CC_REPEAT_FOREVER, 0);

  if (immediate) {
    doDropTroopers(0);
  }

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

  AnimationCache * const ac = AnimationCache::getInstance();
  ac->addAnimationsWithFile(kPlistAnimationsFileName);

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void BattleScene::onKeyPressedScene(EventKeyboard::KeyCode keyCode, Event *) {
  C6_T3(c6, "Key '", (int)keyCode, "' was pressed");

  if (EventKeyboard::KeyCode::KEY_BACKSPACE == keyCode) {
    C6_T1(c6, "That was KEY_BACKSPACE, it does nothing");
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
  else if (EventKeyboard::KeyCode::KEY_SPACE == keyCode) {
    processFireRequest();
  }
  else if (EventKeyboard::KeyCode::KEY_P == keyCode) {
    doDropTroopers(0);
  }
  else if (EventKeyboard::KeyCode::KEY_X == keyCode) {
    C6_D1(c6, "Need to get out (debug, x pressed).");

    // Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();
  }
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void BattleScene::onKeyReleasedScene(EventKeyboard::KeyCode keyCode, Event *) {
  C6_T3(c6, "Key '", (int)keyCode, "' was released");

  if (EventKeyboard::KeyCode::KEY_BACKSPACE == keyCode) {
    C6_T1(c6, "That was KEY_BACKSPACE, it does nothing");
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

void BattleScene::processFireRequest() {
  BulletNode* bullet = tankNode->doFire();
  if (bullet == nullptr) {
    C6_W1(c6, "Bullet wasn't created");
    return;
  }

  bullet->reevaluatePosition(tankNode->getPosition());
  addChild(bullet, kBattleSceneZO.bullet);

  const Vec2 explosionPosition = bullet->getDestinationPoint();
  CallFunc* edcf = CallFunc::create([this, explosionPosition] {
    this->evaluateDamage(explosionPosition);
  });

  bullet->doGo(edcf);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void BattleScene::unloadSpriteCache() {
  BulletNode::unloadAnimations();
  SpriteFrameCache::getInstance()->removeSpriteFramesFromFile(kPlistFileName);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
