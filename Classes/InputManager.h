#ifndef __danmakuX_InputManager_h__
#define __danmakuX_InputManager_h__

#include "cocos2d.h"
#include "Input/MouseManager.h"
#include "Input/KeyboardManager.h"
#include "Network/InputPacketList.h"

class InputManager {
public:
    static InputManager* getInstance();
    KeyboardManager* getKeyboardManager();
    MouseManager* getMouseManager();
    void startKeyboardInputWithNode(cocos2d::Node* node);
    void startMouseInputWithNode(cocos2d::Node* node);
    void sample();
    void removeInputPacketsByTimestamp(float ts);
    InputPacketList& getInputPacketList();
    InputPacket* getPendingInput();
protected:
    float m_lastSample = 0.f;
    InputPacket* m_pendingInput = nullptr;
    KeyboardManager m_keyboardManager;
    MouseManager m_mouseManager;
    InputPacketList m_inputPackets;
};

#endif /* __danmakuX_InputManager_h__ */
