#ifndef __danmakuX_DataParser_h__
#define __danmakuX_DataParser_h__

#include "JSONParser.h"
#include "LuaParser.h"
#include "ValueParser.h"
#include "FbParser.h"
#include "ByteParser.h"
#include "DataMacros.h"

#define DX_DEF_STRUCT_P(klass) \
    template<> StructP<klass>::FieldP_MapT StructP<klass>::FieldP_Map = StructP<klass>::FieldP_MapT(); \
    template<> StructLP<klass>::FieldP_MapT StructLP<klass>::FieldP_Map = StructLP<klass>::FieldP_MapT(); \
    template<> StructCCVP<klass>::FieldP_MapT StructCCVP<klass>::FieldP_Map = StructCCVP<klass>::FieldP_MapT(); \
    template<> StructFBP<klass>::FieldP_VecT StructFBP<klass>::FieldP_Vec = StructFBP<klass>::FieldP_VecT(); \
    template<> StructBP<klass>::FieldP_VecT StructBP<klass>::FieldP_Vec = StructBP<klass>::FieldP_VecT();
#define DX_ADD_OPT_FIELD_P(klass, member) \
    bool g_json_##klass##_##member##_added = StructP<klass>::addField(#member, &klass::member, false); \
    bool g_lua_##klass##_##member##_added = StructLP<klass>::addField(#member, &klass::member, false); \
    bool g_cc_value_##klass##_##member##_added = StructCCVP<klass>::addField(#member, &klass::member, false); \
    bool g_fb_##klass##_##member##_added = StructFBP<klass>::addField(&klass::member); \
    bool g_b_##klass##_##member##_added = StructBP<klass>::addField(&klass::member);
#define DX_ADD_FIELD_P(klass, member) \
    bool g_json_##klass##_##member##_added = StructP<klass>::addField(#member, &klass::member, true); \
    bool g_lua_##klass##_##member##_added = StructLP<klass>::addField(#member, &klass::member, true); \
    bool g_cc_value_##klass##_##member##_added = StructCCVP<klass>::addField(#member, &klass::member, true); \
    bool g_fb_##klass##_##member##_added = StructFBP<klass>::addField(&klass::member); \
    bool g_b_##klass##_##member##_added = StructBP<klass>::addField(&klass::member);
#define DX_ADD_OPT_REF_FIELD_P(klass, member) \
    bool g_json_##klass##_##member##_added = StructP<klass>::addRefField(#member, &klass::member, false); \
    bool g_lua_##klass##_##member##_added = StructLP<klass>::addRefField(#member, &klass::member, false); \
    bool g_cc_value_##klass##_##member##_added = StructCCVP<klass>::addRefField(#member, &klass::member, false); \
    bool g_fb_##klass##_##member##_added = StructFBP<klass>::addRefField(&klass::member); \
    bool g_b_##klass##_##member##_added = StructBP<klass>::addRefField(&klass::member);
#define DX_ADD_REF_FIELD_P(klass, member) \
    bool g_json_##klass##_##member##_added = StructP<klass>::addRefField(#member, &klass::member, true); \
    bool g_lua_##klass##_##member##_added = StructLP<klass>::addRefField(#member, &klass::member, true); \
    bool g_cc_value_##klass##_##member##_added = StructCCVP<klass>::addRefField(#member, &klass::member, true); \
    bool g_fb_##klass##_##member##_added = StructFBP<klass>::addRefField(&klass::member); \
    bool g_b_##klass##_##member##_added = StructBP<klass>::addRefField(&klass::member);

#endif /* __danmakuX_DataParser_h__ */
