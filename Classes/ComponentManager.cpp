#include "GameManager.h"
#include "Objects/PlayerObject.h"
#include "MovementComponent.h"
#include "CCComponentLua.h"

ComponentManager* ComponentManager::getInstance() {
    return GameManager::getInstance()->getComponentManager();
}

ComponentManager::ComponentCreatorMapT ComponentManager::ComponentCreators {
    { "playerSeeker", &ComponentManager::createPlayerSeeker },
    { "playerSeekerX", &ComponentManager::createPlayerSeekerX },
    { "playerAlign", &ComponentManager::createPlayerAlign },
    { "turret", &ComponentManager::createTurret }
};

cocos2d::Component* ComponentManager::createComponent(const std::string& name) {
    auto c_it = ComponentCreators.find(name);
    cocos2d::Component* comp = nullptr;
    if (c_it != std::end(ComponentCreators)) {
        comp = (this->*(c_it->second))();
    }
    return comp;
}

void ComponentManager::setPlayerObject(PlayerObject* obj) {
    if (m_playerObject) {
        CC_SAFE_RELEASE(m_playerObject);
    }
    m_playerObject = obj;
    CC_SAFE_RETAIN(m_playerObject);
}

PlayerObject* ComponentManager::getPlayerObject() {
    return m_playerObject;
}

cocos2d::Component* ComponentManager::createPlayerSeeker() {
    return KinematicArriveComponent::create(m_playerObject);
}

cocos2d::Component* ComponentManager::createPlayerAlign() {
    return AlignComponent::create(m_playerObject);
}

cocos2d::Component* ComponentManager::createPlayerSeekerX() {
    return KinematicArriveXComponent::create(m_playerObject);
}

cocos2d::Component* ComponentManager::createTurret() {
    return TurretComponent::create(m_playerObject);
}
