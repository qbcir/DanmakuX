#include "PlayerDesc.h"

PlayerDesc::~PlayerDesc() {
    CC_SAFE_RELEASE(m_vehicleDesc);
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
