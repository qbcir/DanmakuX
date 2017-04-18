#include "Joystick.h"

Joystick* Joystick::create(
        const std::string bgSprite,
        const std::string thumbSprite) {
    auto j = new (std::nothrow) Joystick();
    if (j && j->initWithSprites(bgSprite, thumbSprite)) {
        j->autorelease();
        return j;
    }
    CC_SAFE_DELETE(j);
    return nullptr;
}

bool Joystick::initWithSprites(
        const std::string bgSprite,
        const std::string thumbSprite) {
    m_director = cocos2d::Director::getInstance();
    m_bgSprite = cocos2d::Sprite::create(bgSprite);
    m_thumbSprite = cocos2d::Sprite::create(thumbSprite);
    if (!m_bgSprite || !m_thumbSprite) {
        return false;
    }
    this->addChild(m_bgSprite, 0);
    this->setContentSize(m_bgSprite->getContentSize());
    this->setJoystickRadius(m_bgSprite->getBoundingBox().size.width / 2);
    this->addChild(m_thumbSprite, 1);
    this->setThumbRadius(m_thumbSprite->getBoundingBox().size.width / 2);
    auto evtListener = cocos2d::EventListenerTouchOneByOne::create();
    evtListener->setSwallowTouches(true);
    evtListener->onTouchBegan = CC_CALLBACK_2(Joystick::onTouchBegan, this);
    evtListener->onTouchMoved = CC_CALLBACK_2(Joystick::onTouchMoved, this);;
    evtListener->onTouchEnded = CC_CALLBACK_2(Joystick::onTouchEnded, this);;
    evtListener->onTouchCancelled = CC_CALLBACK_2(Joystick::onTouchCancelled, this);;
    _eventDispatcher->addEventListenerWithSceneGraphPriority(evtListener, this);
    return true;
}

void Joystick::setJoystickRadius(float r) {
    m_joystickRadius = r;
    m_joystickRadiusSquared = r * r;
}

void Joystick::setThumbRadius(float r) {
    m_thumbRadius = r;
    m_thumbRadiusSquared = r * r;
}

void Joystick::setDeadRadius(float r) {
    m_deadRadius = r;
    m_deadRadiusSquared = r * r;
}

const cocos2d::Vec2& Joystick::getVelocity() const {
    return m_velocity;
}

const cocos2d::Vec2& Joystick::getStickPos() const {
    return m_stickPos;
}

float Joystick::getDirection() const {
    return m_direction;
}

cocos2d::Vec2 Joystick::getTouchNodeSpacePosition(cocos2d::Touch* t) {
    auto pos = m_director->convertToGL(t->getLocationInView());
    return this->convertToNodeSpace(pos);
}

bool Joystick::onTouchBegan(cocos2d::Touch* t, cocos2d::Event* evt) {
    auto tgt = static_cast<Joystick*>(evt->getCurrentTarget());
    auto pos = tgt->getTouchNodeSpacePosition(t);
    if (pos.x < -tgt->m_joystickRadius
            || pos.x > tgt->m_joystickRadius
            || pos.y < -tgt->m_joystickRadius
            || pos.y > tgt->m_joystickRadius) {
        return false;
    } else if (tgt->m_joystickRadiusSquared > pos.lengthSquared()) {
        tgt->updateVelocity(pos);
        return true;
    }
    return false;
}

void Joystick::onTouchMoved(cocos2d::Touch* t, cocos2d::Event* evt) {
    auto tgt = static_cast<Joystick*>(evt->getCurrentTarget());
    auto pos = tgt->getTouchNodeSpacePosition(t);
    tgt->updateVelocity(pos);
}

void Joystick::onTouchEnded(cocos2d::Touch* t, cocos2d::Event* evt) {
    auto tgt = static_cast<Joystick*>(evt->getCurrentTarget());
    auto pos = cocos2d::Vec2::ZERO;
    if (!tgt->m_autoCenter) {
        pos = tgt->getTouchNodeSpacePosition(t);
    }
    cocos2d::log("pos %f %f", pos.x, pos.y);
    tgt->updateVelocity(pos);
}

void Joystick::onTouchCancelled(cocos2d::Touch* t, cocos2d::Event* evt) {
    auto tgt = static_cast<Joystick*>(evt->getCurrentTarget());
    tgt->onTouchEnded(t, evt);
}

void Joystick::updateVelocity(const cocos2d::Vec2& pos) {
    auto distSq = pos.lengthSquared();
    if (distSq <= m_deadRadiusSquared) {
        m_stickPos = pos;
        m_velocity = cocos2d::Vec2::ZERO;
        m_direction = 0.0f;
    } else {
        float angle = std::atan2(pos.y, pos.x);
        if (distSq < m_joystickRadiusSquared) {
            m_stickPos.x = m_joystickRadius * std::cos(angle);
            m_stickPos.y = m_joystickRadius * std::sin(angle);
            angle += CC_DEGREES_TO_RADIANS(90);
        } else {
            m_stickPos = cocos2d::Vec2::ZERO;
        }
        m_velocity = m_stickPos / m_joystickRadius;
        m_direction = CC_RADIANS_TO_DEGREES(angle);
    }
    cocos2d::log("v: %f %f", m_velocity.x, m_velocity.y);
    m_thumbSprite->setPosition(m_stickPos);
    m_thumbSprite->setRotation(m_direction);
}
