#ifndef __danmakuX_LuaParser_h__
#define __danmakuX_LuaParser_h__

#include "tolua_fix.h"
#include "CCLuaEngine.h"
#include "LuaBasicConversions.h"
#include "TypeTraits.h"

/*
 * template<typename T> bool dx_lua_checkType(lua_State *L, int idx);
 */

template<typename P, DX_ENABLE_IF_IS_REF(P)>
bool dx_lua_checkType(lua_State *L, int idx) {
    return lua_istable(L, idx);
}

template<typename T, DX_ENABLE_IF_IS_CLASS(T)>
bool dx_lua_checkType(lua_State *L, int idx) {
    return lua_istable(L, idx);
}

template<typename T, DX_ENABLE_IF_IS_INT(T)>
bool dx_lua_checkType(lua_State *L, int idx) {
    return lua_isnumber(L, idx);
}

template<typename T, DX_ENABLE_IF_IS_FLOAT(T)>
bool dx_lua_checkType(lua_State *L, int idx) {
    return lua_isnumber(L, idx);
}

template<typename T, DX_ENABLE_IF_IS_REF_VEC(T)>
bool dx_lua_checkType(lua_State *L, int idx) {
    return lua_istable(L, idx);
}

template<typename T, DX_ENABLE_IF_IS_VEC(T)>
bool dx_lua_checkType(lua_State *L, int idx) {
    return lua_istable(L, idx);
}

template<typename T, DX_ENABLE_IF_IS_FUNC(T)>
bool dx_lua_checkType(lua_State *L, int idx) {
    return lua_isfunction(L, idx);
}

template<>
inline bool dx_lua_checkType<std::string>(lua_State *L, int idx) {
    return lua_isstring(L, idx);
}

/*
 * template<typename T> T dx_lua_parseEl(lua_State *L, int idx);
 */

template<typename T> class StructLP;
template<typename T>
DX_ENABLE_IF_IS_CLASS_TYPE(T) dx_lua_parseEl(lua_State *L, int idx) {
    T v;
    StructLP<T>::parse(&v, L, idx);
    return v;
}

template <typename P>
DX_ENABLE_IF_IS_REF_TYPE(P) dx_lua_parseEl(lua_State *L, int idx) {
    typedef typename std::remove_pointer<P>::type T;
    auto v = new (std::nothrow) T();
    if (v && StructLP<T>::parse(v, L, idx)) {
        v->autorelease();
        return v;
    }
    CC_SAFE_DELETE(v);
    return nullptr;
}

template<typename VecT>
DX_ENABLE_IF_IS_REF_VEC_TYPE(VecT) dx_lua_parseEl(lua_State *L, int idx) {
    typedef typename VecT::iterator::value_type T;
    VecT vs;
    int N = lua_objlen(L, idx);
    for (int n = 1; n <= N; n++) {
        lua_pushinteger(L, n);
        lua_gettable(L, (idx < 0 ? idx - 1 : idx));
        auto v = dx_lua_parseEl<T>(L, -1);
        vs.pushBack(v);
        lua_pop(L, 1);
    }
    return vs;
}

template<typename VecT>
DX_ENABLE_IF_IS_VEC_TYPE(VecT) dx_lua_parseEl(lua_State *L, int idx) {
    typedef typename VecT::value_type T;
    VecT vs;
    int N = lua_objlen(L, idx);
    for (int n = 1; n <= N; n++) {
        lua_pushinteger(L, n);
        lua_gettable(L, (idx < 0 ? idx - 1 : idx));
        auto v = dx_lua_parseEl<T>(L, -1);
        vs.emplace_back(v);
        lua_pop(L, 1);
    }
    return vs;
}

template<typename T>
DX_ENABLE_IF_IS_FLOAT_TYPE(T) dx_lua_parseEl(lua_State *L, int idx) {
    return T(lua_tonumber(L, idx));
}

template<typename T>
DX_ENABLE_IF_IS_ENUM_TYPE(T) dx_lua_parseEl(lua_State *L, int idx) {
    return static_cast<T>(lua_tointeger(L, idx));
}

template<typename T>
DX_ENABLE_IF_IS_INT_TYPE(T) dx_lua_parseEl(lua_State *L, int idx) {
    return T(lua_tointeger(L, idx));
}

