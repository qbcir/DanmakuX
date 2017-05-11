#include "InputPacketComponent.h"
#include "KeyboardManager.h"
#include "MouseManager.h"
#include "../GameManager.h"

bool InputPacketComponent::init() {
    if (!Component::init()) {
        return false;
    }
    m_gameManager = GameManager::getInstance();
    m_keyboardManager = KeyboardManager::getInstance();
    m_mouseManager = MouseManager::getInstance();
    m_lastSample = m_gameManager->getTime();
    return true;
}

void InputPacketComponent::update(float dt) {
    auto kbdState = m_keyboardManager->getKeysPressed();
    auto mouseState = m_mouseManager->getButtonsPressed();
    m_currInterval += dt;
    if (m_currInterval < m_sampleInterval) {
        return;
    }
    m_currInterval = 0.f;
    float t = m_gameManager->getTime();
    InputPacket ip(kbdState, mouseState, t, t - m_lastSample);
    m_lastSample = t;
    m_inputPackets.add(ip);
}

InputPacketList& InputPacketComponent::getInputPacketList() {
    return m_inputPackets;
}

void InputPacketComponent::processPackets() {
    for (auto& p : m_inputPackets.getInputPackets()) {
        processPacket(p);
    }
}

void InputPacketComponent::processPacket(const InputPacket &p) {
    auto po = static_cast<GameObject*>(this->getOwner());
    float rot = 0;
    float currRot = po->getRotation();
    if (p.isKeyPressed(InputKey::LEFT)) {
        rot = -20;
    } else if (p.isKeyPressed(InputKey::RIGHT)) {
        rot = 20;
    }
    po->setAngularSpeed(rot);
    if (p.isKeyPressed(InputKey::UP)) {
        cocos2d::Vec2 speed(0, 30);
        speed.rotate(cocos2d::Vec2::ZERO, -CC_DEGREES_TO_RADIANS(currRot));
        po->setVelocity(-speed);
    } else if (p.isKeyPressed(InputKey::DOWN)) {
        cocos2d::Vec2 speed(0, 30);
        speed.rotate(cocos2d::Vec2::ZERO, -CC_DEGREES_TO_RADIANS(currRot));
        po->setVelocity(speed);
    } else {
        po->setVelocity(cocos2d::Vec2::ZERO);
    }
    po->update(p.delta);
}
