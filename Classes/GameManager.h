#ifndef __danmakuX_GameManager_h__
#define __danmakuX_GameManager_h__

#include "cocos2d.h"
#include "ObjectManager.h"
#include "InputManager.h"
#include "CollisionManager.h"
#include "ComponentManager.h"
#include "Network//NetworkManager.h"
#include "Data/GameDesc.h"
#include "CCLuaEngine.h"
#include <chrono>

class LevelLayer;
class GameManager {
public:
  static GameManager* getInstance();
  static int getPlatform();
  void releaseInstance();
  void start();
  void update(float dt);
  ObjectManager* getObjectManager();
  InputManager* getInputManager();
  CollisionManager* getCollisionManager();
  ComponentManager* getComponentManager();
  NetworkManager* getNetworkManager();
  GameDesc* getGameDesc();
  SceneDesc* getCurrSceneDesc();
  SceneDesc* getSceneDescription(const std::string& name);
  void runScene(const std::string& name);
  void runScene(SceneDesc *sceneDesc);
  void writeStringToFile(const std::string& filename,
                         const std::string& content);
  float getTime();
  float getFrameStartTime() const;
  void setLevelLayer(LevelLayer* ll);
  LevelLayer* getLevelLayer();
private:
  GameManager();
  ~GameManager();
  static bool m_released;
  static GameManager* m_instance;
  ObjectManager m_objectManager;
  InputManager m_inputManager;
  CollisionManager m_collisionManager;
  ComponentManager m_componentManager;
  NetworkManager m_networkManager;
  cocos2d::Scene* m_currScene = nullptr;
  cocos2d::Director* m_director = nullptr;
  cocos2d::FileUtils* m_fileUtils = nullptr;
  cocos2d::LuaEngine* m_scriptEngine = nullptr;
  lua_State* m_luaState = nullptr;
  cocos2d::LuaStack* m_luaStack = nullptr;
  GameDesc* m_gameDesc = nullptr;
  SceneDesc* m_currSceneDesc = nullptr;
  LevelLayer* m_currLevelLayer = nullptr;
  std::unordered_map<std::string, SceneDesc*> m_sceneDescs;
  std::chrono::high_resolution_clock::time_point m_startTime;
  float m_frameStartTime;
};

#endif /* __danmakuX_GameManager_h__ */
