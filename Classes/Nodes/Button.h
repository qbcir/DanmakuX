#ifndef __dx_BUTTON_H__
#define __dx_BUTTON_H__

#include "cocos2d.h"

class Button : public cocos2d::Node {
public:
    static Button* createWithSprites(
        const std::string& defaultSprite,
        const std::string& pressedSprite,
        const std::string& disabledSprite,
        const std::string& activeSprite);
    bool onTouchBegan(cocos2d::Touch* t, cocos2d::Event* evt);
    void onTouchMoved(cocos2d::Touch* t, cocos2d::Event* evt);
    void onTouchEnded(cocos2d::Touch* t, cocos2d::Event* evt);
    void onTouchCancelled(cocos2d::Touch* t, cocos2d::Event* evt);
    void setHoldable(bool h);
    void setToggleable(bool t);
    bool getPressed() const;
    void updateSprites();
protected:
    bool initWithSprites(
            const std::string& defaultSprite,
            const std::string& pressedSprite,
            const std::string& disabledSprite,
            const std::string& activeSprite);
    bool initWithSprites(
            cocos2d::Sprite* defaultSprite,
            cocos2d::Sprite* pressedSprite,
            cocos2d::Sprite* disabledSprite,
            cocos2d::Sprite* activeSprite);
    void setRadius(float r);
    float m_radius = 0;
    float m_radiusSquared = 0;
    bool m_enabled = true;
    bool m_active = false;
    bool m_holdable = true;
    bool m_toggleable = false;
    bool m_pressed = false;
    cocos2d::Sprite* m_defaultSprite = nullptr;
    cocos2d::Sprite* m_activeSprite = nullptr;
    cocos2d::Sprite* m_disabledSprite = nullptr;
    cocos2d::Sprite* m_pressedSprite = nullptr;
};

#endif // __dx_BUTTON_H__
