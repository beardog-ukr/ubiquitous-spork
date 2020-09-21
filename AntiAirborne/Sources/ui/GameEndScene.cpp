#include "GameEndScene.h"

#include "battle/BattleScene.h"
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
  string failure;
  string victory;
} kMessageSpriteFileNames = {
  .failure = "ui/scull.png",
  .victory = "ui/victory-wreath.png"
};

static const string kPlistFileName = "ui/menu.plist";

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

GameEndScene::GameEndScene() {
  gameResult = true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

GameEndScene::~GameEndScene() {
  unloadSpriteCache();
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

Scene* GameEndScene::createScene(const bool inGameResult, std::shared_ptr<SixCatsLogger> inC6) {
  GameEndScene *pRet = new(std::nothrow) GameEndScene();

  do {
    if (pRet == nullptr) {
      break;
    }

    if (!loadSpriteCache(inC6)) {
      break;
    }

    pRet->gameResult = inGameResult;
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

bool GameEndScene::init() {
  if ( !Scene::init() ) {
    return false;
  }

  if (!initBackground() ) {
    return false;
  }

  if (!initMenu()) {
    return false;
  }

  if (gameResult) {
    if (!initVictoryMessage()) {
      return false;
    }
  }
  else {
    if (!initFailureMessage()) {
      return false;
    }
  }

  if (!initKeyboardProcessing()) {
    return false;
  }

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool GameEndScene::initBackground() {
  Sprite* bgs = Sprite::create(kBackgroundSpriteFileNames.main);
  if (bgs == nullptr) {
    C6_C2(c6, "Failed to find ", kBackgroundSpriteFileNames.main);
    return false;
  }

  const Size cs = getContentSize();
  bgs->setPosition(Vec2(cs.width/2, cs.height/2));
  addChild(bgs, kGameEndSceneZO.background);

  return true;
}


// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool GameEndScene::initFailureMessage() {
  const Size cs = getContentSize();

  // --- image
  Sprite* image = Sprite::create(kMessageSpriteFileNames.failure);
  if (image == nullptr) {
    return false;
  }
  image->setPosition(230, 500);
  addChild(image, kGameEndSceneZO.image);

  image = Sprite::create(kMessageSpriteFileNames.failure);
  image->setPosition(cs.width - 230, 500);
  addChild(image, kGameEndSceneZO.image);

  // --- main label
  Label* titleLabel = Label::createWithTTF("Failure", "fonts/GravityBold8.ttf", 48);
  titleLabel->setTextColor(Color4B(  138, 3, 3, 255));
  titleLabel->setAnchorPoint(Vec2(0.5,0.5));
  titleLabel->setPosition(cs.width/2, 660);
  addChild(titleLabel, kGameEndSceneZO.label);

  // --- label
  const string message1 = "Too many invaders escaped";
  Label* label = Label::createWithTTF(message1, "fonts/ThaleahFat.ttf", 48);
  label->setTextColor(Color4B(255, 255, 255, 255));
  label->setAnchorPoint(Vec2(0.5,0.5));
  label->setPosition(cs.width/2, 500);
  addChild(label, kGameEndSceneZO.label);

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool GameEndScene::initKeyboardProcessing() {
  // keyboard processing
  EventListenerKeyboard* sceneKeyboardListener = EventListenerKeyboard::create();
  sceneKeyboardListener->onKeyPressed = CC_CALLBACK_2(GameEndScene::onKeyPressedScene, this);
//  sceneKeyboardListener->onKeyReleased = CC_CALLBACK_2(GameEndScene::onKeyReleasedScene, this);
  _eventDispatcher->addEventListenerWithSceneGraphPriority(sceneKeyboardListener, this);

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool GameEndScene::initMenu() {
  SpriteFrameCache* const sfc = SpriteFrameCache::getInstance();
  menu = Menu::create();

  const Size currentWindowSize = getContentSize();

  const int itemsCount = 2;
  string captions[itemsCount] = {"Play again", "Exit"};
  ccMenuCallback mcbs[itemsCount] = {CC_CALLBACK_1(GameEndScene::mcPlayAgain, this),
                                     CC_CALLBACK_1(GameEndScene::mcExit, this) };
  float mcbp[itemsCount] = {currentWindowSize.width/4,
                            currentWindowSize.width/2 + currentWindowSize.width/4};


  for (int i = 0; i< itemsCount; i++) {
    MenuItemImage* item = MenuItemImage::create();
    item->setNormalSpriteFrame(sfc->getSpriteFrameByName("common_ui/panel_main.png"));
    item->setSelectedSpriteFrame(sfc->getSpriteFrameByName("common_ui/panel_sec.png"));
    item->setCallback(mcbs[i]);

    const Size itemSize = item->getContentSize();
    C6_D4(c6, "One item size is ", itemSize.width, "x", itemSize.height);
    Vec2 ip(mcbp[i], 100);

    item->setAnchorPoint(Vec2(0.5,0.5));
    item->setPosition(ip);

    Label* label = Label::createWithTTF(captions[i], "fonts/GravityBold8.ttf", 32);
    label->setTextColor(Color4B(10, 151, 238, 255));//200,60,45,255));
    label->setAnchorPoint(Vec2(0.5,0.5));
    label->setPosition(itemSize.width/2, itemSize.height/2);
    item->addChild(label);

    menu->addChild(item);
  }

  menu->setPosition(Vec2::ZERO);
  addChild(menu, kGameEndSceneZO.menu);

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool GameEndScene::initVictoryMessage() {
  // --- image
  Sprite* victoryImage = Sprite::create(kMessageSpriteFileNames.victory);
  if (victoryImage == nullptr) {
    return false;
  }

  const Size cs = getContentSize();
  victoryImage->setPosition(cs.width/2, 430);

  addChild(victoryImage, kGameEndSceneZO.image);

  // --- main label
  Label* titleLabel = Label::createWithTTF("Victory", "fonts/GravityBold8.ttf", 48);
  titleLabel->setTextColor(Color4B(  138, 3, 3, 255));
  titleLabel->setAnchorPoint(Vec2(0.5,0.5));
  titleLabel->setPosition(cs.width/2, 660);
  addChild(titleLabel, kGameEndSceneZO.label);

  // --- label
  const string message1 = "You survived";
  const string message2 = "all the invader waves";
  Label* label = Label::createWithTTF(message1, "fonts/ThaleahFat.ttf", 48);
  label->setTextColor(Color4B(255, 255, 255, 255));
  label->setAnchorPoint(Vec2(0.5,0.5));
  label->setPosition(cs.width/2, 500);
  addChild(label, kGameEndSceneZO.label);

  label = Label::createWithTTF(message2, "fonts/ThaleahFat.ttf", 48);
  label->setTextColor(Color4B(255, 255, 255, 255));
  label->setAnchorPoint(Vec2(0.5,0.5));
  label->setPosition(cs.width/2, 450);
  addChild(label, kGameEndSceneZO.label);

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool GameEndScene::loadSpriteCache(std::shared_ptr<SixCatsLogger> c6) {
  SpriteFrameCache* const sfc = SpriteFrameCache::getInstance();

  sfc->addSpriteFramesWithFile(kPlistFileName);
  if (!sfc->isSpriteFramesWithFileLoaded(kPlistFileName)) {
    C6_C2(c6, "Failed to find ", kPlistFileName);
    return false;
  }

//  AnimationCache * const ac = AnimationCache::getInstance();
//  ac->addAnimationsWithFile(kPlistAnimationsFileName);

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void GameEndScene::onKeyPressedScene(EventKeyboard::KeyCode keyCode, Event *) {
  C6_T3(c6, "Key '", (int)keyCode, "' was pressed");

  if (EventKeyboard::KeyCode::KEY_BACKSPACE == keyCode) {
    C6_T1(c6, "That was KEY_BACKSPACE, it does nothing");
  }
  else if (EventKeyboard::KeyCode::KEY_ESCAPE == keyCode) {
    mcExit(nullptr);
  }
  else if (EventKeyboard::KeyCode::KEY_ENTER == keyCode) {
    mcPlayAgain(nullptr);
  }
  else if (EventKeyboard::KeyCode::KEY_X == keyCode) {
    C6_D1(c6, "Need to get out (debug, x pressed).");

    // Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();
  }
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void GameEndScene::mcExit(cocos2d::Ref *) {
  Director::getInstance()->end();
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void GameEndScene::mcPlayAgain(cocos2d::Ref *) {
  Scene* scene = BattleScene::createScene(false, c6);
  if (scene == nullptr) {
    return;
  }

  Director::getInstance()->replaceScene(scene);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void GameEndScene::unloadSpriteCache() {
//  BulletNode::unloadAnimations();
  SpriteFrameCache::getInstance()->removeSpriteFramesFromFile(kPlistFileName);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
