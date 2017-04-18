#ifndef __dx_FREEMOVEMENTINPUTCOMPONENT_H__
#define __dx_FREEMOVEMENTINPUTCOMPONENT_H__

#include "cocos2d.h"
#include "BaseInputComponent.h"

class FreeMovementInputComponent : public BaseInputComponent {
public:
    void update(float dt);
    CREATE_FUNC(FreeMovementInputComponent)
};

#endif // __dx_FREEMOVEMENTINPUTCOMPONENT_H__
