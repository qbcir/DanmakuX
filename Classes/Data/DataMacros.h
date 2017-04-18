#ifndef __dx_DATAMACROS_H__
#define __dx_DATAMACROS_H__

#define DX_DEFINE_FROM_JSON_FILE_FUNC(klass) \
    static klass* fromJsonFile(const std::string& filename)
#define DX_DEFINE_FROM_JSON_FUNC(klass) \
    static klass* fromJson(const rapidjson::Value& el)

#define DX_CREATE_FROM_JSON_FUNC(klass) \
static klass* fromJson(const rapidjson::Value& el) { \
  klass* v = new (std::nothrow) klass(); \
  if (v && v->initFromJson(el)) { \
    v->autorelease(); \
    return v; \
  } \
  CC_SAFE_DELETE(v); \
  return nullptr; \
}

#define DX_CREATE_FROM_JSON_FUNC_(klass) \
klass* klass::fromJson(const rapidjson::Value& el) { \
  klass* v = new (std::nothrow) klass(); \
  if (v && v->initFromJson(el)) { \
    v->autorelease(); \
    return v; \
  } \
  CC_SAFE_DELETE(v); \
  return nullptr; \
}

#define DX_CREATE_FROM_JSON_FILE_FUNC(klass) \
klass* klass::fromJsonFile(const std::string& filename) { \
    rapidjson::Document doc; \
    bool ok = parseJsonDocument(doc, filename); \
    CCASSERT(ok, "Can't parse '"#klass"'"); \
    const rapidjson::Value& el = *static_cast<const rapidjson::Value*>(&doc); \
    return klass::fromJson(el); \
}

#define DX_CREATE_FROM_LUA_FUNC(klass) \
static klass* fromLua(lua_State* L, int idx) { \
    klass* v = new (std::nothrow) klass(); \
    if (v && v->initFromLua(L, idx)) { \
        v->autorelease(); \
        return v; \
    } \
    CC_SAFE_DELETE(v); \
    return nullptr; \
}

#endif // __dx_DATAMACROS_H__
