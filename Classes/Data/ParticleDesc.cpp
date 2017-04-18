#include "ParticleDesc.h"
#include "DataParser.h"

ParticleDesc* ParticleDesc::create(
        const std::string& label,
        const std::string& plist,
        const std::string& spriteFrame) {
    auto pd = new (std::nothrow) ParticleDesc();
    if (pd && pd->init(label, plist, spriteFrame)) {
        pd->autorelease();
        return pd;
    }
    CC_SAFE_DELETE(pd);
    return pd;
}

bool ParticleDesc::init(
        const std::string& label,
        const std::string& plist,
        const std::string& spriteFrame) {
    m_label = label;
    m_plist = plist;
    m_spriteFrame = spriteFrame;
    return init();
}

bool ParticleDesc::init() {
    auto sp = cocos2d::Sprite::create(m_spriteFrame);
    m_sf = sp->getSpriteFrame();
    m_sf->retain();
    auto fileUtils = cocos2d::FileUtils::getInstance();
    m_plistData = fileUtils->getValueMapFromFile(m_plist);
    return true;
}

bool ParticleDesc::initFromJson(const rapidjson::Value& el) {
    DX_DESC_GET_MEMBER(el, string, label);
    DX_DESC_GET_MEMBER(el, string, plist);
    DX_DESC_GET_MEMBER(el, string, spriteFrame);
    return init();
}

ParticleDesc::~ParticleDesc() {
    CC_SAFE_RELEASE(m_sf);
}

cocos2d::ParticleSystemQuad* ParticleDesc::createPSQ() {
    auto psq = cocos2d::ParticleSystemQuad::create(m_plistData);
    //psq->setTexture(m_sf->getTexture());
    return psq;
}

const std::string& ParticleDesc::getLabel() const {
    return m_label;
}
