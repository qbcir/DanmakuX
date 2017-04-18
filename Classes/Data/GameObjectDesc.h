#ifndef GAMEOBJECTDESC_H
#define GAMEOBJECTDESC_H

#include "cocos2d.h"
#include "ScriptComponentDesc.h"
#include "SpriteDesc.h"
#include "../GameAction.h"

class GameObjectDesc : public cocos2d::Ref {
public:
    virtual ~GameObjectDesc();
    static GameObjectDesc* fromJsonStr(const std::string& s);
    DX_CREATE_FROM_JSON_FUNC(GameObjectDesc)
    DX_CREATE_FROM_LUA_FUNC(GameObjectDesc)
    GameAction* getAction();
    const std::string& getLabel() const;
    const SpriteDesc& getSpriteDesc() const;
    const std::vector<ScriptComponentDesc>& getComponentDescs() const;
    const cocos2d::Vector<GameObjectDesc*>& getObjectDescs() const;
    bool hasMaxSpeed() const;
    float getMaxSpeed() const;
    bool hasMaxRotation() const;
    float getMaxRotation() const;
    bool hasMaxAngularAccel() const;
    float getMaxAngularAccel() const;
    bool getAutoUpdateDirection() const;
protected:
    bool initFromJson(const rapidjson::Value& el);
    bool initFromLua(lua_State* L, int idx);
    std::string m_label;
    SpriteDesc m_sprite;
    std::vector<ScriptComponentDesc> m_components;
    cocos2d::Vector<GameObjectDesc*> m_objects;
    GameAction* m_action = nullptr;
    float m_maxSpeed = std::numeric_limits<float>::max();
    float m_maxRotation = std::numeric_limits<float>::max();
    float m_maxAngularAccel = std::numeric_limits<float>::max();
    bool m_autoUpdateDirection = false;
};

#endif // GAMEOBJECTDESC_H
