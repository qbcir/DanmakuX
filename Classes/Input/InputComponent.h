#ifndef INPUTCOMPONENT_H
#define INPUTCOMPONENT_H

#include "cocos2d.h"

class KeyboardManager;
class MouseManager;
class InputComponent : public cocos2d::Component {
public:
    bool init();
    void update(float dt);
    CREATE_FUNC(InputComponent)
protected:
    void updateKeyboardInput(float dt);
    void updateMouseInput(float dt);
    float m_interval = 0.1f;
    float m_currInterval = 0.0f;
    bool m_useMouse = true;
    KeyboardManager* m_keyboardManager = nullptr;
    MouseManager* m_mouseManager = nullptr;
};

#endif // INPUTCOMPONENT_H
