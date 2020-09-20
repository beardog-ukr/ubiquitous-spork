#include "PlaneNode.h"
using namespace anti_airborne;

#include "battle/ParatrooperNode.h"
#include "ZOrderConstTypes.h"
#include "ZOrderConstValues.h"

#include "SixCatsLogger.h"
#include "SixCatsLoggerMacro.h"
#include <sstream>

#include "AudioEngine.h"
USING_NS_CC;
using namespace std;

static const float kGeneralPath = 200;
static const float kGeneralTime = 1;
static const float kGeneralVelocity = kGeneralPath/kGeneralTime;

static const float kStartingX = -110;
static const float kFinishX = 1280 + 110;
static const int kEchelone = 640;

static const int kLeftZoneStart = 30;
static const int kLeftZoneLength = 530;
static const int kRightZoneStart = 700;
static const int kRightZoneLength = 540;

static const int kAngleManipulationActionTag = 21;
//static const int kDistanceManipulationActionTag = 22;
static const string kEngineSoundFN = "sounds/plane.wav";
static const string kBaseSpriteName = "anti_aiborne/planes/plane";

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

PlaneNode::PlaneNode() {
  dropsPerOperation = 2;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

PlaneNode::~PlaneNode() {
  C6_F1(c6, "here");
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

PlaneNode* PlaneNode::create(shared_ptr<SixCatsLogger> inC6) {

  PlaneNode *pRet = new(std::nothrow) PlaneNode();
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

list<ParatrooperNode*> PlaneNode::doOnePass() {
  list<ParatrooperNode*> result;

  setPosition(kStartingX, kEchelone);

  float flyPath = kFinishX - kStartingX;
  float flyTime = flyPath / kGeneralVelocity;

  MoveTo* mt = MoveTo::create(flyTime, Vec2(kFinishX, kEchelone));
  runAction(mt);

  // --- audio
  int startingSoundId = AudioEngine::play2d(kEngineSoundFN);
  if (startingSoundId == AudioEngine::INVALID_AUDIO_ID) {
    C6_D1(c6, "audio id invalid");
  }

  // --- create troopers
  for(int i = 1; i<= dropsPerOperation; i++) {
    const int rside = RandomHelper::random_int((int)0, 1);
    const float rmod = RandomHelper::random_int((int)0, 99) / 100.0;

    C6_D4(c6, "Random values are ", rside, " and ", rmod);

    float rX;
    if (rside == 0 ) {
      rX = kLeftZoneStart + kLeftZoneLength*rmod;
    }
    else {
      rX = kRightZoneStart + kRightZoneLength*rmod;
    }

    const Vec2 dropPoint(rX, kEchelone);

    const float pathToDrop = dropPoint.x - kStartingX;
    const float timeToDrop = pathToDrop / kGeneralVelocity;

    ParatrooperNode* paratrooper = ParatrooperNode::create(c6);
    paratrooper->setDropParameters(timeToDrop, dropPoint);
    C6_D6(c6, "Dropping trooper at ", dropPoint.x, ":", dropPoint.y, " after ", timeToDrop);

    result.push_back(paratrooper);
  }

  return result;
}


// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool PlaneNode::initSelf() {
  if (!initWithSpriteFrameName(kBaseSpriteName)) {
    C6_C2(c6, "Failed to init with file ", kBaseSpriteName);
    return false;      //
  }

  setPosition(kStartingX, kEchelone);

//  if (!initBaseSprites()) {
//    return false;
//  }

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

