#include "LinkingCtxt.h"
#include "GameObject.h"

uint64_t LinkingCtxt::getNetId(GameObject* go, bool createIfNotExists) {
    auto it = m_gameObject2NetIdMap.find(go);
    if (it != std::end(m_gameObject2NetIdMap)) {
        return it->second;
    } else if (createIfNotExists) {
        uint64_t netId = m_nextNetId++;
        addGameObject(go, netId);
        return netId;
    } else {
        return 0;
    }
}

GameObject* LinkingCtxt::getGameObject(uint64_t netId) {
    auto it = m_netId2GameObjectMap.find(netId);
    return it == std::end(m_netId2GameObjectMap) ? nullptr : it->second;
}

void LinkingCtxt::addGameObject(GameObject* go, uint64_t netId) {
    m_gameObject2NetIdMap[go] = netId;
    m_netId2GameObjectMap[netId] = go;
}

void LinkingCtxt::removeGameObject(GameObject* go) {
    uint64_t netId = m_gameObject2NetIdMap[go];
    m_gameObject2NetIdMap.erase(go);
    m_netId2GameObjectMap.erase(netId);
}
