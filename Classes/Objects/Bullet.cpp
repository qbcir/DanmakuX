#include "Bullet.h"
#include "../Data/BulletDesc.h"

Bullet* Bullet::createFromDesc(BulletDesc* desc) {
    auto b = new (std::nothrow) Bullet();
    if (b && b->initFromDesc(desc)) {
        b->autorelease();
        return b;
    }
    CC_SAFE_DELETE(b);
    return nullptr;
}

bool Bullet::initFromDesc(BulletDesc* desc) {
    if (!GameObject::initFromDesc(desc)) {
        return false;
    }
    if (desc->isEnemy()) {
        this->setCollisionClass(COLLISION_CLASS_ENEMY_BULLET);
        this->enableCollisionsWith(COLLISION_CLASS_PLAYER);
    } else {
        this->setCollisionClass(COLLISION_CLASS_PLAYER_BULLET);
        this->enableCollisionsWith(COLLISION_CLASS_ENEMY);
    }
    m_bulletDesc = desc;
    m_bulletDesc->retain();
    return true;
}

Bullet::~Bullet() {
    CC_SAFE_RELEASE(m_bulletDesc);
}

void Bullet::onCollision(GameObject *obj) {
    this->setDead(true);
}

BulletDesc* Bullet::getBulletDesc() {
    return m_bulletDesc;
}
