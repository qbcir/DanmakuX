#include "Vehicle.h"
#include "Bullet.h"
#include "../Data/VehicleDesc.h"
#include "../Data/BulletDesc.h"
#include "ObjectManager.h"

Vehicle* Vehicle::createFromDesc(VehicleDesc* desc) {
    auto v = new (std::nothrow) Vehicle();
    if (v && v->initFromDesc(desc)) {
        v->autorelease();
        return v;
    }
    CC_SAFE_DELETE(v);
    return nullptr;
}

bool Vehicle::initFromDesc(VehicleDesc* desc) {
    if (!GameObject::initFromDesc(desc)) {
        return false;
    }
    m_vehicleDesc = desc;
    m_vehicleDesc->retain();
    m_health = desc->getHealth();
    this->setCollisionClass(COLLISION_CLASS_ENEMY);
    this->enableCollisionsWith(COLLISION_CLASS_PLAYER);
    this->enableCollisionsWith(COLLISION_CLASS_PLAYER_BULLET);
    return true;
}

Vehicle::~Vehicle() {
    CC_SAFE_RELEASE(m_vehicleDesc);
}

void Vehicle::onCollision(GameObject *obj) {
    if (auto b = dynamic_cast<Bullet*>(obj)) {
        auto bDesc = b->getBulletDesc();
        auto damage = bDesc->getDamage();
        m_healthDec = damage;
    } else if (auto v = dynamic_cast<Vehicle*>(obj)) {
        auto vDesc = v->getVehicleDesc();
        auto vDamage = vDesc->getDamage();
        if (vDamage != 0) {
            m_healthDec = vDamage;
        } else {
            auto vHealth = v->getHealth();
            m_healthDec = vHealth;
        }
    }
}

void Vehicle::update(float dt) {
    if (m_healthDec) {
        m_health -= m_healthDec;
        m_healthDec = 0;
        healthChangeEvent();
    }
    if (m_health <= 0) {
        setDead(true);
    }
    GameObject::update(dt);
}

VehicleDesc* Vehicle::getVehicleDesc() {
    return m_vehicleDesc;
}

float Vehicle::getHealth() const {
    return m_health;
}

void Vehicle::setHealthDecrement(float healthDec) {
    m_healthDec = healthDec;
}

void Vehicle::setDead(bool d) {
    if (d) {
        destroyEvent();
        auto objMgr = ObjectManager::getInstance();
        auto pDesc = objMgr->getParticleDesc("default");
        auto p = pDesc->createPSQ();
        p->setPosition(getPosition());
        auto ll = this->getParent();
        ll->addChild(p);
    }
    GameObject::setDead(d);
}

void Vehicle::destroyEvent() {
    if (m_onDestroyCb) {
        this->retain();
        m_onDestroyCb(this);
        this->release();
    }
}

void Vehicle::healthChangeEvent() {
    if (m_onHealthChangeCb) {
        this->retain();
        m_onHealthChangeCb(this);
        this->release();
    }
}

void Vehicle::addOnHealthChangeCb(const OnHealthChangeCbT& cb) {
    m_onHealthChangeCb = cb;
}
