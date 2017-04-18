#ifndef KEYBOARDMANAGER_H
#define KEYBOARDMANAGER_H

#include "cocos2d.h"

enum class InputKey {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    FIRE,
    PREV_WEAPON,
    NEXT_WEAPON,
    PAUSE,
    MAX,
    INVALID = -1
};

enum class Direction {
    N, S, E, W,
    NE, NW, SE, SW,
    INVALID = -1
};

namespace std {
    template<>
    struct hash<cocos2d::EventKeyboard::KeyCode> {
        size_t operator()(cocos2d::EventKeyboard::KeyCode kc) const {
            return m_hasher(static_cast<int>(kc));
        }
        hash<int> m_hasher;
    };
}

class KeyboardManager {
public:
    typedef std::function<void()> OnKeyUpCbT;
    typedef std::function<void()> OnKeyDownCbT;
    static KeyboardManager* getInstance();
    KeyboardManager();
    void startWithNode(cocos2d::Node* node);
    bool anyKeyPressed() const;
    uint64_t getKeysPressed() const;
    bool isKeyPressed(InputKey key) const;
    void setKeyPressed(InputKey key);
    void setKeyReleased(InputKey key);
    bool readKeyBounced(InputKey key);
    void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    Direction getDirection() const;
    void addOnKeyUpCb(InputKey key, const OnKeyUpCbT& cb);
    void addOnKeyDownCb(InputKey key, const OnKeyDownCbT& cb);
protected:
    uint64_t m_keysPressed = 0;
    uint64_t m_keysBounced = 0;
    std::unordered_map<cocos2d::EventKeyboard::KeyCode, InputKey> m_keyMap;
    std::vector<OnKeyUpCbT> m_onKeyUpCbs;
    std::vector<OnKeyDownCbT> m_onKeyDownCbs;
};

#endif // KEYBOARDMANAGER_H
