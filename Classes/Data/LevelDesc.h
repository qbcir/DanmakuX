#ifndef __dx_LevelDesc_h__
#define __dx_LevelDesc_h__

#include "cocos2d.h"
#include "GameAction.h"
#include "GameObjectDesc.h"
#include "DataMacros.h"

class LevelDesc : public cocos2d::Ref {
public:
    DX_DEFINE_FROM_JSON_FILE_FUNC(LevelDesc);
    DX_CREATE_FROM_JSON_FUNC(LevelDesc)
    GameAction *getActions() const;
protected:
    bool initFromJson(const rapidjson::Value& el);
    std::vector<ScriptComponentDesc> m_components;
    std::string m_script;
    GameAction* m_levelActions = nullptr;
};

#endif /* __dx_LevelDesc_h__ */
