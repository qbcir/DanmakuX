#include "LevelLayer.h"
#include "../Data/LevelDesc.h"
#include "../GameManager.h"
#include "../CollisionManager.h"
#include "../ComponentManager.h"
#include "../Input/InputComponent.h"
#include "../Objects/PlayerObject.h"
#include "../Data/DataParser.h"
#include "utils.h"

LevelLayer* LevelLayer::createFromJson(const std::string& filename) {
    LevelLayer* ll = new (std::nothrow) LevelLayer();
    if (ll && ll->initFromJson(filename)) {
        ll->autorelease();
        return ll;
    }
    CC_SAFE_DELETE(ll);
    return nullptr;
}

LevelLayer::~LevelLayer() {
    delete m_world;
}

bool LevelLayer::initWithLevelDesc(LevelDesc* levelDesc) {
    auto cm = CollisionManager::getInstance();
    b2Vec2 gravity(0, 0);
    m_debugDraw = cocos2d::DrawNode::create();
    this->addChild(m_debugDraw, 4);
    cm->setDrawNode(m_debugDraw);
    m_world = new b2World(gravity);
    m_world->SetContactListener(cm->getContactListener());
    //auto dd = cm->getDebugDraw();
    //dd->AppendFlags(b2Draw::e_shapeBit);
    //m_world->SetDebugDraw(dd);
    m_winSize = cocos2d::Director::getInstance()->getWinSize();
    auto objMgr = ObjectManager::getInstance();
    auto playerObjDesc = objMgr->getPlayerDesc();
    if (playerObjDesc) {
        auto playerObj = PlayerObject::createFromDesc(playerObjDesc);
        if (!playerObj) {
            cocos2d::log("Can't create 'PlayerObject' from 'PlayerDesc'");
            return false;
        }
        this->setPlayerObject(playerObj);
        playerObj->setMinPos(0, 0);
        playerObj->setMaxPos(m_winSize.width, m_winSize.height);
        playerObj->setPosition(m_winSize.width/2, m_winSize.height/2);
        auto compMgr = ComponentManager::getInstance();
        compMgr->setPlayerObject(playerObj);
    } else {
        cocos2d::log("Player creation skipped");
    }
    auto levelActionsDesc = levelDesc->getActions();
    m_levelAction = levelActionsDesc->getAction();
    CCASSERT(m_levelAction != nullptr, "Level 'main' action is null");
    this->runAction(m_levelAction);
    m_lastTimestamp = GameManager::getInstance()->getTime();
    return true;
}

bool LevelLayer::initFromJson(const std::string& filename) {
    if (!Layer::init()) {
        return false;
    }
    auto levelDesc = LevelDesc::fromJsonFile(filename);
    if (!levelDesc) {
        return false;
    }
    return initWithLevelDesc(levelDesc);
}

void LevelLayer::update(float dt) {
    Layer::update(dt);
    if (m_paused) {
        return;
    }
    m_debugDraw->clear();
    m_world->Step(1.f/60.f, 8, 3);
    m_world->DrawDebugData();
    for (GameObject* obj : m_objects) {
        obj->update(dt);
    }
    removeDeadObjects();
    removeOutOfGridObjects();
    /*
    auto gm = GameManager::getInstance();
    auto curr = gm->getTime();
    float delta = curr - m_lastTimestamp;

    m_lastTimestamp = curr;
    */
}

void LevelLayer::onExit() {
    auto compMgr = ComponentManager::getInstance();
    compMgr->setPlayerObject(nullptr);
    Layer::onExit();
}

b2Body* LevelLayer::createPhysBody(GameObject* obj) {
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.userData = (void*)obj;
    auto phyBody = m_world->CreateBody(&bodyDef);
    obj->setPhysBody(phyBody);
    obj->setPhysWorld(m_world);
    //
    auto spName = obj->getSpriteFilename();
    const auto& triangles = CollisionManager::getInstance()->getPolygonData(spName);
    b2PolygonShape shape;
    b2FixtureDef fixtureDef;
    for (const auto& triangle : triangles) {
        shape.Set(triangle.data(), triangle.size());
        fixtureDef.isSensor = true;
        fixtureDef.shape = &shape;
        fixtureDef.userData = (void*)obj;
        fixtureDef.filter.categoryBits = 1 << obj->getCollisionClass();
        fixtureDef.filter.maskBits = obj->getCollisionMask();
        phyBody->CreateFixture(&fixtureDef);
    }
    return phyBody;
}

void LevelLayer::addObject(GameObject* obj) {
    this->addChild(obj, 1);
    obj->unscheduleUpdate();
    m_objects.pushBack(obj);
    obj->setLevelLayer(this);
}

void LevelLayer::setPlayerObject(PlayerObject* obj) {
    m_playerObj = obj;
    m_playerObj->retain();
    this->createPhysBody(obj);
    this->addObject(obj);
}

PlayerObject* LevelLayer::getPlayerObject() {
    return m_playerObj;
}

bool LevelLayer::isOutOfGrid(float x, float y) {
    return x < 0 || x > m_winSize.width || y < 0 || y > m_winSize.height;
}

void LevelLayer::resolveCollisions() {
}

void LevelLayer::removeDeadObjects() {
    cocos2d::Vector<GameObject*> liveObjects;
    for (GameObject* obj : m_objects) {
        if (!obj->isDead()) {
            liveObjects.pushBack(obj);
        } else {
            this->removeChild(obj);
        }
    }
    std::swap(m_objects, liveObjects);
}

void LevelLayer::removeOutOfGridObjects() {
    cocos2d::Vector<GameObject*> liveObjects;
    for (GameObject* obj : m_objects) {
        auto bbox = cocos2d::utils::getCascadeBoundingBox(obj);
        if (m_worldBounds.intersectsRect(bbox)) {
            liveObjects.pushBack(obj);
        } else {
            this->removeChild(obj);
        }
    }
    std::swap(m_objects, liveObjects);
}

bool LevelLayer::isPaused() {
    return m_paused;
}

bool LevelLayer::setPaused(bool p) {
    if (m_paused != p) {
        m_paused = p;
        if (p) {
            pauseLevel();
        } else {
            resumeLevel();
        }
    }
}

void LevelLayer::pauseLevel() {
    this->pause();
    for (auto obj : m_objects) {
        pauseNode(obj);
    }
}

void LevelLayer::resumeLevel() {
    this->resume();
    for (auto obj : m_objects) {
        resumeNode(obj);
    }
}

