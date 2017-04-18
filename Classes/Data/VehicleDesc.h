#ifndef VEHICLEDESC_H
#define VEHICLEDESC_H

#include "GameObjectDesc.h"

class GameObject;
class VehicleDesc : public GameObjectDesc
{
public:
    DX_CREATE_FROM_JSON_FUNC(VehicleDesc)
    DX_CREATE_FROM_LUA_FUNC(VehicleDesc)
    float getHealth() const;
    float getDamage() const;
protected:
    bool initFromJson(const rapidjson::Value& el);
    bool initFromLua(lua_State* L, int idx);
    float m_health = 0;
    float m_damage = 0;
};

#endif // VEHICLEDESC_H
