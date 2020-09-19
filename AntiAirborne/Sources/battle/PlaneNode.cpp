#include "PlaneNode.h"
using namespace anti_airborne;

#include "battle/ParatrooperNode.h"
#include "ZOrderConstTypes.h"
#include "ZOrderConstValues.h"

#include "SixCatsLogger.h"
#include "SixCatsLoggerMacro.h"
#include <sstream>

//#include <tgmath.h>  // sin cos
USING_NS_CC;
using namespace std;

static const float kGeneralPath = 200;
static const float kGeneralTime = 1;
static const float kGeneralVelocity = kGeneralPath/kGeneralTime;

static const float kStartingX = -110;
static const float kFinishX = 1280 + 110;
static const int kEchelone = 640;

static const int kAngleManipulationActionTag = 21;
//static const int kDistanceManipulationActionTag = 22;
//static const float kAngleChangeInterval = 0.5;
static const string kBaseSpriteName = "anti_aiborne/planes/plane";

//static const struct {
//  string body;
//  string parachute;
////  string tracks;
////  string turret;
//} kElementSpriteFileNames = {
//  .body = "anti_aiborne/palne/gunner_jump_00",
//  .parachute = "anti_aiborne/paratrooper/para",
////  .tracks = "anti_aiborne/tank/tracks",
////  .turret = "anti_aiborne/tank/turret1",
//};

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

PlaneNode::PlaneNode() {
//  angle = 45;
//  distance = 200;
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

  Vec2 dropPoint;
  dropPoint.x = 400;
  dropPoint.y = kEchelone;

  float pathToDrop = dropPoint.x - kStartingX;
  float timeToDrop = pathToDrop / kGeneralVelocity;

  MoveTo* moveToDrop = MoveTo::create(timeToDrop, dropPoint);
  //TODO: use acceleration/deceleration here

  float pathFromDrop = kFinishX - dropPoint.x;
  float timeFromDrop = pathFromDrop / kGeneralVelocity;

  MoveTo* moveFromDrop = MoveTo::create(timeFromDrop, Vec2(kFinishX, kEchelone));

  Sequence* seq = Sequence::create(moveToDrop, moveFromDrop, nullptr);
  runAction(seq);

  ParatrooperNode* paratrooper = ParatrooperNode::create(c6);
//  paratrooper->setPosition(dropPoint);
  paratrooper->setDropParameters(timeToDrop, dropPoint);

  result.push_back(paratrooper);



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

