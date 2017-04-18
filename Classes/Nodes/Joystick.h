#ifndef JOYSTICK_H
#define JOYSTICK_H

#include "cocos2d.h"

class Joystick : public cocos2d::Node {
public:
    static Joystick* create(const std::string bgSprite,
                            const std::string thumbSprite);
    const cocos2d::Vec2& getVelocity() const;
    const cocos2d::Vec2& getStickPos() const;
    float getDirection() const;
    bool onTouchBegan(cocos2d::Touch* t, cocos2d::Event* evt);
    void onTouchMoved(cocos2d::Touch* t, cocos2d::Event* evt);
    void onTouchEnded(cocos2d::Touch* t, cocos2d::Event* evt);
    void onTouchCancelled(cocos2d::Touch* t, cocos2d::Event* evt);
protected:
    bool initWithSprites(const std::string bgSprite,
                         const std::string thumbSprite);
    cocos2d::Vec2 getTouchNodeSpacePosition(cocos2d::Touch* t);
    void setJoystickRadius(float r);
    void setThumbRadius(float r);
    void setDeadRadius(float r);
    void updateVelocity(const cocos2d::Vec2& pos);
    cocos2d::Director* m_director = nullptr;
    cocos2d::Sprite* m_bgSprite = nullptr;
    cocos2d::Sprite* m_thumbSprite = nullptr;
    cocos2d::Vec2 m_stickPos;
    cocos2d::Vec2 m_velocity;
    bool m_autoCenter = true;
    float m_direction = 0;
    float m_joystickRadius = 0;
    float m_joystickRadiusSquared = 0;
    float m_thumbRadius = 0;
    float m_thumbRadiusSquared = 0;
    float m_deadRadius = 0;
    float m_deadRadiusSquared = 0;
};

#endif // JOYSTICK_H
