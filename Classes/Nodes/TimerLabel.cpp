#include "TimerLabel.h"

cocos2d::Label* TimerLabel::getLabel() {
    return m_label;
}

bool TimerLabel::init() {
    m_label = cocos2d::Label::create();
    this->addChild(m_label);
    return true;
}

void TimerLabel::update(float dt) {
    m_time += dt;
    int minutes = int(m_time) / 60;
    int seconds = int(m_time) % 60;
    const auto& text = cocos2d::StringUtils::format(
        "%d:%d", minutes, seconds);
    m_label->setString(text);
    Node::update(dt);
}
