#include "LevelLayer.h"
#include "../Data/LevelDesc.h"
#include "GameManager.h"
#include "CollisionManager.h"
#include "ComponentManager.h"
#include "../Input/InputComponent.h"
#include "../Objects/PlayerObject.h"
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

bool LevelLayer::initWithLevelDesc(LevelDesc* levelDesc) {
    // init collision grid
    m_gridCellSize = cocos2d::Size(32, 32);
    m_winSize = cocos2d::Director::getInstance()->getWinSize();
    m_gridSize = cocos2d::Size((m_winSize.width / m_gridCellSize.width) + 1,
        (m_winSize.height / m_gridCellSize.height) + 1);
    m_gridVecSize = m_gridSize.width * m_gridSize.height;
    m_gridRect = cocos2d::Rect(0, 0, m_winSize.width, m_winSize.height);
    auto objMgr = ObjectManager::getInstance();
    auto playerObjDesc = objMgr->getPlayerDesc();
    auto playerObj = PlayerObject::createFromDesc(playerObjDesc);
    if (!playerObj) {
        cocos2d::log("Can't create 'PlayerObject' from 'PlayerDesc'");
        return false;
    }
    playerObj->setMinPos(m_gridRect.getMinX(), m_gridRect.getMinY());
    playerObj->setMaxPos(m_gridRect.getMaxX(), m_gridRect.getMaxY());
    playerObj->setPosition(m_winSize.width/2, m_winSize.height/2);
    this->setPlayerObject(playerObj);
    auto compMgr = ComponentManager::getInstance();
    compMgr->setPlayerObject(playerObj);
    auto levelActionsDesc = levelDesc->getActions();
    m_levelAction = levelActionsDesc->getAction();
    CCASSERT(m_levelAction != nullptr, "Level 'main' action is null");
    this->runAction(m_levelAction);
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
    if (m_paused) {
        return;
    }
    resolveCollisions();
    removeDeadObjects();
    updateGrid();
    removeOutOfGridObjects();
    for (auto obj : m_objects) {
        obj->update(dt);
    }
}

void LevelLayer::onExit() {
    auto compMgr = ComponentManager::getInstance();
    compMgr->setPlayerObject(nullptr);
    Layer::onExit();
}

void LevelLayer::addObject(GameObject* obj) {
    this->addChild(obj);
    m_objects.pushBack(obj);
}

void LevelLayer::setPlayerObject(PlayerObject* obj) {
    m_playerObj = obj;
    m_playerObj->retain();
    this->addObject(obj);
}

PlayerObject* LevelLayer::getPlayerObject() {
    return m_playerObj;
}



bool LevelLayer::isOutOfGrid(float x, float y) {
    return x < 0 || x > m_winSize.width || y < 0 || y > m_winSize.height;
}

size_t LevelLayer::positionToGridIndex(float x, float y) {
    auto ix = x / m_gridCellSize.width;
    auto iy = y / m_gridCellSize.height;
    return iy * m_gridSize.width + ix;
}

void LevelLayer::updateGrid() {
    LevelGridT updatedGrid(m_gridVecSize);
    for (GameObject* obj : m_objects) {
        cocos2d::Rect bbox = cocos2d::utils::getCascadeBoundingBox(obj);
        int x = bbox.origin.x / m_gridCellSize.width;
        int y = bbox.origin.y / m_gridCellSize.height;
        int w = std::ceil(bbox.size.width / m_gridCellSize.width);
        int h = std::ceil(bbox.size.height / m_gridCellSize.height);
        if (!m_gridRect.intersectsRect(bbox)) {
            obj->setOnGrid(false);
            continue;
        }
        for (size_t i = 0; i < w; i++) {
            for (size_t j = 0; j < h; j++) {
                if (isOutOfGrid(x + i, y + j)) {
                    continue;
                }
                int gidx = (y + j)* m_gridSize.width + (x + i);
                updatedGrid.at(gidx).push_back(obj);
            }
        }
    }
    m_grid.swap(updatedGrid);
}

void LevelLayer::resolveCollisions() {
    auto cMgr = CollisionManager::getInstance();
    for (auto& gridCell : m_grid) {
        for (size_t i = 0; i < gridCell.size(); i++) {
            for (size_t j = i; j < gridCell.size(); j++) {
                if (i == j) continue;
                auto obj1 = gridCell[i];
                auto obj2 = gridCell[j];
                if (obj1->canCollideWith(obj2) || obj2->canCollideWith(obj1)) {
                    cMgr->testCollision(obj1, obj2);
                    //cMgr->sync();
                }
            }
        }
    }
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
        if (obj->isOnGrid()) {
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
