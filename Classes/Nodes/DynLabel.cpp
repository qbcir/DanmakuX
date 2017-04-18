#include "DynLabel.h"

AnimateLabel* AnimateLabel::create(const std::string& text,float duration) {
    auto a = new (std::nothrow) AnimateLabel();
    if (a && a->init(text, duration)) {
        a->autorelease();
        return a;
    }
    CC_SAFE_DELETE(a);
    return nullptr;
}

bool AnimateLabel::init(
        const std::string& text,
        float duration) {
    m_text = text;
    return this->initWithDuration(duration);
}

void AnimateLabel::startWithTarget(cocos2d::Node *node) {
    auto tgt = static_cast<cocos2d::Label*>(node);
    tgt->setString(m_text);
    for (size_t i = 0; i < m_text.size(); i++) {
        auto letter = tgt->getLetter(i);
        letter->setOpacity(0);
    }
    ActionInterval::startWithTarget(node);
}

void AnimateLabel::update(float dt) {
    auto tgt = static_cast<cocos2d::Label*>(this->getTarget());
    const auto& str = tgt->getString();
    auto elapsed = this->getElapsed();
    auto duration = this->getDuration();
    auto ratio = elapsed / duration;
    size_t len = std::ceil(ratio * str.size());
    for (size_t i = m_strIdx; i < len && i < str.size(); i++) {
        auto letter = tgt->getLetter(i);
        letter->setOpacity(255);
        m_strIdx++;
    }
}

bool AnimateLabel::isDone() {
    m_strIdx >= m_text.size();
}
