#ifndef INVENTORYDESC_H
#define INVENTORYDESC_H

#include "VehicleDesc.h"
#include "WeaponDesc.h"

class InventoryDesc : cocos2d::Ref
{
public:
    DX_CREATE_FROM_JSON_FUNC(InventoryDesc)
    DX_DEFINE_FROM_JSON_FILE_FUNC(InventoryDesc);
    const cocos2d::Vector<WeaponDesc*>& getWeapons();
    const cocos2d::Vector<VehicleDesc*>& getVehicles();
protected:
    bool initFromJson(const rapidjson::Value& el);
    cocos2d::Vector<WeaponDesc*> m_weapons;
    cocos2d::Vector<VehicleDesc*> m_vehicles;
};

#endif // INVENTORYDESC_H
