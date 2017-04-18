#ifndef BULLETDESC_H
#define BULLETDESC_H

#include "GameObjectDesc.h"

class GameObject;
class BulletDesc : public GameObjectDesc
{
public:
    DX_CREATE_FROM_JSON_FUNC(BulletDesc)
    DX_CREATE_FROM_LUA_FUNC(BulletDesc)
    bool isEnemy() const;
    void setEnemy(float enemy);
    float getDamage() const;
protected:
    bool initFromJson(const rapidjson::Value& el);
    bool initFromLua(lua_State *L, int idx);
    bool m_enemy = true;
    float m_damage = 0;
    cocos2d::Vector<GameAction*> m_onHit;
};

#endif // BULLETDESC_H
