#ifndef __dx_SIMPLELEVELLAYERDESC_H__
#define __dx_SIMPLELEVELLAYERDESC_H__

#include "LevelDesc.h"

enum BackgroundType {
    MOVING,
    STATIC,
    ERROR = -1
};

BackgroundType Str2BackgroundType(const std::string& t);

class BackgroundDesc {
public:
    std::string bgType;
    std::string bgSprite;
};

class SimpleLevelDesc : public LevelDesc {
public:
    DX_DEFINE_FROM_JSON_FILE_FUNC(SimpleLevelDesc);
    DX_CREATE_FROM_JSON_FUNC(SimpleLevelDesc)
    const BackgroundDesc& getBackgroundDesc() const;
protected:
    bool initFromJson(const rapidjson::Value& el);
    BackgroundDesc m_bg;
};

#endif // __dx_SIMPLELEVELLAYERDESC_H__
