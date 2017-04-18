#include "utils.h"

void pauseNode(cocos2d::Node* node) {
    node->pause();
    for (auto ch : node->getChildren()) {
        pauseNode(ch);
    }
}

void resumeNode(cocos2d::Node* node) {
    node->resume();
    for (auto ch : node->getChildren()) {
        resumeNode(ch);
    }
}
