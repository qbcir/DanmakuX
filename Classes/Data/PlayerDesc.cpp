#include "PlayerDesc.h"
#include "DataParser.h"

PlayerDesc::~PlayerDesc() {
    CC_SAFE_RELEASE(m_vehicleDesc);
}

PlayerDesc* PlayerDesc::createEmpty() {
    auto pd = new (std::nothrow) PlayerDesc();
    if (pd) {
        pd->autorelease();
        return pd;
    }
    return nullptr;
}

bool PlayerDesc::initFromJson(const rapidjson::Value& el) {
    DX_DESC_GET_MEMBER(el, string, label);
    DX_DESC_GET_REF_MEMBER(el, VehicleDesc, vehicleDesc);
    DX_DESC_GET_VEC(el, WeaponDesc, weapons, m_weapons);
    CC_SAFE_RETAIN(m_vehicleDesc);
    return true;
}

bool PlayerDesc::initFromLua(lua_State* L, int idx) {
    DX_LUA_DESC_GET_MEMBER(L, idx, string, label);
    DX_LUA_DESC_GET_REF_MEMBER(L, idx, VehicleDesc, vehicleDesc);
    DX_LUA_DESC_GET_VEC(L, idx, WeaponDesc, weapons, m_weapons);
    CC_SAFE_RETAIN(m_vehicleDesc);
    return true;
}

const std::string& PlayerDesc::getLabel() const {
    return m_label;
}

void PlayerDesc::setVehicleDesc(VehicleDesc* vd) {
    CC_SAFE_RELEASE(m_vehicleDesc);
    CC_SAFE_RETAIN(vd);
    m_vehicleDesc = vd;
}

VehicleDesc* PlayerDesc::getVehicleDesc() {
    return m_vehicleDesc;
}

void PlayerDesc::reset() {
    m_vehicleDesc = nullptr;
    m_weapons.clear();
}

void PlayerDesc::setMaxWeaponSlots(size_t num) {

}

void PlayerDesc::replaceWeaponDesc(size_t idx, WeaponDesc* desc) {
    m_weapons.replace(idx ,desc);
}

void PlayerDesc::addWeaponDesc(WeaponDesc* desc) {
    m_weapons.pushBack(desc);
}

const cocos2d::Vector<WeaponDesc*>& PlayerDesc::getWeapons() {
    return m_weapons;
}
