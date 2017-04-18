#include "InputComponent.h"
#include "InputManager.h"
#include "Objects/PlayerObject.h"

bool InputComponent::init() {
    if (!Component::init()) {
        return false;
    }
    m_keyboardManager = KeyboardManager::getInstance();
    m_mouseManager = MouseManager::getInstance();
    return true;
}

void InputComponent::update(float dt) {
    updateKeyboardInput(dt);
    if (m_useMouse) {
        updateMouseInput(dt);
    }
}

void InputComponent::updateMouseInput(float dt) {
    auto owner = static_cast<PlayerObject*>(this->getOwner());
    auto objPos = owner->getPosition();
    auto cursorPos = m_mouseManager->getLocation();
    auto delta = cursorPos - objPos;
    auto dist = delta.length();
    auto speed = std::min(dist / dt, 150.0f);
    delta.normalize();
    delta *= speed;
    owner->setVelocity(delta);
    if (m_mouseManager->isButtonPressed(0)) {
        owner->fire();
    }
}

const float DIRECTION_N_ANGLE = 0;
std::vector<float> direction2angle {
    CC_DEGREES_TO_RADIANS(DIRECTION_N_ANGLE),       // N
    CC_DEGREES_TO_RADIANS(DIRECTION_N_ANGLE - 180), // S
    CC_DEGREES_TO_RADIANS(DIRECTION_N_ANGLE - 90),  // E
    CC_DEGREES_TO_RADIANS(DIRECTION_N_ANGLE + 90),  // W
    CC_DEGREES_TO_RADIANS(DIRECTION_N_ANGLE - 45),  // NE
    CC_DEGREES_TO_RADIANS(DIRECTION_N_ANGLE + 45),  // NW
    CC_DEGREES_TO_RADIANS(DIRECTION_N_ANGLE - 135), // SE
    CC_DEGREES_TO_RADIANS(DIRECTION_N_ANGLE + 135)  // SW
};

void InputComponent::updateKeyboardInput(float dt) {
    m_currInterval += dt;
    if (m_currInterval < m_interval) {
        return;
    } else {
        m_currInterval = 0;
    }
    auto owner = static_cast<PlayerObject*>(this->getOwner());
    if (!m_useMouse) {
        auto direction = m_keyboardManager->getDirection();
        if (direction != Direction::INVALID) {
            cocos2d::Vec2 speed(0, 30);
            speed.rotate(cocos2d::Vec2::ZERO, direction2angle[static_cast<size_t>(direction)]);
            owner->setVelocity(speed);
        } else {
            owner->setVelocity(cocos2d::Vec2::ZERO);
        }
        if (m_keyboardManager->readKeyBounced(InputKey::PREV_WEAPON)) {
            owner->switchPrevWeapon();
        } else if (m_keyboardManager->readKeyBounced(InputKey::NEXT_WEAPON)) {
            owner->switchNextWeapon();
        }
        if (m_keyboardManager->isKeyPressed(InputKey::FIRE)) {
            owner->fire();
        }
    }
}

