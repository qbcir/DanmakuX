#include "LevelDesc.h"
#include "DataParser.h"
#include "ObjectManager.h"

DX_CREATE_FROM_JSON_FILE_FUNC(LevelDesc)

bool LevelDesc::initFromJson(const rapidjson::Value& el) {
    DX_DESC_GET_MEMBER(el, vector<ScriptComponentDesc>, components);
    auto objMgr = ObjectManager::getInstance();
#define PROCESS_ACTIONDESC() \
    objMgr->addActionDesc(ActionDesc_obj);
    DX_DESC_PROCESS_OPT_VEC(el, ActionDesc, actionDefs, PROCESS_ACTIONDESC);
#undef PROCESS_ACTIONDESC
#define PROCESS_OBJECTDESC() \
    objMgr->addGameObjectDesc(GameObjectDesc_obj);
    DX_DESC_PROCESS_OPT_VEC(el, GameObjectDesc, objectDefs, PROCESS_OBJECTDESC);
#undef PROCESS_OBJECTDESC
#define PROCESS_VEHICLEDESC() \
    objMgr->addVehicleDesc(VehicleDesc_obj);
    DX_DESC_PROCESS_OPT_VEC(el, VehicleDesc, vehicleDefs, PROCESS_VEHICLEDESC);
#undef PROCESS_VEHICLEDESC
#define PROCESS_BULLETDESC() \
    objMgr->addBulletDesc(BulletDesc_obj);
    DX_DESC_PROCESS_OPT_VEC(el, BulletDesc, bulletDefs, PROCESS_BULLETDESC);
#undef PROCESS_BULLETDESC
#define PROCESS_PARTICLEDESC() \
    objMgr->addParticleDesc(ParticleDesc_obj);
    DX_DESC_PROCESS_OPT_VEC(el, ParticleDesc, particleDefs, PROCESS_PARTICLEDESC);
#undef PROCESS_PARTICLEDESC
    cocos2d::Vector<GameAction*> levelActions;
    DX_DESC_GET_OPT_VEC(el, GameAction, actions, levelActions);
    if (levelActions.size() > 0) {
        m_levelActions = ActionDesc::createFromActions("__main", levelActions);
    } else {
        DX_DESC_GET_MEMBER(el, string, script);
        m_levelActions = GameAction::fromLua(m_script);
    }
    CCASSERT(m_levelActions != nullptr, "");
    CC_SAFE_RETAIN(m_levelActions);
    return true;
}

GameAction* LevelDesc::getActions() const {
    return m_levelActions;
}
