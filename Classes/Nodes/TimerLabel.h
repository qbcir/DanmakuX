#ifndef TIMERLABEL_H
#define TIMERLABEL_H

#include "cocos2d.h"

class TimerLabel : public cocos2d::Node {
public:
    CREATE_FUNC(TimerLabel)
    virtual bool init() override;
    virtual void update(float dt) override;
    cocos2d::Label* getLabel();
protected:
    cocos2d::Label* m_label = nullptr;
    float m_time = 0;
};

#endif // TIMERLABEL_H
