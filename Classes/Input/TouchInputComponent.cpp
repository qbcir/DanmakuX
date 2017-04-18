#include "TouchInputComponent.h"
#include "Objects/PlayerObject.h"
#include "Nodes/Joystick.h"
#include "Nodes/Button.h"

TouchInputComponent::~TouchInputComponent() {
    CC_SAFE_RELEASE(m_fireButton);
    CC_SAFE_RELEASE(m_joystick);
}

void TouchInputComponent::setFireButton(Button* b) {
    CC_SAFE_RELEASE(m_fireButton);
    m_fireButton = b;
    CC_SAFE_RETAIN(b);
}

void TouchInputComponent::setJoystick(Joystick* j) {
    CC_SAFE_RELEASE(m_joystick);
    m_joystick = j;
    CC_SAFE_RETAIN(m_joystick);
}

void TouchInputComponent::update(float dt) {
    updateJoystick(dt);
    updateFireButton(dt);
}

void TouchInputComponent::updateFireButton(float dt) {
    auto owner = static_cast<PlayerObject*>(this->getOwner());
    if (m_fireButton->getPressed()) {
        owner->fire();
    }
}

void TouchInputComponent::updateJoystick(float dt) {
    auto owner = static_cast<PlayerObject*>(this->getOwner());
    auto v = m_joystick->getVelocity();
    v *= 50.0f;
    owner->setVelocity(v);
}
