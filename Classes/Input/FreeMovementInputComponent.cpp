#include "FreeMovementInputComponent.h"
#include "../Objects/PlayerObject.h"
#include "KeyboardManager.h"

const float DIRECTION_N_ANGLE = 0;
static const std::vector<float> direction2angle {
    CC_DEGREES_TO_RADIANS(DIRECTION_N_ANGLE),       // N
    CC_DEGREES_TO_RADIANS(DIRECTION_N_ANGLE - 180), // S
    CC_DEGREES_TO_RADIANS(DIRECTION_N_ANGLE - 90),  // E
    CC_DEGREES_TO_RADIANS(DIRECTION_N_ANGLE + 90),  // W
    CC_DEGREES_TO_RADIANS(DIRECTION_N_ANGLE - 45),  // NE
    CC_DEGREES_TO_RADIANS(DIRECTION_N_ANGLE + 45),  // NW
    CC_DEGREES_TO_RADIANS(DIRECTION_N_ANGLE - 135), // SE
    CC_DEGREES_TO_RADIANS(DIRECTION_N_ANGLE + 135)  // SW
};

void FreeMovementInputComponent::update(float dt) {
    auto po = getPlayerObject();
    auto direction = m_keyboardManager->getDirection();
    if (direction != Direction::INVALID) {
        cocos2d::Vec2 speed(0, 30);
        float angle = direction2angle[static_cast<size_t>(direction)];
        speed.rotate(cocos2d::Vec2::ZERO, angle);
        po->setRotation(-CC_RADIANS_TO_DEGREES(angle));
        po->setVelocity(speed);
    } else {
        po->setVelocity(cocos2d::Vec2::ZERO);
    }
}
