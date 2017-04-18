#ifndef TOUCHINPUTCOMPONENT_H
#define TOUCHINPUTCOMPONENT_H

#include "cocos2d.h"

class Button;
class Joystick;
class TouchInputComponent : public cocos2d::Component {
public:
    ~TouchInputComponent();
    void update(float dt);
    void setFireButton(Button* b);
    void setJoystick(Joystick* j);
    CREATE_FUNC(TouchInputComponent)
protected:
    void updateFireButton(float dt);
    void updateJoystick(float dt);
    Button* m_fireButton = nullptr;
    Joystick* m_joystick = nullptr;
};

#endif // TOUCHINPUTCOMPONENT_H
