#include "TileMapLevelDesc.h"
#include "JSONParser.h"

DX_CREATE_FROM_JSON_FILE_FUNC(TileMapLevelDesc)

bool TileMapLevelDesc::initFromJson(const rapidjson::Value& el) {
    if (!LevelDesc::initFromJson(el)) {
        return false;
    }
    DX_DESC_GET_MEMBER(el, string, tilemap);
    return true;
}

const std::string& TileMapLevelDesc::getTilemap() const {
    return m_tilemap;
}
