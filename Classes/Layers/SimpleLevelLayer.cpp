#include "SimpleLevelLayer.h"
#include "../Data/DataParser.h"

DX_CREATE_FROM_JSON_FILE_FUNC(SimpleLevelLayer)

void SimpleLevelLayer::update(float dt) {
    LevelLayer::update(dt);
    updateBackground(dt);
}

const float BG_SPEED = 20.0f;
void SimpleLevelLayer::updateBackground(float dt) {
    if (m_bgType == BackgroundType::MOVING) {
        auto bgSp0 = m_bgSprites.at(0);
        auto bgSp1 = m_bgSprites.at(1);
        auto pos0 = bgSp0->getPosition();
        auto pos1 = bgSp1->getPosition();
        auto dy = BG_SPEED * dt;
        pos0.y -= dy;
        pos1.y -= dy;
        auto contentSize = bgSp0->getContentSize();
        if (pos0.y < -contentSize.height) {
            pos0.y += contentSize.height;
            pos1.y += contentSize.height;
        }
        bgSp0->setPosition(pos0);
        bgSp1->setPosition(pos1);
    }
}

bool SimpleLevelLayer::initFromJson(const rapidjson::Value &el) {
    auto levelDesc = SimpleLevelDesc::fromJson(el);
    if (!levelDesc) {
        return false;
    }
    return initWithSimpleLevelDesc(levelDesc);
}

bool SimpleLevelLayer::initWithSimpleLevelDesc(SimpleLevelDesc* levelDesc) {
    if (!LevelLayer::initWithLevelDesc(levelDesc)) {
        return false;
    }
    auto& bgDesc = levelDesc->getBackgroundDesc();
    m_bgType = Str2BackgroundType(bgDesc.bgType);
    if (m_bgType == BackgroundType::MOVING) {
        auto bgSp0 = cocos2d::Sprite::create(bgDesc.bgSprite);
        bgSp0->setAnchorPoint(cocos2d::Vec2::ZERO);
        auto bgSp1 = cocos2d::Sprite::create(bgDesc.bgSprite);
        bgSp1->setAnchorPoint(cocos2d::Vec2::ZERO);
        bgSp1->setPositionY(bgSp0->getContentSize().height);
        m_bgSprites.push_back(bgSp0);
        m_bgSprites.push_back(bgSp1);
    }
    for (auto bgSp : m_bgSprites) {
        this->addChild(bgSp);
    }
    return true;
}
