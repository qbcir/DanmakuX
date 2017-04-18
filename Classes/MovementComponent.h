#ifndef MOVEMENTCOMPONENT_H
#define MOVEMENTCOMPONENT_H

#include "cocos2d.h"

class GameObject;
class MovementComponent : public cocos2d::Component {
public:
    virtual ~MovementComponent();
    void setTarget(GameObject* tgt);
    GameObject* getTarget();
protected:
    bool initWithTarget(GameObject* tgt);
    GameObject* m_tgt = nullptr;
};

#define DX_CREATE_MOVE_COMPONENT_FUNC(klass) \
static klass* create(GameObject* tgt) { \
    auto mc = new (std::nothrow) klass(); \
    if (mc && mc->initWithTarget(tgt)) { \
        mc->autorelease(); \
        return mc; \
    } \
    CC_SAFE_DELETE(mc); \
    return nullptr; \
}

class TurretComponent : public MovementComponent {
public:
    DX_CREATE_MOVE_COMPONENT_FUNC(TurretComponent)
    void update(float dt);
protected:
    float m_timeToTgt = 0.25f;
    float m_slowRadius = 0.1f;
    float m_prevRotation = 0.0f;
};

class AlignComponent : public MovementComponent {
public:
    DX_CREATE_MOVE_COMPONENT_FUNC(AlignComponent)
    void update(float dt);
protected:
    float m_timeToTgt = 0.25f;
    float m_slowRadius = 0.1f;
};

class KinematicArriveComponent : public MovementComponent {
public:
    DX_CREATE_MOVE_COMPONENT_FUNC(KinematicArriveComponent)
    void setRadius(float r);
    void update(float dt);
protected:
    bool initWithTarget(GameObject* tgt);
    float m_radius = 0.0f; // satisfaction radius
    float m_radiusSquared = 0.0f;
    float m_timeToTgt = 0.25f;
};

class KinematicArriveXComponent : public MovementComponent {
public:
    DX_CREATE_MOVE_COMPONENT_FUNC(KinematicArriveXComponent)
    void setRadius(float r);
    void update(float dt);
protected:
    float m_radius = 0.0f; // satisfaction radius
    float m_timeToTgt = 0.25f;
};

class KinematicArriveYComponent : public MovementComponent {
public:
    DX_CREATE_MOVE_COMPONENT_FUNC(KinematicArriveYComponent)
    void setRadius(float r);
    void update(float dt);
protected:
    float m_radius = 0.0f; // satisfaction radius
    float m_timeToTgt = 0.25f;
};

#endif // MOVEMENTCOMPONENT_H
