#include "ObjectManager.h"
#include "GameManager.h"
#include "Data/PlayerDesc.h"
#include "Objects/Vehicle.h"
#include "Objects/Bullet.h"
#include "Objects/PlayerObject.h"

#define DX_CREATE_GET_DESC_FUNC(klass, member) \
    klass* ObjectManager::get##klass(const std::string& label) const { \
        auto it = member.find(label); \
        return it == member.end() ? nullptr : it->second; \
    }

#define DX_CREATE_ADD_DESC_FUNC(klass, member) \
    klass* ObjectManager::add##klass(klass* obj) {\
        klass* prev = nullptr; \
        auto prev_it = member.find(obj->getLabel()); \
        if (prev_it != member.end()) {\
            prev = prev_it->second; \
        } \
        member.insert(obj->getLabel(), obj); \
        return prev; \
    }

#define DX_CREATE_REMOVE_DESC_FUNC(klass, member) \
    klass* ObjectManager::remove##klass(klass* obj) {\
        klass* prev = nullptr; \
        auto prev_it = member.find(obj->getLabel()); \
        if (prev_it != member.end()) {\
            prev = prev_it->second; \
            member.erase(prev_it); \
        } \
        return prev; \
    }

#define DX_CREATE_CLEAR_DESC_FUNC(klass, member) \
    void ObjectManager::clear##klass##s() {\
        member.clear(); \
    }

#define DX_CREATE_DESC_FUNCS(klass, member) \
    DX_CREATE_GET_DESC_FUNC(klass, member) \
    DX_CREATE_ADD_DESC_FUNC(klass, member) \
    DX_CREATE_REMOVE_DESC_FUNC(klass, member) \
    DX_CREATE_CLEAR_DESC_FUNC(klass, member)

DX_CREATE_DESC_FUNCS(ActionDesc, m_actionDescs)
DX_CREATE_DESC_FUNCS(BulletDesc, m_bulletDescs)
DX_CREATE_DESC_FUNCS(VehicleDesc, m_vehicleDescs)
DX_CREATE_DESC_FUNCS(ParticleDesc, m_particleDescs)
DX_CREATE_DESC_FUNCS(GameObjectDesc, m_objDescs)

ObjectManager* ObjectManager::getInstance() {
  return GameManager::getInstance()->getObjectManager();
}

void ObjectManager::clearDescriptions() {
    clearActionDescs();
    clearBulletDescs();
    clearVehicleDescs();
    clearGameObjectDescs();
}

PlayerDesc* ObjectManager::getPlayerDesc() {
    return &m_playerDesc;
}


