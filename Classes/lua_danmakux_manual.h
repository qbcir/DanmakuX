#ifndef LUA_DANMAKUX_MANUAL_H
#define LUA_DANMAKUX_MANUAL_H

#ifdef __cplusplus
extern "C" {
#endif
#include "tolua++.h"
#ifdef __cplusplus
}
#endif

TOLUA_API int register_danmakux_module(lua_State* L);

#endif // LUA_DANMAKUX_MANUAL_H
