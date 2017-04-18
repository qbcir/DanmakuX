#include "PlayerObject.h"
#include "../Data/PlayerDesc.h"

PlayerObject* PlayerObject::createFromDesc(PlayerDesc* desc) {
    auto po = new (std::nothrow) PlayerObject();
    if (po && po->initFromDesc(desc)) {
        po->autorelease();
        return po;
    }
    CC_SAFE_DELETE(po);
    return nullptr;
}

bool PlayerObject::initFromDesc(PlayerDesc* desc) {
    auto vehicleDesc = desc->getVehicleDesc();
    CCASSERT(vehicleDesc != nullptr, "PlayerDesc.getVehicleDesc() == nullptr");
    if (!vehicleDesc) {
        return false;
    }
    if (!Vehicle::initFromDesc(vehicleDesc)) {
        return false;
    }
    m_playerDesc = desc;
    m_playerDesc->retain();
    this->setCollisionMask(0);
    this->setCollisionClass(COLLISION_CLASS_PLAYER);
    this->enableCollisionsWith(COLLISION_CLASS_ENEMY_BULLET);
    this->enableCollisionsWith(COLLISION_CLASS_ENEMY);
    auto& weapons = m_playerDesc->getWeapons();
    for (auto i = 0; i < weapons.size(); i++) {
        m_weaponActionDescs.pushBack(weapons.at(i)->getActionDesc());
        m_fireActions.push_back(nullptr);
    }
    return true;
}

PlayerObject::~PlayerObject() {
    CC_SAFE_RELEASE(m_playerDesc);
}

void PlayerObject::update(float dt) {
    Vehicle::update(dt);
    auto pos = getPosition();
    pos.clamp(m_minPos, m_maxPos);
    setPosition(pos);
}

void PlayerObject::switchNextWeapon() {
    m_currWeaponIdx++;
    if (m_currWeaponIdx >= m_fireActions.size()) {
        m_currWeaponIdx = 0;
    }
}

void PlayerObject::switchPrevWeapon() {
    if (m_fireActions.size() == 0) {
        return;
    }
    if (m_currWeaponIdx == 0) {
        m_currWeaponIdx = m_fireActions.size();
    }
    m_currWeaponIdx--;
}

void PlayerObject::fire() {
    auto currFireAction = m_fireActions.at(m_currWeaponIdx);
    if (!currFireAction || currFireAction->isDone()) {
        auto fireAction = m_weaponActionDescs.at(m_currWeaponIdx)->getAction();
        fireAction->retain();
        if (m_fireActions.at(m_currWeaponIdx)) {
            m_fireActions.at(m_currWeaponIdx)->release();
        }
        m_fireActions.at(m_currWeaponIdx) = fireAction;
        this->runAction(fireAction);
    }
}

void PlayerObject::onCollision(GameObject *obj) {
    Vehicle::onCollision(obj);
}

PlayerDesc* PlayerObject::getPlayerDesc() {
    return m_playerDesc;
}

void PlayerObject::addOnPlayerDestroyCb(const OnPlayerDestroyCallback& cb) {
    m_onPlayerDestroyCb = cb;
}

void PlayerObject::setDead(bool d) {
    if (d) {
        destroyEvent();
    }
    Vehicle::setDead(d);
}

void PlayerObject::destroyEvent() {
    if (m_onPlayerDestroyCb) {
        this->retain();
        m_onPlayerDestroyCb(this);
        this->release();
    }
}

void PlayerObject::setMinPos(float x, float y) {
    m_minPos.x = x;
    m_minPos.y = y;
}

void PlayerObject::setMaxPos(float x, float y) {
    m_maxPos.x = x;
    m_maxPos.y = y;
}

float PlayerObject::getDirection() const {
    return 90;
}
