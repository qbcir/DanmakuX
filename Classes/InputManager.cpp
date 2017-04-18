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
