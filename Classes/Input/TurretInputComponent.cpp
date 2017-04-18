#include "TurretInputComponent.h"
#include "../Objects/PlayerObject.h"
#include "KeyboardManager.h"
#include "MouseManager.h"

void TurretInputComponent::update(float dt) {
    auto po = getPlayerObject();
    auto levelLayer = po->getParent();
    if (!m_dn) {
        m_dn = cocos2d::DrawNode::create();
        levelLayer->addChild(m_dn);
    }
    auto rot = po->getRotation();
    auto playerPos = po->getPosition();
    auto cursorPosInView = getCursorPosition();
    auto cursorPos = levelLayer->convertToNodeSpace(cursorPosInView);
    auto& children = po->getChildren();
    for (auto& child : children) {
        auto tag = child->getTag();
        if (tag == GAME_OBJECT_TAG_TURRET) {
            auto turret = static_cast<GameObject*>(child);
            updateTurret(turret, playerPos, cursorPos, rot);
        }
    }
}

void TurretInputComponent::updateTurret(GameObject* t,
        const cocos2d::Vec2& p1,
        const cocos2d::Vec2& p2, float rot) {
    cocos2d::Vec2 vel = p2 - p1;
    m_dn->clear();
    m_dn->drawLine(p1, p2, cocos2d::Color4F::GREEN);
    float newRotation = -CC_RADIANS_TO_DEGREES(vel.getAngle()) - 90;
    float rotation;
    float currRot = t->getRotation() + rot;
    if(newRotation < -180 && currRot > 0)
        rotation = ((newRotation + 360) - currRot);
    else if(newRotation > 0 && currRot < -180)
        rotation = (newRotation - (currRot + 360));
    else
        rotation = (newRotation - currRot);
    auto rotationSize = std::abs(rotation);
    float tgtRotation;
    float maxRotation;
    if (t->hasMaxRotation()) {
        maxRotation = t->getMaxRotation();
    } else {
        maxRotation = rotationSize;
    }
    tgtRotation = maxRotation;
    /*
    if (rotationSize > 0.1) {
        tgtRotation = maxRotation;
    } else {
        tgtRotation = maxRotation * rotationSize / 0.1;
    }
    */
    if (rotation < 0) {
        tgtRotation = -tgtRotation;
    }
    //cocos2d::log("tgtRotation=%f rotationSize=%f", tgtRotation, rotationSize);
    t->setAngularSpeed(tgtRotation);
}
