#ifndef PARTICLEDESC_H
#define PARTICLEDESC_H

#include "cocos2d.h"
#include "json/document.h"
#include "DataMacros.h"

class ParticleDesc : public cocos2d::Ref {
public:
    static ParticleDesc* create(
            const std::string& label,
            const std::string& plist,
            const std::string& spriteFrame);
    DX_CREATE_FROM_JSON_FUNC(ParticleDesc)
    ~ParticleDesc();
    const std::string& getLabel() const;
    cocos2d::ParticleSystemQuad* createPSQ();
protected:
    bool init();
    bool init(
            const std::string& label,
            const std::string& plist,
            const std::string& spriteFrame);
    bool initFromJson(const rapidjson::Value& el);
    std::string m_label;
    std::string m_plist;
    std::string m_spriteFrame;
    cocos2d::ValueMap m_plistData;
    cocos2d::SpriteFrame* m_sf = nullptr;
};

#endif // PARTICLEDESC_H