template<>
inline std::string dx_lua_parseEl<std::string>(lua_State *L, int idx) {
    return std::string(lua_tostring(L, idx));
}

template<>
inline bool dx_lua_parseEl<bool>(lua_State *L, int idx) {
    return lua_toboolean(L, idx);
}

template<typename T,
         typename std::enable_if<std::is_enum<T>::value, T>::type Unused = T()>
void dx_lua_push(lua_State* L, const T& v) {
    lua_pushinteger(L, static_cast<int>(v));
}

template<typename T, DX_ENABLE_IF_IS_INT(T)>
void dx_lua_push(lua_State* L, const T& v) {
    lua_pushinteger(L, v);
}

template<typename T, DX_ENABLE_IF_IS_FLOAT(T)>
void dx_lua_push(lua_State* L, const T& v) {
    lua_pushnumber(L, v);
}

template<>
inline void dx_lua_push(lua_State *L, const bool &v) {
    lua_pushboolean(L, v);
}

template<typename R>
inline R dx_call_lua_function_by_handler(
        cocos2d::LuaStack* stack, int handler, int nargs) {
    R res;
    stack->executeFunction(handler, nargs, 1, [&res](lua_State* ls, int numRes) {
        res = dx_lua_parseEl<R>(ls, -1);
    });
    stack->clean();
    return res;
}

template<>
inline void dx_call_lua_function_by_handler<void>(
        cocos2d::LuaStack* stack, int handler, int nargs) {
    stack->executeFunctionByHandler(handler, nargs);
    stack->clean();
}

template<typename R, typename Head, typename ...Tail>
inline R dx_call_lua_function_by_handler(
        cocos2d::LuaStack* stack, int handler, int nargs,
        Head arg, Tail... args) {
    dx_lua_push<Head>(stack->getLuaState(), arg);
    return dx_call_lua_function_by_handler<R, Tail...>(stack, handler, nargs + 1, args...);
}

template<typename T> struct dx_lua_func_wrapper;
template<typename R, typename ...Args>
struct dx_lua_func_wrapper<std::function<R(Args...)>> : public std::function<R(Args...)> {
    typedef std::function<R(Args...)> func_t;
    dx_lua_func_wrapper(int handler) : func_t([=](Args... args) -> R {
            auto engine = cocos2d::LuaEngine::getInstance();
            auto stack = engine->getLuaStack();
            return dx_call_lua_function_by_handler<R, Args...>(stack, handler, 0, args...);
    }) {}
};

template<typename T>
DX_ENABLE_IF_IS_FUNC_TYPE(T) dx_lua_parseEl(lua_State *L, int idx) {
    int fidx = (idx < 0 ? lua_gettop(L) + idx + 1 : idx);
    LUA_FUNCTION handler = (toluafix_ref_function(L,fidx,0));
    return dx_lua_func_wrapper<T>(handler);
}

template<typename T>
class FieldLP_ {
public:
    FieldLP_(bool required) :
        m_required(required)
    {}
    virtual ~FieldLP_() {}
    virtual bool parse(T* obj, lua_State *L, int idx) = 0;
    bool required() const { return m_required; }
protected:
    bool m_required = false;
};

template<typename T1,
         typename T2>
class FieldLP : public FieldLP_<T1> {
public:
  FieldLP(T2 T1::*ptr) :
        FieldLP_<T1>(false),
        m_ptr(ptr)
  {}
  FieldLP(T2 T1::* ptr, bool required) :
      FieldLP_<T1>(required),
      m_ptr(ptr)
  {}
  bool parse(T1* obj, lua_State *L, int idx) {
      if (!dx_lua_checkType<T2>(L, idx)) {
          return false;
      }
      (obj->*(m_ptr)) = dx_lua_parseEl<T2>(L, idx);
      return true;
  }
protected:
  T2 T1::* m_ptr = nullptr;
};

template <typename T1,
          typename T2>
