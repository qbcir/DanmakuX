#include "BulletDesc.h"
#include "DataParser.h"

bool BulletDesc::initFromJson(const rapidjson::Value& el) {
    if (!GameObjectDesc::initFromJson(el)) {
        return false;
    }
    DX_DESC_GET_MEMBER(el, float, damage);
    return true;
}

bool BulletDesc::initFromLua(lua_State *L, int idx) {
    if (!GameObjectDesc::initFromLua(L, idx)) {
        return false;
    }
    DX_LUA_DESC_GET_MEMBER(L, idx, float, damage);
    return true;
}

bool BulletDesc::isEnemy() const {
    return m_enemy;
}

void BulletDesc::setEnemy(float enemy) {
    m_enemy = enemy;
}

float BulletDesc::getDamage() const {
    return m_damage;
}
