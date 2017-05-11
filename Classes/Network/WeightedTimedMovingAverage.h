#ifndef __dx_WEIGHTEDTIMEDMOVINGAVERAGE_H__
#define __dx_WEIGHTEDTIMEDMOVINGAVERAGE_H__

#include <cstdint>

class WeightedTimedMovingAverage {
public:
    WeightedTimedMovingAverage(float duration);
    void init();
    void update(float value);
    float getValue() const;
private:
    float m_lastEntryTime = 0;
    float m_value = 0;
    float m_duration = 0;
};

#endif /* __dx_WEIGHTEDTIMEDMOVINGAVERAGE_H__ */
