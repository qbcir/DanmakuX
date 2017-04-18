#include "lua_danmakux_manual.h"
#include "lua_danmakux_auto.hpp"
#include "cocos2d.h"
#include "tolua_fix.h"
#include "LuaBasicConversions.h"
#include "cocos2d/LuaScriptHandlerMgr.h"
#include "CCLuaValue.h"
#include "CCLuaEngine.h"
#include "Objects/PlayerObject.h"
#include "InputManager.h"
#include "GameAction.h"
#include "Data/Data.h"
//#include "Data/DataParser.h"

static int lua_danmakux_PlayerObject_addOnPlayerDestroyCb(lua_State* L) {
    if (nullptr == L)
        return 0;

    int argc = 0;
    PlayerObject* self = nullptr;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L,1,"dx.PlayerObject",0,&tolua_err)) goto tolua_lerror;
#endif

    self = static_cast<PlayerObject*>(tolua_tousertype(L,1,0));

#if COCOS2D_DEBUG >= 1
    if (nullptr == self) {
        tolua_error(L,"invalid 'self' in function 'lua_danmakux_LevelLayer_addOnPlayerDestroyCb'\n", NULL);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (1 == argc)
    {
#if COCOS2D_DEBUG >= 1
        if (!toluafix_isfunction(L,2,"LUA_FUNCTION",0,&tolua_err))
        {
            goto tolua_lerror;
        }
#endif

        LUA_FUNCTION handler = (  toluafix_ref_function(L,2,0));

        self->addOnPlayerDestroyCb([=](cocos2d::Ref* sender){
            LuaStack* stack = cocos2d::LuaEngine::getInstance()->getLuaStack();
            stack->pushObject(sender, "cc.Ref");
            stack->executeFunctionByHandler(handler, 1);
            stack->clean();
        });

        cocos2d::ScriptHandlerMgr::getInstance()->addCustomHandler((void*)self, handler);
        return 0;
    }

    luaL_error(L, "'addOnPlayerDestroyCb' function of PlayerObject has wrong number of arguments: %d, was expecting %d\n", argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
tolua_lerror:
    tolua_error(L,"#ferror in function 'addOnPlayerDestroyCb'.",&tolua_err);
    return 0;
#endif
}

static int lua_danmakux_Vehicle_addOnHealthChangeCb(lua_State* L) {
    if (nullptr == L)
        return 0;

    int argc = 0;
    Vehicle* self = nullptr;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L,1,"dx.Vehicle",0,&tolua_err)) goto tolua_lerror;
#endif

    self = static_cast<Vehicle*>(tolua_tousertype(L,1,0));

#if COCOS2D_DEBUG >= 1
    if (nullptr == self) {
        tolua_error(L,"invalid 'self' in function 'lua_danmakux_Vehicle_addOnHealthChangeCb'\n", NULL);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (1 == argc)
    {
#if COCOS2D_DEBUG >= 1
        if (!toluafix_isfunction(L,2,"LUA_FUNCTION",0,&tolua_err))
        {
            goto tolua_lerror;
        }
#endif

        LUA_FUNCTION handler = (  toluafix_ref_function(L,2,0));

        self->addOnHealthChangeCb([=](cocos2d::Ref* sender){
            LuaStack* stack = cocos2d::LuaEngine::getInstance()->getLuaStack();
            stack->pushObject(sender, "cc.Ref");
            stack->executeFunctionByHandler(handler, 1);
            stack->clean();
        });

        cocos2d::ScriptHandlerMgr::getInstance()->addCustomHandler((void*)self, handler);
        return 0;
    }

    luaL_error(L, "'addOnHealthChangeCb' function of Vehicle has wrong number of arguments: %d, was expecting %d\n", argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
tolua_lerror:
    tolua_error(L,"#ferror in function 'addOnHealthChangeCb'.",&tolua_err);
    return 0;
#endif
}


static int lua_danmakux_KeyboardManager_addOnKeyUpCb(lua_State* L) {
    if (nullptr == L)
        return 0;

    int argc = 0;
    KeyboardManager* self = nullptr;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L,1,"dx.KeyboardManager",0,&tolua_err)) goto tolua_lerror;
#endif

    self = static_cast<KeyboardManager*>(tolua_tousertype(L,1,0));

