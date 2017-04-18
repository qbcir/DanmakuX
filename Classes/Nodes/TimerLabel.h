#ifndef TIMERLABEL_H
#define TIMERLABEL_H

#include "cocos2d.h"

class TimerLabel : public cocos2d::Label
{
public:
    CREATE_FUNC(TimerLabel)
    void update(float dt);
protected:
    float m_time = 0;
};

#endif // TIMERLABEL_H
