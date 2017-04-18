#ifndef MOUSEMANAGER_H
#define MOUSEMANAGER_H

#include "cocos2d.h"

class MouseManager {
public:
    static MouseManager* getInstance();
    void startWithNode(cocos2d::Node* node);
    uint64_t getButtonsPressed() const;
    bool isButtonPressed(int button);
    void onMouseMove(cocos2d::Event* evt);
    void onMouseUp(cocos2d::Event* evt);
    void onMouseDown(cocos2d::Event* evt);
    void onMouseScroll(cocos2d::Event* evt);
    const cocos2d::Vec2& getLocation() const;
    const cocos2d::Vec2& getDelta() const;
protected:
    uint64_t m_buttonPressed = 0;
    cocos2d::Vec2 m_delta;
    cocos2d::Vec2 m_pos;
};

#endif // MOUSEMANAGER_H
