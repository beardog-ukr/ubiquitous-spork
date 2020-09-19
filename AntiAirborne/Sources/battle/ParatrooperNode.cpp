#include "ParatrooperNode.h"
using namespace anti_airborne;

#include "ZOrderConstTypes.h"
#include "ZOrderConstValues.h"

#include "SixCatsLogger.h"
#include "SixCatsLoggerMacro.h"
#include <sstream>

//#include <tgmath.h>  // sin cos
USING_NS_CC;
using namespace std;

//static const int kAngleManipulationActionTag = 21;
//static const int kDistanceManipulationActionTag = 22;
//static const float kAngleChangeInterval = 0.5;
static const float kFadeinInterval = 3.0;

static const float kFallingInterval = 20.0;

static const struct {
  string body;
  string parachute;
//  string tracks;
//  string turret;
} kElementSpriteFileNames = {
  .body = "anti_aiborne/paratrooper/gunner_jump_00",
  .parachute = "anti_aiborne/paratrooper/para",
//  .tracks = "anti_aiborne/tank/tracks",
//  .turret = "anti_aiborne/tank/turret1",
};

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

ParatrooperNode::ParatrooperNode() {
//  angle = 45;
//  distance = 200;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

ParatrooperNode::~ParatrooperNode() {
  C6_F1(c6, "here");
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

ParatrooperNode* ParatrooperNode::create(shared_ptr<SixCatsLogger> inC6) {

  ParatrooperNode *pRet = new(std::nothrow) ParatrooperNode();
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

void ParatrooperNode::doDrop() {
  C6_D6(c6, "Do drop with delay: ", dropDelay,
        "Drop point ", dropPoint.x, ":", dropPoint.y);

  setPosition(dropPoint);

  // --- para
  Sequence* seqPara = Sequence::create(DelayTime::create(dropDelay),
                                       FadeIn::create(kFadeinInterval),
                                       DelayTime::create(kFallingInterval-kFadeinInterval*2),
                                       FadeOut::create(kFadeinInterval),
                                       RemoveSelf::create(),
                                       nullptr);
  parachute->runAction(seqPara);

  // --- body
  CallFunc* cf = CallFunc::create([this]() {
    this->body->setOpacity(255);
  });

  Sequence* seqBody = Sequence::create(DelayTime::create(dropDelay), cf, nullptr);
  body->runAction(seqBody);

  // --- fall
  Vec2 destinationPoint(dropPoint.x, body->getContentSize().height/2);
  MoveTo* moveTo = MoveTo::create(kFallingInterval, destinationPoint);
  Sequence* seqFall = Sequence::create(DelayTime::create(dropDelay), moveTo, nullptr);
  runAction(seqFall);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool ParatrooperNode::initBaseSprites() {
  body = Sprite::createWithSpriteFrameName(kElementSpriteFileNames.body);
  if (body == nullptr) {
    C6_W2(c6, "Failed to open: ", kElementSpriteFileNames.body);
    return false;
  }

  body->setOpacity(0);
  addChild(body, kBattleSceneZO.paraBody);


  // ---
  parachute = Sprite::createWithSpriteFrameName(kElementSpriteFileNames.parachute);
  if (parachute == nullptr) {
    C6_W2(c6, "Failed to open: ", kElementSpriteFileNames.parachute);
    return false;
  }

  parachute->setOpacity(0);
  parachute->setPosition(Vec2(0, 20));
  addChild(parachute, kBattleSceneZO.parachute);

  return true;
}


// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool ParatrooperNode::initSelf() {
  if (!initBaseSprites()) {
    return false;
  }

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void ParatrooperNode::setDropParameters(const float inTravelDelay, const Vec2& inDropPoint) {
  dropDelay = inTravelDelay;
  dropPoint = inDropPoint;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

