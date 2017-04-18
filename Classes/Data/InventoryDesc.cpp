#include "InventoryDesc.h"
#include "BulletDesc.h"
#include "ParticleDesc.h"
#include "JSONParser.h"
#include "ObjectManager.h"

DX_CREATE_FROM_JSON_FILE_FUNC(InventoryDesc)

bool InventoryDesc::initFromJson(const rapidjson::Value& el) {
    cocos2d::Vector<BulletDesc*> bulletDefs;
    DX_DESC_GET_OPT_VEC(el, BulletDesc, bulletDefs, bulletDefs);
    cocos2d::Vector<ActionDesc*> actionDefs;
    DX_DESC_GET_OPT_VEC(el, ActionDesc, actionDefs, actionDefs);
    cocos2d::Vector<ParticleDesc*> particleDefs;
    DX_DESC_GET_OPT_VEC(el, ParticleDesc, particleDefs, particleDefs);
    DX_DESC_GET_VEC(el, WeaponDesc, weapons, m_weapons);
    DX_DESC_GET_VEC(el, VehicleDesc, vehicles, m_vehicles);
    auto objMgr = ObjectManager::getInstance();
    for (auto actionDef : actionDefs) {
        objMgr->addActionDesc(actionDef);
    }
    for (auto bulletDef : bulletDefs) {
        bulletDef->setEnemy(false);
        objMgr->addBulletDesc(bulletDef);
    }
    for (auto particleDef : particleDefs) {
        objMgr->addParticleDesc(particleDef);
    }
    return true;
}

const cocos2d::Vector<WeaponDesc*>& InventoryDesc::getWeapons() {
    return m_weapons;
}

const cocos2d::Vector<VehicleDesc*>& InventoryDesc::getVehicles() {
    return m_vehicles;
}
