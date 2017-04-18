#ifndef VEHICLE_H
#define VEHICLE_H

#include "GameObject.h"

class VehicleDesc;
class Vehicle : public GameObject
{
public:
    typedef std::function<void(cocos2d::Ref*)> OnDestroyCbT;
    typedef std::function<void(cocos2d::Ref*)> OnHealthChangeCbT;
    static Vehicle* createFromDesc(VehicleDesc* desc);
    virtual ~Vehicle();
    virtual void update(float dt);
    virtual void onCollision(GameObject *obj);
    VehicleDesc* getVehicleDesc();
    float getHealth() const;
    void setHealthDecrement(float healthDec);
    virtual void setDead(bool d);
    virtual void addOnHealthChangeCb(const OnHealthChangeCbT& cb);
protected:
    void destroyEvent();
    void healthChangeEvent();
    bool initFromDesc(VehicleDesc* desc);
    float m_health = 0;
    float m_healthDec = 0;
    bool m_ignoreCollision = false;
    float m_damageOnCollision = 0;
    VehicleDesc* m_vehicleDesc = nullptr;
    OnDestroyCbT m_onDestroyCb;
    OnHealthChangeCbT m_onHealthChangeCb;
};

#endif // VEHICLE_H
