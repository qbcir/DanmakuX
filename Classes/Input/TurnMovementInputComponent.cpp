#include "TurnMovementInputComponent.h"
#include "KeyboardManager.h"
#include "../Objects/PlayerObject.h"

void TurnMovementInputComponent::update(float dt) {
    auto po = getPlayerObject();
    float rot = 0;
    float currRot = po->getRotation();
    if (m_keyboardManager->isKeyPressed(InputKey::LEFT)) {
        rot = -20;
    } else if (m_keyboardManager->isKeyPressed(InputKey::RIGHT)) {
        rot = 20;
    }
    po->setAngularSpeed(rot);
    if (m_keyboardManager->isKeyPressed(InputKey::UP)) {
        cocos2d::Vec2 speed(0, 30);
        speed.rotate(cocos2d::Vec2::ZERO, -CC_DEGREES_TO_RADIANS(currRot));
        po->setVelocity(-speed);
    } else if (m_keyboardManager->isKeyPressed(InputKey::DOWN)) {
        cocos2d::Vec2 speed(0, 30);
        speed.rotate(cocos2d::Vec2::ZERO, -CC_DEGREES_TO_RADIANS(currRot));
        po->setVelocity(speed);
    } else {
        po->setVelocity(cocos2d::Vec2::ZERO);
    }
}