class RefFieldLP : FieldLP_<T1> {
public:
  RefFieldLP(T2 T1::*ptr) :
      FieldLP_<T1>(false),
      m_ptr(ptr)
  {}
  RefFieldLP(T2 T1::* ptr, bool required) :
      FieldLP_<T1>(required),
      m_ptr(ptr)
  {}
  bool parse(T1* obj, lua_State *L, int idx) {
      if (!dx_lua_checkType<T2>(L, idx)) {
          return false;
      }
      auto ptr = dx_lua_parseEl<T2>(L, idx);
      auto prev = obj->*(m_ptr);
      if (prev) {
          CC_SAFE_RELEASE_NULL(prev);
      }
      (obj->*(m_ptr)) = ptr;
      return true;
  }
protected:
  T2 T1::* m_ptr = nullptr;
};

template<typename T>
class StructLP {
    typedef std::unique_ptr<FieldLP_<T>> FieldLP_Ptr;
    typedef std::unordered_map<std::string, FieldLP_Ptr> FieldP_MapT;
    static FieldP_MapT FieldP_Map;
public:
    template<typename T2>
    static bool addField(const std::string& key, T2 T::* ptr, bool required) {
        FieldP_Map.emplace(key, FieldLP_Ptr(new FieldLP<T, T2>(ptr, required)));
        return true;
    }
    template<typename T2>
    static bool addRefField(const std::string& key, T2 T::* ptr, bool required) {
        FieldP_Map.emplace(key, FieldLP_Ptr(new RefFieldLP<T, T2>(ptr, required)));
        return true;
    }
    static bool parse(T* obj, lua_State *L, int idx) {
        if (!dx_lua_checkType<T>(L, idx)) {
            cocos2d::log("Invalid object type for %s", typeid(T).name());
            return false;
        }
        auto fp_begin = std::begin(FieldP_Map);
        auto fp_end = std::end(FieldP_Map);
        for (auto fp_it = fp_begin; fp_it != fp_end; ++fp_it) {
            auto key = fp_it->first.c_str();
            auto& fp = fp_it->second;
            lua_pushstring(L, key);
            lua_gettable(L, (idx < 0 ? idx - 1 : idx));
            if (!lua_isnil(L, -1)) {
                if (!fp->parse(obj, L, -1)) {
                    cocos2d::log("Error parsing field %s", key);
                    lua_pop(L, 1);
                    return false;
                }
            } else if (fp->required()) {
                cocos2d::log("Required field %s is missing", key);
                lua_pop(L, 1);
                return false;
            }
            lua_pop(L, 1);
        }
        return true;
    }
};

