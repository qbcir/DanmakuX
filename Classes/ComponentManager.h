#ifndef COMPONENTMANAGER_H
#define COMPONENTMANAGER_H

#include "cocos2d.h"

class PlayerObject;
class ComponentManager
{
public:
    typedef cocos2d::Component*(ComponentManager::*ComponentCreatorT)();
    typedef std::unordered_map<std::string, ComponentCreatorT> ComponentCreatorMapT;
    static ComponentCreatorMapT ComponentCreators;
    static ComponentManager* getInstance();
    cocos2d::Component* createComponent(const std::string& name);
    void setPlayerObject(PlayerObject* obj);
    PlayerObject* getPlayerObject();
private:
    cocos2d::Component* createPlayerSeeker();
    cocos2d::Component* createPlayerSeekerX();
    cocos2d::Component* createPlayerAlign();
    cocos2d::Component* createTurret();
    cocos2d::Component* createLuaComponent();
    PlayerObject* m_playerObject = nullptr;
};

#endif // COMPONENTMANAGER_H
