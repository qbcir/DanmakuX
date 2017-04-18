#include "BaseInputComponent.h"
#include "../Objects/PlayerObject.h"
#include "KeyboardManager.h"
#include "MouseManager.h"

bool BaseInputComponent::init() {
    if (!Component::init()) {
        return false;
    }
    m_keyboardManager = KeyboardManager::getInstance();
    m_mouseManager = MouseManager::getInstance();
    return true;
}

PlayerObject* BaseInputComponent::getPlayerObject() {
    return static_cast<PlayerObject*>(this->getOwner());
}

const cocos2d::Vec2& BaseInputComponent::getCursorPosition() {
    return m_mouseManager->getLocation();
}

void BaseInputComponent::update(float dt) {
}
