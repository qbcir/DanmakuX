#include "PlayerObject.h"
#include "../Data/PlayerDesc.h"
#include "../Data/DataParser.h"
#include "../InputManager.h"
#include "../GameManager.h"

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
    //pos.clamp(m_minPos, m_maxPos);
    setPosition(pos);
    if (m_lastInputPacket) {
        m_lastInputPacket->delta += dt;
    }
    auto im = InputManager::getInstance();
    auto pp = im->getPendingInput();
    if (pp) {
        if (m_lastInputPacket) {
            m_lastInputPacket->processed = true;
        }
        m_lastInputPacket = pp;
        auto& p = *pp;
        float rot = 0;
        float currRot = this->getRotation();
        if (p.isKeyPressed(InputKey::LEFT)) {
            rot = -20;
        } else if (p.isKeyPressed(InputKey::RIGHT)) {
            rot = 20;
        }
        this->setAngularSpeed(rot);
        if (p.isKeyPressed(InputKey::UP)) {
            cocos2d::Vec2 speed(0, 30);
            speed.rotate(cocos2d::Vec2::ZERO, -CC_DEGREES_TO_RADIANS(currRot));
            this->setVelocity(-speed);
        } else if (p.isKeyPressed(InputKey::DOWN)) {
            cocos2d::Vec2 speed(0, 30);
            speed.rotate(cocos2d::Vec2::ZERO, -CC_DEGREES_TO_RADIANS(currRot));
            this->setVelocity(speed);
        } else {
            this->setVelocity(cocos2d::Vec2::ZERO);
        }
    }
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

GameObjectState PlayerObject::readStateBytes(uint8_t* &p) {
    auto old = GameObject::readStateBytes(p);
    auto pos = getPosition();
    auto rot = getRotation();
    /*
    auto im = InputManager::getInstance();
    auto& ipl = im->getInputPacketList();
    auto& uipl = ipl.getInputPackets();
    for (auto& p : uipl) {
        //cocos2d::log("replay ts=%f dt=%f", p.timestamp, p.delta);
        float angV = 0;
        if (p.isKeyPressed(InputKey::LEFT)) {
            angV = -20;
        } else if (p.isKeyPressed(InputKey::RIGHT)) {
            angV = 20;
        }
        this->setAngularSpeed(angV);
        if (p.isKeyPressed(InputKey::UP)) {
            cocos2d::Vec2 speed(0, 30);
            speed.rotate(cocos2d::Vec2::ZERO, -CC_DEGREES_TO_RADIANS(rot));
            this->setVelocity(-speed);
        } else if (p.isKeyPressed(InputKey::DOWN)) {
            cocos2d::Vec2 speed(0, 30);
            speed.rotate(cocos2d::Vec2::ZERO, -CC_DEGREES_TO_RADIANS(rot));
            this->setVelocity(speed);
        } else {
            this->setVelocity(cocos2d::Vec2::ZERO);
        }
        pos += m_velocity * p.delta;
        rot += m_rotation * p.delta;
        //cocos2d::log("ts=%f dt=%f replay pos.x=%f pos.y=%f", p.timestamp, p.delta, pos.x, pos.y);
    }
    */
    setPosition(pos);
    setRotation(rot);
    return old;
}
