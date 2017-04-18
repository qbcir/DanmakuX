#ifndef __danmakux_LevelLayer_h__
#define __danmakux_LevelLayer_h__

#include "cocos2d.h"
#include "../Objects/GameObject.h"
#include "../Data/LevelDesc.h"

typedef std::vector<std::vector<GameObject*>> LevelGridT;

class PlayerObject;
class LevelLayer : public cocos2d::Layer {
public:
    static LevelLayer* createFromJson(const std::string& filename);
    virtual void update(float dt);
    void onExit();
    void addObject(GameObject* obj);
    void setPlayerObject(PlayerObject* obj);
    PlayerObject* getPlayerObject();
    bool isPaused();
    bool setPaused(bool p);
protected:
    bool initWithLevelDesc(LevelDesc* desc);
    bool initFromJson(const std::string& filename);

    void pauseLevel();
    void resumeLevel();
    bool detectCollision(GameObject* obj0, GameObject* obj1);
    bool isOutOfGrid(float x, float y);
    size_t positionToGridIndex(float x, float y);
    void updateGrid();
    void resolveCollisions();
    void removeDeadObjects();
    void removeOutOfGridObjects();
    cocos2d::Size m_winSize;
    cocos2d::Size m_gridSize;
    cocos2d::Size m_gridCellSize;
    cocos2d::Rect m_gridRect;
    size_t m_gridVecSize = 0;
    LevelGridT m_grid;
    cocos2d::Vector<GameObject*> m_objects;
    PlayerObject* m_playerObj = nullptr;
    bool m_paused = false;
    cocos2d::Action* m_levelAction = nullptr;
};

#endif /* __danmakux_LevelLayer_h__ */
