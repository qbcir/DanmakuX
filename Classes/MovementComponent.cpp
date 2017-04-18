#include "MovementComponent.h"
#include "Objects/GameObject.h"

bool MovementComponent::initWithTarget(GameObject *tgt) {
    if (!tgt) {
        return false;
    }
    m_tgt = tgt;
    CC_SAFE_RETAIN(m_tgt);
    return true;
}

MovementComponent::~MovementComponent() {
    CC_SAFE_RELEASE(m_tgt);
}

void MovementComponent::setTarget(GameObject* tgt) {
    CC_SAFE_RELEASE(m_tgt);
    m_tgt = tgt;
    CC_SAFE_RETAIN(m_tgt);
}

GameObject* MovementComponent::getTarget() {
    return m_tgt;
}

void TurretComponent::update(float dt) {
    auto owner = static_cast<GameObject*>(this->getOwner());
    auto actor = static_cast<GameObject*>(owner->getParent());
    cocos2d::Vec2 vel = m_tgt->getPosition() - actor->getPosition();
    float newRotation = -CC_RADIANS_TO_DEGREES(vel.getAngle()) - 90;
    float rotation;
    float currRot = owner->getRotation();
    if(newRotation < -180 && currRot > 0)
        rotation = ((newRotation + 360) - currRot);
    else if(newRotation > 0 && currRot < -180)
        rotation = (newRotation - (currRot + 360));
    else
        rotation = (newRotation - currRot);
    auto rotationSize = std::abs(rotation);
    float tgtRotation;
    float maxRotation;
    if (owner->hasMaxRotation()) {
        maxRotation = owner->getMaxRotation();
    } else {
        maxRotation = rotationSize;
    }
    if (rotationSize > m_slowRadius) {
        tgtRotation = maxRotation;
    } else {
        tgtRotation = maxRotation * rotationSize / m_slowRadius;
    }
    if (std::signbit(rotation)) {
        tgtRotation = -tgtRotation;
    }
    cocos2d::log("tgtRotation=%f rotationSize=%f", tgtRotation, rotationSize);
    owner->setAngularSpeed(tgtRotation);
}

void AlignComponent::update(float dt) {
    auto owner = static_cast<GameObject*>(this->getOwner());
    cocos2d::Vec2 vel = m_tgt->getPosition() - owner->getPosition();
    auto rotation = CC_RADIANS_TO_DEGREES(vel.getAngle(owner->getVelocity()));
    auto rotationSize = std::abs(rotation);
    float tgtRotation;
    float maxRotation;
    if (owner->hasMaxRotation()) {
        maxRotation = owner->getMaxRotation();
    } else {
        maxRotation = rotationSize;
    }
    if (rotationSize > m_slowRadius) {
        tgtRotation = maxRotation;
    } else {
        tgtRotation = maxRotation * rotationSize / m_slowRadius;
    }
    if (std::signbit(rotation)) {
        tgtRotation = -tgtRotation;
    }
    //cocos2d::log("tgtRotation=%f rotationSize=%f", tgtRotation, rotationSize);
    owner->setAngularSpeed(tgtRotation);
}


bool KinematicArriveComponent::initWithTarget(GameObject *tgt) {
    if (!MovementComponent::initWithTarget(tgt)) {
        return false;
    }
    auto bbox = cocos2d::utils::getCascadeBoundingBox(tgt);
    m_radius = bbox.size.width / 2;
    return true;
}

void KinematicArriveComponent::setRadius(float r) {
    m_radius = r;
    m_radiusSquared = r * r;
}

void KinematicArriveComponent::update(float dt) {
    auto owner = static_cast<GameObject*>(this->getOwner());
    cocos2d::Vec2 vel = m_tgt->getPosition() - owner->getPosition();
    if (vel.lengthSquared() < m_radiusSquared) {
        return;
    }
    vel.x /= m_timeToTgt;
    vel.y /= m_timeToTgt;
    if (vel.lengthSquared() > owner->getMaxSpeedSquared()) {
        vel.normalize();
        vel *= owner->getMaxSpeed();
    }
    owner->setVelocity(vel);
}

void KinematicArriveXComponent::setRadius(float r) {
    m_radius = r;
}

void KinematicArriveXComponent::update(float dt) {
    auto owner = static_cast<GameObject*>(this->getOwner());
    float dx = m_tgt->getPositionX() - owner->getPositionX();
    if (std::abs(dx) < m_radius) {
        return;
    }
    dx /= m_timeToTgt;
    auto vel = owner->getVelocity();
    vel.x = dx;
    if (vel.lengthSquared() > owner->getMaxSpeedSquared()) {
        vel.x = (dx < 0 ? -1 : 1) * std::sqrt(owner->getMaxSpeedSquared() - vel.y * vel.y);
    }
    owner->setVelocity(vel);
}

void KinematicArriveYComponent::setRadius(float r) {
    m_radius = r;
}

void KinematicArriveYComponent::update(float dt) {
    auto owner = static_cast<GameObject*>(this->getOwner());
    float dy = m_tgt->getPositionY() - owner->getPositionY();
    if (std::abs(dy) < m_radius) {
        return;
    }
    dy /= m_timeToTgt;
    auto vel = owner->getVelocity();
    vel.y = dy;
    if (vel.lengthSquared() > owner->getMaxSpeedSquared()) {
        vel.y = (dy < 0 ? -1 : 1) * std::sqrt(owner->getMaxSpeedSquared() - vel.x * vel.x);
    }
    owner->setVelocity(vel);
}
