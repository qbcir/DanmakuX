#ifndef __dx_TURNMOVEMENTINPUTCOMPONENT_H__
#define __dx_TURNMOVEMENTINPUTCOMPONENT_H__

#include "cocos2d.h"
#include "BaseInputComponent.h"

class TurnMovementInputComponent : public BaseInputComponent {
public:
    void update(float dt);
    CREATE_FUNC(TurnMovementInputComponent)
};

#endif // __dx_TURNMOVEMENTINPUTCOMPONENT_H__
