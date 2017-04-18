#include "TimerLabel.h"

void TimerLabel::update(float dt) {
    m_time += dt;
    int minutes = int(m_time) / 60;
    int seconds = int(m_time) % 60;
    const auto& text = cocos2d::StringUtils::format(
        "%d:%d", minutes, seconds);
    this->setString(text);
    Label::update(dt);
}
