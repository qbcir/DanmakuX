#ifndef __DanmakuXNet_LINKINGCTXT_H__
#define __DanmakuXNet_LINKINGCTXT_H__

#include "cocos2d.h"
#include "DanmakuX/GameObject.h"

class LinkingCtxt
{
public:
    LinkingCtxt() {}
    uint64_t getNetId(GameObject* go, bool createIfNotExists);
    GameObject* getGameObject(uint64_t netId);
    void addGameObject(GameObject* go, uint64_t netId);
    void removeGameObject(GameObject* go);
private:
    std::unordered_map<uint64_t, GameObject*> m_netId2GameObjectMap;
    std::unordered_map<const GameObject*, uint64_t> m_gameObject2NetIdMap;
    uint64_t m_nextNetId = 1;
};

#endif // __DanmakuXNet_LINKINGCTXT_H__
