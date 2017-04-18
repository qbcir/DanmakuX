#ifndef __dx_BASEINPUTCOMPONENT_H__
#define __dx_BASEINPUTCOMPONENT_H__

#include "cocos2d.h"

class PlayerObject;
class KeyboardManager;
class MouseManager;
class BaseInputComponent : public cocos2d::Component {
public:
    bool init();
    void update(float dt);
    PlayerObject* getPlayerObject();
    const cocos2d::Vec2& getCursorPosition();
protected:
    KeyboardManager* m_keyboardManager = nullptr;
    MouseManager* m_mouseManager = nullptr;
};

#endif // __dx_BASEINPUTCOMPONENT_H__
