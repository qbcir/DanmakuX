#include "JSONParser.h"

bool parseJsonDocument(rapidjson::Document& doc, const std::string& filename) {
    auto futils = cocos2d::FileUtils::getInstance();
    auto fpath = futils->fullPathForFilename(filename);
    auto content = futils->getStringFromFile(fpath);
    doc.Parse(content.c_str());
    auto errCode = doc.GetParseError();
    if (errCode != rapidjson::kParseErrorNone) {
        cocos2d::log("JSON parse error in %s",//: %s (%u)",
            filename.c_str());
        return false;
    }
    return true;
}

cocos2d::ValueMap dx_cc_value_map_parseEl(const rapidjson::Value& el) {
    cocos2d::ValueMap vm;
    auto end_it = el.MemberEnd();
    for (auto it = el.MemberBegin();
         it != end_it;
         ++it) {
        auto key = it->name.GetString();
        auto val = dx_cc_value_parseEl(it->value);
        vm[key] = val;
    }
    return vm;
}

cocos2d::ValueVector dx_cc_value_vec_parseEl(const rapidjson::Value& el) {
    cocos2d::ValueVector vv;
    for (rapidjson::SizeType i = 0; i < el.Size(); i++) {
        auto val = dx_cc_value_parseEl(el[i]);
        vv.emplace_back(val);
    }
    return vv;
}

cocos2d::Value dx_cc_value_parseEl(const rapidjson::Value& el) {
    switch (el.GetType()) {
        case rapidjson::kNullType:
            return cocos2d::Value();
        case rapidjson::kFalseType :
            return cocos2d::Value(false);
        case rapidjson::kTrueType :
            return cocos2d::Value(true);
        case rapidjson::kStringType :
            return cocos2d::Value(el.GetString());
        case rapidjson::kNumberType : {
                if (el.IsDouble()) {
                    return cocos2d::Value(el.GetDouble());
                } else if (el.IsInt()) {
                    return cocos2d::Value(el.GetInt());
                } else {
                    return cocos2d::Value();
                }
            }
        case rapidjson::kObjectType : {
                auto vm = dx_cc_value_map_parseEl(el);
                return cocos2d::Value(vm);
            }
        case rapidjson::kArrayType : {
                auto vv = dx_cc_value_vec_parseEl(el);
                return cocos2d::Value(vv);
            }
        default:;
    }
    return cocos2d::Value();
}
