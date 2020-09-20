#include "TankNode.h"
using namespace anti_airborne;

#include "BulletNode.h"
#include "ZOrderConstTypes.h"
#include "ZOrderConstValues.h"

#include "SixCatsLogger.h"
#include "SixCatsLoggerMacro.h"
#include <sstream>

#include <tgmath.h>  // sin cos
USING_NS_CC;
using namespace std;

static const int kAngleManipulationActionTag = 21;
static const int kDistanceManipulationActionTag = 22;
static const float kAngleChangeInterval = 0.5;
static const float kDistanceChangeInterval = 0.5;

static const int kAngleStep = 10;
static const int kDistanceStep = 40;

static const struct {
  string body;
  string crosshair;
  string tracks;
  string turret;
} kElementSpriteFileNames = {
  .body = "anti_aiborne/tank/body",
  .crosshair = "anti_aiborne/tank/crosshair",
  .tracks = "anti_aiborne/tank/tracks",
  .turret = "anti_aiborne/tank/turret1",
};

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

TankNode::TankNode() {
  angle = 100;
  distance = 200;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

TankNode::~TankNode() {
  C6_F1(c6, "here");
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

cocos2d::Vec2 TankNode::calculateAnglePos(const float angle, const float distance) const {
  const float pi = 3.14159265359;

  float xMod = 1;
  float angleDegrees = angle;
  if (angle > 90) {
    angleDegrees = 180 - angle;
    xMod = -1;
  }

  C6_T2(c6, "Angle degrees is: ", angleDegrees);

  const float angleRad = (pi*angleDegrees) / 180;
  C6_T2(c6, "Sinus angle rad is: ",sin(angleRad));

  const float a = distance * sin(angleRad);
  const float b = xMod * distance * cos(angleRad);

  return Vec2(b, a);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void TankNode::changeAngle(const bool doIncrease) {
  bool valueChanged = false;
  if ((angle < 180)  && (doIncrease)) {
    angle += kAngleStep;
    valueChanged = true;
  }

  if ((angle > 0)  && (!doIncrease)) {
    angle -= kAngleStep;
    valueChanged = true;
  }

  if (!valueChanged) {
    return;
  }

  crosshair->stopAllActions();

  Vec2 newCrosshairPoint = calculateAnglePos(angle, distance);
  MoveTo* mt = MoveTo::create(kDistanceChangeInterval, newCrosshairPoint);
  crosshair->runAction(mt);

  // --- turret
  Vec2 newTurretPoint = calculateAnglePos(angle, 40);
  turret->setPosition(newTurretPoint);
  turret->setRotation(-1*angle);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void TankNode::changeDistance(const bool doIncrease) {
  bool valueChanged = false;
  if ((distance < 550)  && (doIncrease)) {
    distance += kDistanceStep;
    valueChanged = true;
  }

  if ((distance > 0)  && (!doIncrease)) {
    distance = distance - kDistanceStep;
    valueChanged = true;
  }

  if (!valueChanged) {
    return;
  }

  crosshair->stopAllActions();

  Vec2 newCrosshairPoint = calculateAnglePos(angle, distance);
  MoveTo* mt = MoveTo::create(kDistanceChangeInterval, newCrosshairPoint);
  crosshair->runAction(mt);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

TankNode* TankNode::create(shared_ptr<SixCatsLogger> inC6) {

  TankNode *pRet = new(std::nothrow) TankNode();
  if (pRet ==nullptr) {
    return nullptr;
  }

  pRet->setLogger(inC6);

  if (!pRet->initSelf()) {
    delete pRet;
    pRet = nullptr;
    return nullptr;
  }

  pRet->autorelease();
  return pRet;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

BulletNode* TankNode::doFire() {
  BulletNode* bullet = BulletNode::create(c6);
  if(bullet == nullptr) {
    return bullet;
  }

  Vec2 bulletPoint = calculateAnglePos(angle, 60);
//  bullet->setPosition(bulletPoint);
  bullet->setRotation(-1*angle);
  bullet->setRoute(bulletPoint, crosshair->getPosition());

  return bullet;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool TankNode::initBodySprites() {
  body = Sprite::createWithSpriteFrameName(kElementSpriteFileNames.body);
  if (body == nullptr) {
    C6_W2(c6, "Failed to open: ", kElementSpriteFileNames.body);
    return false;
  }

  addChild(body, kBattleSceneZO.tankBody);


  tracks = Sprite::createWithSpriteFrameName(kElementSpriteFileNames.tracks);
  if (tracks == nullptr) {
    C6_W2(c6, "Failed to open: ", kElementSpriteFileNames.tracks);
    return false;
  }

  tracks->setPosition(Vec2(0, -30));
  addChild(tracks, kBattleSceneZO.tankTracks);

  // ---
  crosshair = Sprite::createWithSpriteFrameName(kElementSpriteFileNames.crosshair);
  if (crosshair == nullptr) {
    C6_W2(c6, "Failed to open: ", kElementSpriteFileNames.crosshair);
    return false;
  }

  crosshair->setPosition(calculateAnglePos(angle, distance));
  addChild(crosshair, kBattleSceneZO.crosshair);

  // ---
  turret = Sprite::createWithSpriteFrameName(kElementSpriteFileNames.turret);
  if (turret == nullptr) {
    C6_W2(c6, "Failed to open: ", kElementSpriteFileNames.turret);
    return false;
  }

  Vec2 newTurretPoint = calculateAnglePos(angle, 40);
  turret->setPosition(newTurretPoint);
  turret->setRotation(-1*angle);
  addChild(turret, kBattleSceneZO.tankTurret);

  return true;
}


// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool TankNode::initSelf() {
  if (!initBodySprites()) {
    return false;
  }

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void TankNode::startDecreasingAngle() {
  C6_D1(c6, "Here");
  stopAllActionsByTag(kAngleManipulationActionTag);
  crosshair->stopAllActions();

  // --- one first action
  changeAngle(false);

  // --- start other actions
  CallFunc* cf = CallFunc::create([this]() {
    this->changeAngle(false);
  });

  Sequence* seq = Sequence::create(DelayTime::create(kAngleChangeInterval), cf, nullptr);

  RepeatForever* repeatForever = RepeatForever::create(seq);
  repeatForever->setTag(kAngleManipulationActionTag);

  runAction(repeatForever);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void TankNode::startDecreasingDistance() {
  C6_D1(c6, "Here");
  stopAllActionsByTag(kDistanceManipulationActionTag);
//  crosshair->stopAllActions();

  // --- one first action
  changeDistance(false);

  // --- start other actions
  CallFunc* cf = CallFunc::create([this]() {
    this->changeDistance(false);
  });

  Sequence* seq = Sequence::create(DelayTime::create(kDistanceChangeInterval), cf, nullptr);

  RepeatForever* repeatForever = RepeatForever::create(seq);
  repeatForever->setTag(kDistanceManipulationActionTag);

  runAction(repeatForever);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void TankNode::startIncreasingAngle() {
  C6_D1(c6, "Here");
  stopAllActionsByTag(kAngleManipulationActionTag);
  crosshair->stopAllActions();

  // --- one first action
  changeAngle(true);

  // --- start other actions
  CallFunc* cf = CallFunc::create([this]() {
    this->changeAngle(true);
  });

  Sequence* seq = Sequence::create(DelayTime::create(kAngleChangeInterval), cf, nullptr);

  RepeatForever* repeatForever = RepeatForever::create(seq);
  repeatForever->setTag(kAngleManipulationActionTag);

  runAction(repeatForever);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void TankNode::startIncreasingDistance() {
  C6_D1(c6, "Here");
  stopAllActionsByTag(kDistanceManipulationActionTag);
//  crosshair->stopAllActions();

  // --- one first action
  changeDistance(true);

  // --- start other actions
  CallFunc* cf = CallFunc::create([this]() {
    this->changeDistance(true);
  });

  Sequence* seq = Sequence::create(DelayTime::create(kDistanceChangeInterval), cf, nullptr);

  RepeatForever* repeatForever = RepeatForever::create(seq);
  repeatForever->setTag(kDistanceManipulationActionTag);

  runAction(repeatForever);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void TankNode::stopDecreasingAngle() {
  C6_D1(c6, "Here");
  stopAllActionsByTag(kAngleManipulationActionTag);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void TankNode::stopIncreasingAngle() {
  C6_D1(c6, "Here");
  stopAllActionsByTag(kAngleManipulationActionTag);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void TankNode::stopDecreasingDistance() {
  C6_D1(c6, "Here");
  stopAllActionsByTag(kDistanceManipulationActionTag);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void TankNode::stopIncreasingDistance() {
  C6_D1(c6, "Here");
  stopAllActionsByTag(kDistanceManipulationActionTag);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

