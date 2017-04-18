#include "SimpleLevelDesc.h"
#include "DataParser.h"

DX_DEF_STRUCT_P(BackgroundDesc)
DX_ADD_FIELD_P(BackgroundDesc, bgType);
DX_ADD_FIELD_P(BackgroundDesc, bgSprite);

DX_CREATE_FROM_JSON_FILE_FUNC(SimpleLevelDesc)

BackgroundType Str2BackgroundType(const std::string& t) {
    if (t == "moving") {
        return BackgroundType::MOVING;
    } else if (t == "static") {
        return BackgroundType::STATIC;
    }
    return BackgroundType::ERROR;
}

bool SimpleLevelDesc::initFromJson(const rapidjson::Value& el) {
    if (!LevelDesc::initFromJson(el)) {
        return false;
    }
    DX_DESC_GET_MEMBER(el, BackgroundDesc, bg);
    return true;
}

const BackgroundDesc& SimpleLevelDesc::getBackgroundDesc() const {
    return m_bg;
}
