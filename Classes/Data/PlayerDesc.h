#ifndef PLAYERDESC_H
#define PLAYERDESC_H

#include "VehicleDesc.h"
#include "WeaponDesc.h"

class PlayerDesc : public cocos2d::Ref
{
public:
    ~PlayerDesc();
    static PlayerDesc* createEmpty();
    DX_CREATE_FROM_JSON_FUNC(PlayerDesc)
    DX_CREATE_FROM_LUA_FUNC(PlayerDesc)
    const std::string& getLabel() const;
    void setVehicleDesc(VehicleDesc* vd);
    VehicleDesc* getVehicleDesc();
    void setMaxWeaponSlots(size_t num);
    void replaceWeaponDesc(size_t idx, WeaponDesc* desc);
    void addWeaponDesc(WeaponDesc* desc);
    const cocos2d::Vector<WeaponDesc*>& getWeapons();
    void reset();
protected:
    bool initFromJson(const rapidjson::Value& el);
    bool initFromLua(lua_State* L, int idx);
    std::string m_label;
    VehicleDesc* m_vehicleDesc = nullptr;
    cocos2d::Vector<WeaponDesc*> m_weapons;
};

#endif // PLAYERDESC_H