#if COCOS2D_DEBUG >= 1
    if (nullptr == self) {
        tolua_error(L,"invalid 'self' in function 'lua_danmakux_KeyboardManager_addOnKeyUpCb'\n", NULL);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (2 == argc)
    {
#if COCOS2D_DEBUG >= 1
        if (!toluafix_isfunction(L,3,"LUA_FUNCTION",0,&tolua_err) ||
            !tolua_isint(L,2,0,&tolua_err))
        {
            goto tolua_lerror;
        }
#endif

        InputKey input_key = static_cast<InputKey>(tolua_tonumber(L,2,0));
        LUA_FUNCTION handler = (  toluafix_ref_function(L,3,0));

        self->addOnKeyUpCb(input_key, [=]() {
            LuaStack* stack = cocos2d::LuaEngine::getInstance()->getLuaStack();
            stack->executeFunctionByHandler(handler, 0);
            stack->clean();
        });

        cocos2d::ScriptHandlerMgr::getInstance()->addCustomHandler((void*)self, handler);
        return 0;
    }

    luaL_error(L, "'addOnKeyUpCb' function of KeyboardManager has wrong number of arguments: %d, was expecting %d\n", argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
tolua_lerror:
    tolua_error(L,"#ferror in function 'addOnKeyUpCb'.",&tolua_err);
    return 0;
#endif
}

static int lua_danmakux_KeyboardManager_addOnKeyDownCb(lua_State* L) {
    if (nullptr == L)
        return 0;

    int argc = 0;
    KeyboardManager* self = nullptr;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L,1,"dx.KeyboardManager",0,&tolua_err)) goto tolua_lerror;
#endif

    self = static_cast<KeyboardManager*>(tolua_tousertype(L,1,0));

