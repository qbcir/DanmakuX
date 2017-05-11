#ifndef __DX_INPUTPACKETCOMPONENT_H__
#define __DX_INPUTPACKETCOMPONENT_H__

#include "cocos2d.h"
#include "../Network/InputPacketList.h"

class KeyboardManager;
class MouseManager;
class GameManager;
class InputPacketComponent : public cocos2d::Component {
public:
    bool init();
    void update(float dt);
    CREATE_FUNC(InputPacketComponent)
    InputPacketList& getInputPacketList();
    void processPackets();
protected:
    void processPacket(const InputPacket &p);
    float m_currInterval = 0;
    float m_sampleInterval = 0.03f;
    float m_lastSample = 0.f;
    GameManager* m_gameManager = nullptr;
    KeyboardManager* m_keyboardManager = nullptr;
    MouseManager* m_mouseManager = nullptr;
    InputPacketList m_inputPackets;
};

#endif /* __DX_INPUTPACKETCOMPONENT_H__ */
