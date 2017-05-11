#ifndef __danmakux_LevelLayer_h__
#define __danmakux_LevelLayer_h__

#include "cocos2d.h"
#include "../Objects/GameObject.h"
#include "../Data/LevelDesc.h"
#include "Box2D/Box2D.h"

class PlayerObject;
class LevelLayer : public cocos2d::Layer {
public:
    virtual ~LevelLayer();
    static LevelLayer* createFromJson(const std::string& filename);
    virtual void update(float dt);
    void onExit();
    void addObject(GameObject* obj);
    void setPlayerObject(PlayerObject* obj);
    PlayerObject* getPlayerObject();
    b2Body* createPhysBody(GameObject* obj);
    bool isPaused();
    bool setPaused(bool p);
protected:
    bool initWithLevelDesc(LevelDesc* desc);
    bool initFromJson(const std::string& filename);
    void pauseLevel();
    void resumeLevel();
    bool detectCollision(GameObject* obj0, GameObject* obj1);
    bool isOutOfGrid(float x, float y);
    void resolveCollisions();
    void removeDeadObjects();
    void removeOutOfGridObjects();
    float m_lastTimestamp = 0.f;
    cocos2d::Size m_winSize;
    cocos2d::Rect m_worldBounds;
    cocos2d::Vector<GameObject*> m_objects;
    PlayerObject* m_playerObj = nullptr;
    bool m_paused = false;
    cocos2d::Action* m_levelAction = nullptr;
    b2World* m_world = nullptr;
    cocos2d::DrawNode* m_debugDraw = nullptr;
};

#endif /* __danmakux_LevelLayer_h__ */
