#ifndef PLAYERDESC_H
#define PLAYERDESC_H

#include "VehicleDesc.h"
#include "WeaponDesc.h"

class PlayerDesc : public cocos2d::Ref
{
public:
    ~PlayerDesc();
    void setVehicleDesc(VehicleDesc* vd);
    VehicleDesc* getVehicleDesc();
    void setMaxWeaponSlots(size_t num);
    void replaceWeaponDesc(size_t idx, WeaponDesc* desc);
    void addWeaponDesc(WeaponDesc* desc);
    const cocos2d::Vector<WeaponDesc*>& getWeapons();
    void reset();
protected:
    VehicleDesc* m_vehicleDesc = nullptr;
    cocos2d::Vector<WeaponDesc*> m_weapons;
};

#endif // PLAYERDESC_H
