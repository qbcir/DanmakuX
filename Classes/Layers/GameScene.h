#ifndef __danmakux_GameScene_h__
#define __danmakux_GameScene_h__

#include "cocos2d.h"

class SceneDesc;
class GameScene : public cocos2d::Layer {
public:
    static cocos2d::Scene* createScene(SceneDesc* desc);
    static GameScene* create(SceneDesc* desc);
protected:
    bool initWithSceneDesc(SceneDesc* desc);
};

#endif /* __danmakux_GameScene_h__ */
