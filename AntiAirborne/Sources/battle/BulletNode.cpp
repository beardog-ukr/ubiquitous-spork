#include "BulletNode.h"
using namespace anti_airborne;

//#include "battle/ParatrooperNode.h"
#include "ZOrderConstTypes.h"
#include "ZOrderConstValues.h"

#include "SixCatsLogger.h"
#include "SixCatsLoggerMacro.h"
#include <sstream>

#include "AudioEngine.h"
USING_NS_CC;
using namespace std;

static const float kGeneralPath = 300;
static const float kGeneralTime = 1;
static const float kGeneralVelocity = kGeneralPath/kGeneralTime;

//static con/st float kStartingX = -110;
//static const float kFinishX = 1280 + 110;
//static const int kEchelone = 640;

//static const int kAngleManipulationActionTag = 21;

static const string kExplosionAnimationName = "explosion";
static const string kExplosionStartFrameName = "anti_aiborne/explosion/explosion_00";

static const int kExplosionSoundsMaxId = 2;
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
static const string kExplosionSounds[kExplosionSoundsMaxId] = {
  "sounds\Explosion3.mp3", "sounds\Explosion4.mp3"};
#else
static const string kExplosionSounds[kExplosionSoundsMaxId] = {
  "sounds/Explosion3.wav", "sounds/Explosion4.wav"};
#endif
static const float kExplosionDuration = 1.8;
static const string kBaseSpriteName = "anti_aiborne/tank/tank_bullet3";

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

BulletNode::BulletNode() {
//
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

BulletNode::~BulletNode() {
  C6_F1(c6, "here");
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

BulletNode* BulletNode::create(shared_ptr<SixCatsLogger> inC6) {

  BulletNode *pRet = new(std::nothrow) BulletNode();
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

void BulletNode::doExplode() {
  Sprite* es = Sprite::createWithSpriteFrameName(kExplosionStartFrameName);
  if (es == nullptr) {
    C6_D2(c6, "Failed to load ", kExplosionStartFrameName);
    return;
  }

  addChild(es, kBattleSceneZO.explosion);

  Animation* animationE = AnimationCache::getInstance()->getAnimation(kExplosionAnimationName);
//  C6_D2(c6, "Animation Duration ", animationE->getDuration());
  Animate* animateE = Animate::create(animationE);

  es->runAction(animateE);

  const int esfnid = RandomHelper::random_int((int)0, kExplosionSoundsMaxId-1);
  int startingSoundId = AudioEngine::play2d(kExplosionSounds[esfnid]);
  if (startingSoundId == AudioEngine::INVALID_AUDIO_ID) {
    C6_D1(c6, "audio id invalid");
  }
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void BulletNode::doGo(cocos2d::CallFunc* evaluateDamageCB) {
  const Vec2 currentPos = getPosition();
  const float xDiff = destinationPoint.x - currentPos.x;
  const float yDiff = destinationPoint.y - currentPos.y;
  const float distance = sqrt( xDiff* xDiff + yDiff * yDiff );
  C6_C2(c6, "Bullet fly distance is ", distance);

  const float travelTime = distance / kGeneralVelocity;

  MoveTo* mt = MoveTo::create(travelTime, destinationPoint);

  CallFunc* cf = CallFunc::create([this] {
    this->runAction(FadeOut::create(kExplosionDuration*0.8));
    this->doExplode();
  });

  Sequence* seq = Sequence::create(mt, cf, evaluateDamageCB,
                                   DelayTime::create(kExplosionDuration),
                                   RemoveSelf::create(),  nullptr);
  runAction(seq);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

Vec2 BulletNode::getDestinationPoint() const {
  return destinationPoint;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool BulletNode::initSelf() {
  if (!initWithSpriteFrameName(kBaseSpriteName)) {
    C6_C2(c6, "Failed to init with file ", kBaseSpriteName);
    return false;      //
  }

//  setPosition(kStartingX, kEchelone);

//  if (!initBaseSprites()) {
//    return false;
//  }

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void BulletNode::reevaluatePosition(const cocos2d::Vec2& basePos) {
  const Vec2 currentPos = getPosition();

  Vec2 newPos;
  newPos.x = currentPos.x + basePos.x;
  newPos.y = currentPos.y + basePos.y;

  setPosition(newPos);

  // ---
  destinationPoint.x = destinationPoint.x + basePos.x;
  destinationPoint.y = destinationPoint.y + basePos.y;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void BulletNode::setRoute(const cocos2d::Vec2& startPos, const cocos2d::Vec2& inDestinationPos) {
  destinationPoint = inDestinationPos;
  setPosition(startPos);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void BulletNode::unloadAnimations() {
  AnimationCache * const ac = AnimationCache::getInstance();
  ac->removeAnimation(kExplosionAnimationName);

//  SpriteFrameCache::getInstance()->removeSpriteFramesFromFile(framesPlistFN);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
