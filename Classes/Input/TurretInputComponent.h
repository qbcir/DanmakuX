#ifndef __dx_TURRETINPUTCOMPONENT_H__
#define __dx_TURRETINPUTCOMPONENT_H__

#include "cocos2d.h"
#include "BaseInputComponent.h"

class GameObject;
class TurretInputComponent : public BaseInputComponent {
public:
    void update(float dt);
    CREATE_FUNC(TurretInputComponent)
protected:
    void updateTurret(GameObject* t,
        const cocos2d::Vec2& p1,
        const cocos2d::Vec2& p2,
        float rot);
    cocos2d::DrawNode* m_dn = nullptr;
};

#endif // __dx_TURRETINPUTCOMPONENT_H__
