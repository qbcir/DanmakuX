#ifndef __dx_SIMPLELEVELLAYER_H__
#define __dx_SIMPLELEVELLAYER_H__

#include "LevelLayer.h"
#include "../Data/SimpleLevelDesc.h"

class SimpleLevelLayer : public LevelLayer {
public:
    DX_DEFINE_FROM_JSON_FILE_FUNC(SimpleLevelLayer);
    DX_CREATE_FROM_JSON_FUNC(SimpleLevelLayer)
    void update(float dt);
protected:
    bool initFromJson(const rapidjson::Value &el);
    bool initWithSimpleLevelDesc(SimpleLevelDesc* desc);
    void updateBackground(float dt);
    BackgroundType m_bgType;
    std::vector<cocos2d::Sprite*> m_bgSprites;
};

#endif // __dx_SIMPLELEVELLAYER_H__
