#ifndef __dx_WEIGHTEDTIMEDMOVINGAVERAGE_H__
#define __dx_WEIGHTEDTIMEDMOVINGAVERAGE_H__

#include <cstdint>

class WeightedTimedMovingAverage {
public:
    void update(std::size_t nbytes);
private:
    float m_lastEntryTime = 0;
    float m_value = 0;
    float m_duration = 0;
};

#endif /* __dx_WEIGHTEDTIMEDMOVINGAVERAGE_H__ */
