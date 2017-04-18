#include "VehicleDesc.h"
#include "DataParser.h"

bool VehicleDesc::initFromJson(const rapidjson::Value& el) {
    if (!GameObjectDesc::initFromJson(el)) {
        return false;
    }
    DX_DESC_GET_MEMBER(el, float, health);
    DX_DESC_GET_OPT_MEMBER(el, float, damage);
    return true;
}

bool VehicleDesc::initFromLua(lua_State* L, int idx) {
    if (!GameObjectDesc::initFromLua(L, idx)) {
        return false;
    }
    DX_LUA_DESC_GET_MEMBER(L, idx, float, health);
    DX_LUA_DESC_GET_OPT_MEMBER(L, idx, float, damage);
    return true;
}

float VehicleDesc::getHealth() const {
    return m_health;
}

float VehicleDesc::getDamage() const {
    return m_damage;
}
