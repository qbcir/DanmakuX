#ifndef __danmakuX_InputManager_h__
#define __danmakuX_InputManager_h__

#include "cocos2d.h"
#include "Input/MouseManager.h"
#include "Input/KeyboardManager.h"

class InputManager {
public:
    static InputManager* getInstance();
    KeyboardManager* getKeyboardManager();
    MouseManager* getMouseManager();
    void startKeyboardInputWithNode(cocos2d::Node* node);
    void startMouseInputWithNode(cocos2d::Node* node);
protected:
    KeyboardManager m_keyboardManager;
    MouseManager m_mouseManager;
};

#endif /* __danmakuX_InputManager_h__ */
