#ifndef __dx_TILEDMAPLEVELLAYER_H__
#define __dx_TILEDMAPLEVELLAYER_H__

#include "LevelLayer.h"
#include "../Data/TileMapLevelDesc.h"

class TiledMapLevelLayer : public LevelLayer {
public:
    DX_DEFINE_FROM_JSON_FILE_FUNC(TiledMapLevelLayer);
    DX_CREATE_FROM_JSON_FUNC(TiledMapLevelLayer)
    virtual void update(float dt);
protected:
    void setViewPointCenter(const cocos2d::Vec2 &pos);
    bool initFromJson(const rapidjson::Value& el);
    bool initWithTileMapLevelDesc(TileMapLevelDesc* desc);
    cocos2d::Size m_tileSize;
    cocos2d::Size m_tileMapSize;
    cocos2d::Size m_mapSize;
    cocos2d::Size m_centerOfView;
    cocos2d::TMXTiledMap* m_tileMap = nullptr;
};

#endif // __dx_TILEDMAPLEVELLAYER_H__
