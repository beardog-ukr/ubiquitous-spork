#include "AppDelegate.h"
#include "battle/BattleScene.h"
#include "ui/GameEndScene.h"

#include "SixCatsLogger.h"
#include "SixCatsLoggerMacro.h"
#include <sstream>

// #define USE_AUDIO_ENGINE 1

#if USE_AUDIO_ENGINE
#include "audio/include/AudioEngine.h"
using namespace cocos2d::experimental;
#endif

USING_NS_CC;

static cocos2d::Size designResolutionSize = cocos2d::Size(1280, 720);

AppDelegate::AppDelegate() {
  //Logging level for the application is set here in next line
  c6 = std::make_shared<SixCatsLogger>(SixCatsLogger::Critical);
}

AppDelegate::~AppDelegate() {
#if USE_AUDIO_ENGINE
  AudioEngine::end();
#endif
}

// if you want a different context, modify the value of glContextAttrs
// it will affect all platforms
void AppDelegate::initGLContextAttrs() {
  // set OpenGL context attributes: red,green,blue,alpha,depth,stencil,multisamplesCount
  GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8, 0};

  GLView::setGLContextAttrs(glContextAttrs);
}

// if you want to use the package manager to install more packages,
// don't modify or remove this function
static int register_all_packages() {
  return 0;   //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching() {
  // initialize director
  auto director = Director::getInstance();
  auto glview = director->getOpenGLView();
  if(!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || \
    (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
    glview =
      GLViewImpl::createWithRect("AntiAirborne",
                                 cocos2d::Rect(0, 0, designResolutionSize.width,
                                               designResolutionSize.height));
#else
    glview = GLViewImpl::create("AntiAirborne");
#endif
    director->setOpenGLView(glview);
  }

  // turn on/off display FPS
  director->setDisplayStats(false);

  // set FPS. the default value is 1.0/60 if you don't call this
  director->setAnimationInterval(1.0f / 60);

  // Set the design resolution
  glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height,
                                  ResolutionPolicy::NO_BORDER);

  register_all_packages();

  Scene* scene = anti_airborne::BattleScene::createScene(true, c6);
//  Scene* scene = anti_airborne::GameEndScene::createScene(false, c6);
  if (scene == nullptr) {
    C6_C1(c6, "Failed to create a scene.");
    return false;
  }

  // run
  director->runWithScene(scene);

  return true;
}

// This function will be called when the app is inactive. Note, when receiving a phone call it is invoked.
void AppDelegate::applicationDidEnterBackground() {
  Director::getInstance()->stopAnimation();

#if USE_AUDIO_ENGINE
  AudioEngine::pauseAll();
#endif
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
  Director::getInstance()->startAnimation();

#if USE_AUDIO_ENGINE
  AudioEngine::resumeAll();
#endif
}
