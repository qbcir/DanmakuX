#ifndef __dx_ObjectManager_h__
#define __dx_ObjectManager_h__

#include "cocos2d.h"
#include "GameAction.h"
#include "Data/Data.h"

#define DX_OM_DEF_DESC_MAP(klass, member) cocos2d::Map<std::string, klass*> member
#define DX_OM_DEF_GET_DESC_FUNC(klass) klass* get##klass (const std::string& label) const
#define DX_OM_DEF_ADD_DESC_FUNC(klass) klass* add##klass (klass* obj)
#define DX_OM_DEF_REMOVE_DESC_FUNC(klass) klass* remove##klass (klass* obj)
#define DX_OM_DEF_CLEAR_DESC_FUNC(klass) void clear##klass##s ()

#define DX_OM_DEF_DESC_FUNCS(klass) \
    DX_OM_DEF_GET_DESC_FUNC(klass); \
    DX_OM_DEF_ADD_DESC_FUNC(klass); \
    DX_OM_DEF_REMOVE_DESC_FUNC(klass); \
    DX_OM_DEF_CLEAR_DESC_FUNC(klass);

class PlayerDesc;
class ObjectManager {
public:
    static ObjectManager* getInstance();
    DX_OM_DEF_DESC_FUNCS(ActionDesc)
    DX_OM_DEF_DESC_FUNCS(BulletDesc)
    DX_OM_DEF_DESC_FUNCS(VehicleDesc)
    DX_OM_DEF_DESC_FUNCS(ParticleDesc)
    DX_OM_DEF_DESC_FUNCS(GameObjectDesc)

    void clearDescriptions();
    PlayerDesc* getPlayerDesc();
protected:
    DX_OM_DEF_DESC_MAP(ActionDesc, m_actionDescs);
    DX_OM_DEF_DESC_MAP(BulletDesc, m_bulletDescs);
    DX_OM_DEF_DESC_MAP(VehicleDesc, m_vehicleDescs);
    DX_OM_DEF_DESC_MAP(ParticleDesc, m_particleDescs);
    DX_OM_DEF_DESC_MAP(GameObjectDesc, m_objDescs);
    PlayerDesc m_playerDesc;
};

#endif /* __dx_ObjectManager_h__ */
