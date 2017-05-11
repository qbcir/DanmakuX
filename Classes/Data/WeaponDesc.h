#ifndef WEAPONDESC_H
#define WEAPONDESC_H

#include "../GameAction.h"

class WeaponDesc : public cocos2d::Ref {
public:
    DX_CREATE_FROM_JSON_FUNC(WeaponDesc)
    DX_CREATE_FROM_LUA_FUNC(WeaponDesc)
    ActionDesc* getActionDesc();
protected:
    bool initFromJson(const rapidjson::Value& el);
    bool initFromLua(lua_State* L, int idx);
    std::string m_label;
    std::string m_image;
    std::string m_description;
    cocos2d::Vector<GameAction*> m_actions;
};

#endif // WEAPONDESC_H
