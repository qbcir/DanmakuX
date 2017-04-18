#ifndef SPRITEDESC_H
#define SPRITEDESC_H

#include "cocos2d.h"

struct SpriteDesc {
    std::string filename;
    std::string name;
    int zOrder;
    cocos2d::Vec2 position;
    cocos2d::Vec2 anchorPoint { 0.5, 0.5 };
};

#endif // SPRITEDESC_H
