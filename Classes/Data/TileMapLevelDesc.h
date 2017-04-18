#ifndef __dx_TILEDMAPLEVELDESC_H__
#define __dx_TILEDMAPLEVELDESC_H__

#include "LevelDesc.h"

class TileMapLevelDesc : public LevelDesc
{
public:
    DX_DEFINE_FROM_JSON_FILE_FUNC(TileMapLevelDesc);
    DX_CREATE_FROM_JSON_FUNC(TileMapLevelDesc)
    const std::string& getTilemap() const;
protected:
    bool initFromJson(const rapidjson::Value& el);
    std::string m_tilemap;
};

#endif // __dx_TILEDMAPLEVELDESC_H__
