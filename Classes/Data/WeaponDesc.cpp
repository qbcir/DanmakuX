#include "WeaponDesc.h"
#include "DataParser.h"

bool WeaponDesc::initFromJson(const rapidjson::Value& el) {
    DX_DESC_GET_MEMBER(el, string, label);
    DX_DESC_GET_MEMBER(el, string, image);
    DX_DESC_GET_OPT_MEMBER(el, string, description);
    DX_DESC_GET_VEC(el, GameAction, actions, m_actions);
    return true;
}

bool WeaponDesc::initFromLua(lua_State* L, int idx) {
    DX_LUA_DESC_GET_MEMBER(L, idx, string, label);
    DX_LUA_DESC_GET_MEMBER(L, idx, string, image);
    DX_LUA_DESC_GET_OPT_MEMBER(L, idx, string, description);
    DX_LUA_DESC_GET_VEC(L, idx, GameAction, actions, m_actions);
    return true;
}

ActionDesc* WeaponDesc::getActionDesc() {
    return ActionDesc::createFromActions("", m_actions);
}
