#include "register_all_packages.h"
#include "lua_danmakux_manual.h"

#if __cplusplus
extern "C" {
#endif

#include "lua/cjson/lua_cjson.h"
#include "lua/lua/lua.h"
#include "lua/lua/lualib.h"
#include "lua/lua/lauxlib.h"

static luaL_Reg luax_exts[] = {
    { "cjson", luaopen_cjson },
    { "cjson.safe", luaopen_cjson_safe },
    {NULL, NULL}
};

void luaopen_lua_cjson_ext(lua_State *L)
{
    // load extensions
    luaL_Reg* lib = luax_exts;
    lua_getglobal(L, "package");
    lua_getfield(L, -1, "preload");
    for (; lib->func; lib++)
    {
        lua_pushcfunction(L, lib->func);
        lua_setfield(L, -2, lib->name);
    }
    lua_pop(L, 2);
}

#if __cplusplus
}
#endif

#include "cocos2d/lua_cocos2dx_manual.hpp"
#include "cocos2d/lua_cocos2dx_experimental_manual.hpp"
#include "cocostudio/lua_cocos2dx_csloader_manual.hpp"
#include "ui/lua_cocos2dx_ui_manual.hpp"
#include "lua_cocos2dx_csloader_auto.hpp"


int lua_register_all_packages(lua_State *state) {
    //luaL_openlibs(state);
    luaopen_lua_cjson_ext(state);
    register_all_cocos2dx_manual(state);
    register_all_cocos2dx_module_manual(state);
    register_all_cocos2dx_experimental_manual(state);
    register_all_cocos2dx_csloader(state);
    register_all_cocos2dx_csloader_manual(state);
    register_ui_moudle(state);
    register_danmakux_module(state);
    return 0;
}
