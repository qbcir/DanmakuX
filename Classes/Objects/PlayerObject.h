#ifndef PLAYER_H
#define PLAYER_H

#include "Vehicle.h"
#include "GameAction.h"

class InputPacket;
class PlayerDesc;
class PlayerObject : public Vehicle {
public:
    typedef std::function<void(cocos2d::Ref*)> OnPlayerDestroyCallback;
    static PlayerObject* createFromDesc(PlayerDesc* desc);
    virtual GameObjectState readStateBytes(uint8_t* &p);
    ~PlayerObject();
    void update(float dt);
    void fire();
    void setMinPos(float x, float y);
    void setMaxPos(float x, float y);
    void switchNextWeapon();
    void switchPrevWeapon();
    void onCollision(GameObject *obj);
    PlayerDesc* getPlayerDesc();
    void addOnPlayerDestroyCb(const OnPlayerDestroyCallback& cb);
    void setDead(bool d);
    float getDirection() const;
protected:
    void destroyEvent();
    bool initFromDesc(PlayerDesc* desc);
    size_t m_currWeaponIdx = 0;
    PlayerDesc* m_playerDesc = nullptr;
    cocos2d::Vector<ActionDesc*> m_weaponActionDescs;
    std::vector<cocos2d::Action*> m_fireActions;
    OnPlayerDestroyCallback m_onPlayerDestroyCb;
    cocos2d::Vec2 m_minPos;
    cocos2d::Vec2 m_maxPos;
    InputPacket* m_lastInputPacket = nullptr;
};

#endif // PLAYER_H
