#include "GameManager.h"
#include "Layers/GameScene.h"
#include "Data/GameDesc.h"
#include "register_all_packages.h"
#include <fstream>

bool GameManager::m_released = false;
GameManager* GameManager::m_instance = nullptr;

GameManager::GameManager() {
    m_director = cocos2d::Director::getInstance();
    m_fileUtils = cocos2d::FileUtils::getInstance();
    m_scriptEngine = cocos2d::LuaEngine::getInstance();
}

GameManager::~GameManager() {
    CC_SAFE_DELETE(m_gameDesc);
}

GameManager* GameManager::getInstance() {
    CCASSERT(!m_released, "GameManager already released");
    if (!m_instance) {
        m_instance = new GameManager();
    }
    return m_instance;
}

void GameManager::releaseInstance() {
    if (m_instance) {
        delete m_instance;
    }
    m_released = true;
}

int GameManager::getPlatform() {
    return CC_TARGET_PLATFORM;
}

void GameManager::start() {
    m_startTime = std::chrono::high_resolution_clock::now();
    m_fileUtils->addSearchPath("data");
    m_fileUtils->addSearchPath("lua");
    m_fileUtils->addSearchPath("res");
    auto wrPath = m_fileUtils->getWritablePath();
    m_fileUtils->addSearchPath(wrPath);

    bool ok = m_collisionManager.init();
    CCASSERT(ok, "Can't init collision manager");

    cocos2d::ScriptEngineManager::getInstance()->setScriptEngine(m_scriptEngine);
    auto luaPath = m_fileUtils->fullPathForFilename("lua");
    cocos2d::log("luaPath: %s", luaPath.c_str());
    m_scriptEngine->addSearchPath(luaPath.c_str());
    m_luaStack = m_scriptEngine->getLuaStack();
    m_luaState = m_luaStack->getLuaState();
    lua_register_all_packages(m_luaState);
    m_scriptEngine->executeScriptFile("lua/global.lua");
    m_gameDesc = GameDesc::fromJson("data/app.json");
    CCASSERT(m_gameDesc != nullptr, "GameDesc is null!");
    m_gameDesc->retain();
    SceneDesc* entryScene = nullptr;
    for (auto sceneDesc : m_gameDesc->scenes) {
        if (sceneDesc->role == "main") {
            entryScene = sceneDesc;
        }
        m_sceneDescs[sceneDesc->name] = sceneDesc;
    }
    if (!entryScene) {
        entryScene = m_gameDesc->scenes.at(0);
    }
    runScene(entryScene);
}

ObjectManager* GameManager::getObjectManager() {
    return &m_objectManager;
}

InputManager* GameManager::getInputManager() {
    return &m_inputManager;
}

CollisionManager* GameManager::getCollisionManager() {
    return &m_collisionManager;
}

ComponentManager* GameManager::getComponentManager() {
    return &m_componentManager;
}

NetworkManager* GameManager::getNetworkManager() {
    return &m_networkManager;
}

GameDesc* GameManager::getGameDesc() {
    return m_gameDesc;
}

SceneDesc* GameManager::getCurrSceneDesc() {
    return m_currSceneDesc;
}

SceneDesc* GameManager::getSceneDescription(const std::string& name) {
    return m_sceneDescs[name];
}

void GameManager::runScene(const std::string& name) {
    auto sceneDesc = getSceneDescription(name);
    runScene(sceneDesc);
}

void GameManager::runScene(SceneDesc* sceneDesc) {
    m_luaStack->clean();
    CCASSERT(sceneDesc != nullptr, "Scene description is null!");
    auto scene = GameScene::createScene(sceneDesc);
    if (!m_currScene) {
        m_director->runWithScene(scene);
    } else {
        m_director->replaceScene(scene);
    }
    m_currScene = scene;
    m_currSceneDesc = sceneDesc;
}

void GameManager::writeStringToFile(
        const std::string& filename,
        const std::string& content) {
    auto wrPath = m_fileUtils->getWritablePath();
    auto fPath = wrPath + filename;
    std::ofstream os(fPath);
    os << content;
}

float GameManager::getTime() {
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float, std::ratio<1, 1000>> ms = now - m_startTime;
    return ms.count();
}

