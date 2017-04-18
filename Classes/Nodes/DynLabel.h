#ifndef DYNTEXT_H
#define DYNTEXT_H

#include "cocos2d.h"

class AnimateLabel : public cocos2d::ActionInterval {
public:
    static AnimateLabel* create(
            const std::string& text,
            float duration);
    bool init(const std::string& text,
              float duration);
    void startWithTarget(cocos2d::Node* node);
    void update(float time);
    bool isDone();
protected:
    size_t m_strIdx = 0;
    std::string m_text;
};

#endif // DYNTEXT_H
