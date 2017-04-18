#include "TiledMapLevelLayer.h"
#include "../Data/DataParser.h"
#include "../Objects/PlayerObject.h"

DX_CREATE_FROM_JSON_FILE_FUNC(TiledMapLevelLayer)

bool TiledMapLevelLayer::initFromJson(const rapidjson::Value &el) {
    auto levelDesc = TileMapLevelDesc::fromJson(el);
    if (!levelDesc) {
        return false;
    }
    return initWithTileMapLevelDesc(levelDesc);
}

bool TiledMapLevelLayer::initWithTileMapLevelDesc(TileMapLevelDesc* levelDesc) {
    if (!LevelLayer::initWithLevelDesc(levelDesc)) {
        return false;
    }
    m_centerOfView = m_winSize / 2;
    m_tileMap = cocos2d::TMXTiledMap::create(levelDesc->getTilemap());
    m_tileMapSize = m_tileMap->getMapSize();
    m_tileSize = m_tileMap->getTileSize();
    m_mapSize = cocos2d::Size(m_tileMapSize.width * m_tileSize.width,
        m_tileMapSize.height * m_tileSize.height);
    m_gridCellSize = cocos2d::Size(32, 32);
    m_gridSize = cocos2d::Size((m_mapSize.width / m_gridCellSize.width) + 1,
        (m_mapSize.height / m_gridCellSize.height) + 1);
    m_gridVecSize = m_gridSize.width * m_gridSize.height;
    m_gridRect = cocos2d::Rect(0, 0, m_mapSize.width, m_mapSize.height);
    auto objGroup = m_tileMap->getObjectGroup("GameObjects");
    auto& objects = objGroup->getObjects();
    for (auto object : objects) {
    }
    m_playerObj->setMaxPos(m_mapSize.width, m_mapSize.height);
    cocos2d::log("set tile map %f %f",
        m_tileMap->getPositionX(), m_tileMap->getPositionY());
    this->addChild(m_tileMap, -1);
    return true;
}

void TiledMapLevelLayer::update(float dt) {
    LevelLayer::update(dt);
    setViewPointCenter(m_playerObj->getPosition());
}

void TiledMapLevelLayer::setViewPointCenter(const cocos2d::Vec2& pos) {
    cocos2d::Vec2 pos_(
        std::min(std::max(pos.x, m_winSize.width/2), m_mapSize.width - m_winSize.width / 2),
        std::min(std::max(pos.y, m_winSize.height/2), m_mapSize.height - m_winSize.height / 2));
    cocos2d::Vec2 viewPoint = cocos2d::Vec2(m_centerOfView) - pos_;
    this->setPosition(viewPoint);
}