#define DX_LUA_DESC_GET_MEMBER(L, idx, klass, member) \
    do { \
        using namespace std; \
        lua_pushstring(L, #member); \
        lua_gettable(L, (idx < 0 ? idx - 1 : idx)); \
        if (lua_isnil(L, -1)) { \
            cocos2d::log(#member" property of type '"#klass"' is not defined"); \
            lua_pop(L, 1); \
            return false; \
        } else { \
            m_##member = dx_lua_parseEl<klass>(L, -1); \
            lua_pop(L, 1); \
        } \
    } while (0)

#define DX_LUA_DESC_GET_OPT_MEMBER(L, idx, klass, member) \
    do { \
        using namespace std; \
        lua_pushstring(L, #member); \
        lua_gettable(L, (idx < 0 ? idx - 1 : idx)); \
        if (!lua_isnil(L, -1)) { \
            m_##member = dx_lua_parseEl<klass>(L, -1); \
        } \
        lua_pop(L, 1); \
    } while(0)

#define DX_LUA_DESC_GET_REF_MEMBER(L, idx, klass, member) \
    do { \
        lua_pushstring(L, #member); \
        lua_gettable(L, (idx < 0 ? idx - 1 : idx)); \
        if (lua_isnil(L, -1)) { \
            cocos2d::log(#member" property of type '"#klass"' is not defined"); \
            lua_pop(L, 1); \
            return false; \
        } else { \
            m_##member = klass::fromLua(L, -1); \
            lua_pop(L, 1); \
            if (!m_##member) { \
                cocos2d::log("Can't create member '"#member"'"); \
                return false; \
            } \
        } \
    } while(0)

#define DX_LUA_DESC_GET_OPT_REF_MEMBER(L, idx, klass, member) \
    do { \
        lua_pushstring(L, #member); \
        lua_gettable(L, (idx < 0 ? idx - 1 : idx)); \
        if (!lua_isnil(L, -1)) { \
            m_##member = klass::fromLua(L, -1); \
            if (!m_##member) { \
                cocos2d::log("Can't create member '"#member"'"); \
                lua_pop(L, 1); \
                return false; \
            } \
        } \
        lua_pop(L, 1); \
    } while (0)

#define DX_LUA_DESC_GET_VEC(L, idx, klass, member, vec) \
    do { \
        lua_pushstring(L, #member); \
        lua_gettable(L, (idx < 0 ? idx - 1 : idx)); \
        if (lua_isnil(L, -1)) { \
            cocos2d::log("'"#klass"' should define '"#member"' member array"); \
            lua_pop(L, 1); \
            return false; \
        } else { \
            int N = lua_objlen(L, -1); \
            for (int n = 1; n <= N; n++) { \
                lua_pushinteger(L, n); \
                lua_gettable(L, -2); \
                auto klass##_obj = klass::fromLua(L, -1); \
                if (!klass##_obj) { \
                    cocos2d::log(#klass"::fromLua returned nullptr"); \
                    lua_pop(L, 2); \
                    return false; \
                } \
                vec.pushBack(klass##_obj); \
                lua_pop(L, 1); \
            } \
        } \
        lua_pop(L, 1); \
    } while (0)

#define DX_LUA_DESC_GET_OPT_VEC(el, klass, member, vec) \
    do { \
        lua_pushstring(L, #member); \
        lua_gettable(L, (idx < 0 ? idx - 1 : idx)); \
        if (!lua_isnil(L, lua_gettop(L))) { \
            int N = lua_objlen(L, -1); \
            for (int n = 1; n <= N; n++) { \
                lua_pushinteger(L, n); \
                lua_gettable(L, -2); \
                auto klass##_obj = klass::fromLua(L, -1); \
                if (!klass##_obj) { \
                    cocos2d::log(#klass"::fromLua returned nullptr"); \
                    return false; \
                    lua_pop(L, 2); \
                } \
                vec.pushBack(klass##_obj); \
                lua_pop(L, 1); \
            } \
        } \
        lua_pop(L, 1); \
    } while (0)

#define DX_LUA_DESC_PROCESS_VEC(L, idx, klass, member, P) \
    do { \
        lua_pushstring(L, #member); \
        lua_gettable(L, (idx < 0 ? idx - 1 : idx)); \
        if (lua_isnil(L, -1)) { \
            cocos2d::log(#klass" should define '"#member"' property"); \
            lua_pop(L, 1); \
            return false; \
        } else { \
            auto objMgr = ObjectManager::getInstance(); \
            int N = lua_objlen(L, -1); \
            for (int n = 1; n <= N; n++) { \
                lua_pushinteger(L, n); \
                lua_gettable(L, -2); \
                auto klass##_obj = klass::fromLua(L, -1); \
                if (!klass##_obj) { \
                    cocos2d::log(#klass"::fromLua returned nullptr"); \
                    lua_pop(L, 2); \
                    return false; \
                } \
                P() \
                lua_pop(L, 1); \
            } \
        } \
        lua_pop(L, 1); \
    } while (0)

#define DX_LUA_DESC_PROCESS_OPT_VEC(L, idx, klass, member, P) \
    do { \
        lua_pushstring(L, #member); \
        lua_gettable(L, (idx < 0 ? idx - 1 : idx)); \
        if (!lua_isnil(L, -1)) { \
            auto objMgr = ObjectManager::getInstance(); \
            int N = lua_objlen(L, -1); \
            for (int n = 1; n <= N; n++) { \
                lua_pushinteger(L, n); \
                lua_gettable(L, -2); \
                auto klass##_obj = klass::fromLua(L, lua_gettop(L)); \
                if (!klass##_obj) { \
                    cocos2d::log(#klass"::fromLua returned nullptr"); \
                    lua_pop(L, 2); \
                    return false; \
                } \
                P() \
                lua_pop(L, 1); \
            } \
        } \
        lua_pop(L, 1); \
    } while (0)

#endif /* __danmakuX_LuaParser_h__ */
