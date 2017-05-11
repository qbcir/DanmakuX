#include "InputManager.h"
#include "GameManager.h"
#include "Objects/GameObject.h"

InputManager* InputManager::getInstance() {
  return GameManager::getInstance()->getInputManager();
}

void InputManager::startKeyboardInputWithNode(cocos2d::Node* node) {
    m_keyboardManager.startWithNode(node);
}

void InputManager::startMouseInputWithNode(cocos2d::Node* node) {
    m_mouseManager.startWithNode(node);
}

KeyboardManager* InputManager::getKeyboardManager() {
    return &m_keyboardManager;
}

MouseManager* InputManager::getMouseManager() {
    return &m_mouseManager;
}

void InputManager::sample() {
    auto kbdState = m_keyboardManager.getKeysPressed();
    auto mouseState = m_mouseManager.getButtonsPressed();
    auto gm = GameManager::getInstance();
    float t = gm->getTime();
    InputPacket ip(kbdState, mouseState, t, t - m_lastSample);
    m_lastSample = t;
    if (m_inputPackets.add(ip)) {
        auto& last = m_inputPackets.getInputPackets().back();
        m_pendingInput = &last;
    }
}

void InputManager::removeInputPacketsByTimestamp(float ts) {
    m_inputPackets.removeProcessedInputs(ts);
}

InputPacketList& InputManager::getInputPacketList() {
    return m_inputPackets;
}

InputPacket* InputManager::getPendingInput() {
    auto res = m_pendingInput;
    m_pendingInput = nullptr;
    return res;
}
