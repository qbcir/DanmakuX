#include "WeaponDesc.h"
#include "JSONParser.h"

bool WeaponDesc::initFromJson(const rapidjson::Value& el) {
    DX_DESC_GET_MEMBER(el, string, label);
    DX_DESC_GET_MEMBER(el, string, image);
    DX_DESC_GET_OPT_MEMBER(el, string, description);
    DX_DESC_GET_VEC(el, GameAction, actions, m_actions);
    return true;
}

ActionDesc* WeaponDesc::getActionDesc() {
    return ActionDesc::createFromActions("", m_actions);
}
