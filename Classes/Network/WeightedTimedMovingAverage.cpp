#include "WeightedTimedMovingAverage.h"
#include "../GameManager.h"
#include "cocos2d.h"

WeightedTimedMovingAverage::WeightedTimedMovingAverage(float duration) :
    m_duration(duration),
    m_value(0.f)
{}

void WeightedTimedMovingAverage::init() {
    m_lastEntryTime = GameManager::getInstance()->getTime();
}

void WeightedTimedMovingAverage::update(float value) {
    float time = GameManager::getInstance()->getTime();
    float timeSinceLastEntry = time - m_lastEntryTime;
    float fractionOfDuration  = timeSinceLastEntry / m_duration;
    if(fractionOfDuration > 1.f) {
        fractionOfDuration = 1.f;
    }
    m_value = m_value*(1.f-fractionOfDuration ) + value*fractionOfDuration;
    m_lastEntryTime = time;
}

float WeightedTimedMovingAverage::getValue() const {
    return m_value;
}
