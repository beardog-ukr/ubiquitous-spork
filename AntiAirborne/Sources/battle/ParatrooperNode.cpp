#include "ParatrooperNode.h"
using namespace anti_airborne;

#include "ZOrderConstTypes.h"
#include "ZOrderConstValues.h"

#include "SixCatsLogger.h"
#include "SixCatsLoggerMacro.h"
#include <sstream>

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
} kElementSpriteFileNames = {
  .body = "anti_aiborne/paratrooper/gunner_jump_00",
  .parachute = "anti_aiborne/paratrooper/para",
};

static const int kJumpAnimationTag = 69;
static const string kJumpAnimationName = "paratrooper_jump";
static const string kRunAnimationName = "paratrooper_run";

static const int kScreenMiddle = 640;
static const int kEscapePointLeft = -30;
static const int kEscapePointRight = 1280 + 30;
static const float kEscapeVelocity = 75 / 1.0;

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

ParatrooperNode::ParatrooperNode() {
  alive = true;
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

void ParatrooperNode::doDie() {
  alive = false;
  parachute->stopAllActions();
  body->stopAllActions();

  body->runAction(FadeOut::create(kFadeinInterval));
  parachute->runAction(FadeOut::create(kFadeinInterval));
}


// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void ParatrooperNode::doDrop(CallFunc *escapeNotifier) {
  C6_D6(c6, "Do drop with delay: ", dropDelay,
        "Drop point ", dropPoint.x, ":", dropPoint.y);

  setPosition(dropPoint);

  // --- para
  Sequence* seqPara = Sequence::create(DelayTime::create(dropDelay),
                                       FadeIn::create(kFadeinInterval),
                                       DelayTime::create(kFallingInterval-kFadeinInterval*2),
                                       FadeOut::create(kFadeinInterval),
                                       nullptr);
  parachute->runAction(seqPara);

  // --- body
  CallFunc* cf = CallFunc::create([this]() {
    this->body->setOpacity(255);
  });

  Sequence* seqBody = Sequence::create(DelayTime::create(dropDelay), cf, nullptr);
  body->runAction(seqBody);

  // --- body animation
  Animation* animationE = AnimationCache::getInstance()->getAnimation(kJumpAnimationName);
  Animate* animateE = Animate::create(animationE);
  Repeat* ra = Repeat::create(animateE, 100);
  ra->setTag(kJumpAnimationTag);
  body->runAction(ra);

  // --- fall + escape
  Vec2 destinationPoint(dropPoint.x, body->getContentSize().height/2);
  MoveTo* fallMT = MoveTo::create(kFallingInterval, destinationPoint);

  MoveTo* escapeMT;
  CallFunc* animationLaunchCF = CallFunc::create([this] {
    this->launchRunningAnimation();
  });;

  if (dropPoint.x < kScreenMiddle) {
    Vec2 escapePoint(kEscapePointLeft, destinationPoint.y);
    float escapeTime = (dropPoint.x - escapePoint.x)/kEscapeVelocity;
    escapeMT = MoveTo::create(escapeTime, escapePoint);
  }
  else {
    Vec2 escapePoint(kEscapePointRight, destinationPoint.y);
    const float escapeTime = (escapePoint.x - dropPoint.x)/kEscapeVelocity;
    escapeMT = MoveTo::create(escapeTime, escapePoint);

  }

  Sequence* seqFallEscape = Sequence::create(DelayTime::create(dropDelay), fallMT,
                                             animationLaunchCF, escapeMT, escapeNotifier, nullptr);
  runAction(seqFallEscape);
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

bool ParatrooperNode::isAlive() const {
  return alive;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void ParatrooperNode::launchRunningAnimation() {
  C6_D1(c6, "here");
  Animation* animationE = AnimationCache::getInstance()->getAnimation(kRunAnimationName);
  Animate* animateE = Animate::create(animationE);

  body->stopAllActionsByTag(kJumpAnimationTag);
  body->runAction(RepeatForever::create(animateE));

  if (getPosition().x < kScreenMiddle) {// mirror soldier if it goes left
    body->setScaleX(-1);
  }
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void ParatrooperNode::setDropParameters(const float inTravelDelay, const Vec2& inDropPoint) {
  dropDelay = inTravelDelay;
  dropPoint = inDropPoint;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