#if COCOS2D_DEBUG >= 1
    if (nullptr == self) {
        tolua_error(L,"invalid 'self' in function 'lua_danmakux_KeyboardManager_addOnKeyDownCb'\n", NULL);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (2 == argc)
    {
#if COCOS2D_DEBUG >= 1
        if (!toluafix_isfunction(L,3,"LUA_FUNCTION",0,&tolua_err) ||
            !tolua_isint(L,2,0,&tolua_err))
        {
            goto tolua_lerror;
        }
#endif

        InputKey input_key = static_cast<InputKey>(tolua_tonumber(L,2,0));
        LUA_FUNCTION handler = (  toluafix_ref_function(L,3,0));

        self->addOnKeyDownCb(input_key, [=]() {
            LuaStack* stack = cocos2d::LuaEngine::getInstance()->getLuaStack();
            stack->executeFunctionByHandler(handler, 0);
            stack->clean();
        });

        cocos2d::ScriptHandlerMgr::getInstance()->addCustomHandler((void*)self, handler);
        return 0;
    }

    luaL_error(L, "'addOnKeyDownCb' function of KeyboardManager has wrong number of arguments: %d, was expecting %d\n", argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
tolua_lerror:
    tolua_error(L,"#ferror in function 'addOnKeyDownCb'.",&tolua_err);
    return 0;
#endif
}

#if COCOS2D_DEBUG >= 0
#define DX_LUA_CREATE_FROM_TABLE(klass) \
static int lua_danmakux_##klass##_create(lua_State* L) { \
    if (nullptr == L) \
        return 0; \
    int argc = 0; \
    tolua_Error tolua_err; \
    if (!tolua_isusertable(L,1,#klass,0,&tolua_err)) { \
        goto tolua_lerror; \
    } \
    argc = lua_gettop(L) - 1; \
    if (argc == 1) { \
        if (!toluafix_istable(L,2,"",0,&tolua_err)) { \
            goto tolua_lerror; \
        } \
        klass* tolua_ret = klass::fromLua(L, 2); \
        int nID = (tolua_ret) ? (int)tolua_ret->_ID : -1; \
        int* pLuaID = (tolua_ret) ? &tolua_ret->_luaID : NULL; \
        toluafix_pushusertype_ccobject(L, nID, pLuaID, (void*)tolua_ret,"dx."#klass); \
        return 1; \
    } \
    luaL_error(L, "'create' function of "#klass" has wrong number of arguments: %d, was expecting %d\n", argc, 1); \
    return 0; \
tolua_lerror: \
    tolua_error(L,"#ferror in function 'create'.",&tolua_err); \
    return 0; \
}
#else
#define DX_LUA_CREATE_FROM_TABLE(klass) \
static int lua_danmakux_##klass##_create(lua_State* L) { \
    if (nullptr == L) \
        return 0; \
    int argc = lua_gettop(L) - 1; \
    if (argc == 1) { \
        klass* tolua_ret = klass::fromLua(L, -1); \
        int nID = (tolua_ret) ? (int)tolua_ret->_ID : -1; \
        int* pLuaID = (tolua_ret) ? &tolua_ret->_luaID : NULL; \
        toluafix_pushusertype_ccobject(L, nID, pLuaID, (void*)tolua_ret,"dx."#klass); \
        return 1; \
    } \
    luaL_error(L, "'create' function of "#klass" has wrong number of arguments: %d, was expecting %d\n", argc, 1); \
    return 0; \
}
#endif

#define DX_FOR_EACH_DESC_CLASS() \
    PROCESS_DESC_CLASS(VehicleDesc) \
    PROCESS_DESC_CLASS(ForeverDesc) \
    PROCESS_DESC_CLASS(RepeatDesc) \
    PROCESS_DESC_CLASS(ActionRefDesc) \
    PROCESS_DESC_CLASS(PlaceObjectDesc) \
    PROCESS_DESC_CLASS(PlaceVehicleDesc) \
    PROCESS_DESC_CLASS(FireBulletDesc) \
    PROCESS_DESC_CLASS(PlaceObjectRefDesc) \
    PROCESS_DESC_CLASS(PlaceVehicleRefDesc) \
    PROCESS_DESC_CLASS(FireBulletRefDesc) \
    PROCESS_DESC_CLASS(ChangeSpeedDesc) \
    PROCESS_DESC_CLASS(ChangeDirectionDesc) \
    PROCESS_DESC_CLASS(AccelDesc) \
    PROCESS_DESC_CLASS(VanishDesc) \
    PROCESS_DESC_CLASS(WaitDesc) \
    PROCESS_DESC_CLASS(ActionDesc)

#define PROCESS_DESC_CLASS(klass) DX_LUA_CREATE_FROM_TABLE(klass)
    DX_FOR_EACH_DESC_CLASS()
#undef PROCESS_DESC_CLASS

#define DX_LUA_EXTEND_DESC(klass) \
static void extend##klass(lua_State* L) { \
    lua_pushstring(L, #klass); \
    lua_rawget(L, LUA_REGISTRYINDEX); \
    if (lua_istable(L,-1)) { \
        lua_pushstring(L, "create"); \
        lua_pushcfunction(L, lua_danmakux_##klass##_create); \
        lua_rawset(L,-3); \
    } \
    lua_pop(L, 1); \
}

#define PROCESS_DESC_CLASS(klass) DX_LUA_EXTEND_DESC(klass)
    DX_FOR_EACH_DESC_CLASS()
#undef PROCESS_DESC_CLASS

static void extendPlayerObject(lua_State* L)
{
    lua_pushstring(L, "PlayerObject");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L,-1))
    {
        tolua_function(L, "addOnPlayerDestroyCb", lua_danmakux_PlayerObject_addOnPlayerDestroyCb);
    }
    lua_pop(L, 1);
}

static void extendVehicle(lua_State* L)
{
    lua_pushstring(L, "Vehicle");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L,-1))
    {
        tolua_function(L, "addOnHealthChangeCb", lua_danmakux_Vehicle_addOnHealthChangeCb);
    }
    lua_pop(L, 1);
}

static void extendKeyboardManager(lua_State* L)
{
    lua_pushstring(L, "KeyboardManager");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L,-1))
    {
        tolua_function(L, "addOnKeyUpCb", lua_danmakux_KeyboardManager_addOnKeyUpCb);
        tolua_function(L, "addOnKeyDownCb", lua_danmakux_KeyboardManager_addOnKeyDownCb);
    }
    lua_pop(L, 1);
}

int register_all_danmakux_manual(lua_State* L) {
    if (nullptr == L)
        return 0;
    extendVehicle(L);
    extendPlayerObject(L);
    extendKeyboardManager(L);
    //
#define PROCESS_DESC_CLASS(klass) extend##klass(L);
    DX_FOR_EACH_DESC_CLASS()
#undef PROCESS_DESC_CLASS
    return 0;
}

int register_danmakux_module(lua_State* L)
{
    lua_getglobal(L, "_G");
    if (lua_istable(L,-1))//stack:...,_G,
    {
        register_all_danmakux(L);
        register_all_danmakux_manual(L);
    }
    lua_pop(L, 1);
    return 1;
}
