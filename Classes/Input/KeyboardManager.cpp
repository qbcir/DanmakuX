#include "InputManager.h"

bool KeyboardManager::anyKeyPressed() const {
    return m_keysPressed != 0;
}

uint64_t KeyboardManager::getKeysPressed() const {
    return m_keysPressed;
}

bool KeyboardManager::isKeyPressed(InputKey key) const {
    return (m_keysPressed >> static_cast<uint8_t>(key)) & 1;
}

void KeyboardManager::setKeyPressed(InputKey key) {
    uint64_t mask = (uint64_t(1) << static_cast<uint8_t>(key));
    m_keysPressed |= mask;
    m_keysBounced &= ~mask;
}

void KeyboardManager::setKeyReleased(InputKey key) {
    uint64_t mask = (uint64_t(1) << static_cast<uint8_t>(key));
    m_keysPressed &= ~mask;
    m_keysBounced |= mask;
}

void KeyboardManager::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
    auto it = m_keyMap.find(keyCode);
    if (it != std::end(m_keyMap)) {
        setKeyPressed(it->second);
        auto cb = m_onKeyDownCbs.at(static_cast<size_t>(it->second));
        if (cb) {
            cb();
        }
    }
}

bool KeyboardManager::readKeyBounced(InputKey key) {
    uint64_t mask = (uint64_t(1) << static_cast<uint8_t>(key));
    bool bounced = m_keysBounced & mask;
    m_keysBounced &= ~mask;
    return bounced;
}

void KeyboardManager::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
    auto it = m_keyMap.find(keyCode);
    if (it != std::end(m_keyMap)) {
        setKeyReleased(it->second);
        auto cb = m_onKeyUpCbs.at(static_cast<size_t>(it->second));
        if (cb) {
            cb();
        }
    }
}

void KeyboardManager::startWithNode(cocos2d::Node* node) {
    auto kbdListener = cocos2d::EventListenerKeyboard::create();
    kbdListener->onKeyPressed = CC_CALLBACK_2(KeyboardManager::onKeyPressed, this);
    kbdListener->onKeyReleased = CC_CALLBACK_2(KeyboardManager::onKeyReleased, this);
    auto evtDispatcher = node->getEventDispatcher();
    evtDispatcher->addEventListenerWithSceneGraphPriority(kbdListener, node);
}

Direction KeyboardManager::getDirection() const {
    switch (m_keysPressed & 0xF) {
        case 0x1 : return Direction::N;
        case 0x2 : return Direction::S;
        case 0x4 : return Direction::W;
        case 0x5 : return Direction::NW;
        case 0x6 : return Direction::SW;
        case 0x8 : return Direction::E;
        case 0x9 : return Direction::NE;
        case 0xa : return Direction::SE;
        default: return Direction::INVALID;
    }
}

KeyboardManager::KeyboardManager() {
    m_onKeyUpCbs.resize(static_cast<size_t>(InputKey::MAX));
    m_onKeyDownCbs.resize(static_cast<size_t>(InputKey::MAX));
    m_keyMap[cocos2d::EventKeyboard::KeyCode::KEY_W] = InputKey::UP;
    m_keyMap[cocos2d::EventKeyboard::KeyCode::KEY_A] = InputKey::LEFT;
    m_keyMap[cocos2d::EventKeyboard::KeyCode::KEY_S] = InputKey::DOWN;
    m_keyMap[cocos2d::EventKeyboard::KeyCode::KEY_D] = InputKey::RIGHT;
    m_keyMap[cocos2d::EventKeyboard::KeyCode::KEY_Q] = InputKey::PREV_WEAPON;
    m_keyMap[cocos2d::EventKeyboard::KeyCode::KEY_E] = InputKey::NEXT_WEAPON;
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    m_keyMap[cocos2d::EventKeyboard::KeyCode::KEY_BACK] = InputKey::PAUSE;
#elif CC_TARGET_PLATFORM == CC_PLATFORM_LINUX
    m_keyMap[cocos2d::EventKeyboard::KeyCode::KEY_ESCAPE] = InputKey::PAUSE;
#endif
}

KeyboardManager* KeyboardManager::getInstance() {
    return InputManager::getInstance()->getKeyboardManager();
}

void KeyboardManager::addOnKeyUpCb(InputKey key, const OnKeyUpCbT& cb) {
    m_onKeyUpCbs.at(static_cast<size_t>(key)) = cb;
}

void KeyboardManager::addOnKeyDownCb(InputKey key, const OnKeyDownCbT& cb) {
    m_onKeyDownCbs.at(static_cast<size_t>(key)) = cb;
}
