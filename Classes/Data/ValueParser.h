#ifndef __dx_VALUEPARSER_H__
#define __dx_VALUEPARSER_H__

#include "cocos2d.h"
#include "TypeTraits.h"

template<typename T, DX_ENABLE_IF_IS_CLASS(T)>
bool dx_cc_value_checkType(const cocos2d::Value &v) {
    return v.getType() == cocos2d::Value::Type::MAP;
}
//
template<typename T, DX_ENABLE_IF_IS_REF(T)>
bool dx_cc_value_checkType(const cocos2d::Value &v) {
    return v.getType() == cocos2d::Value::Type::MAP;
}
//
template<typename T, DX_ENABLE_IF_IS_REF_VEC(T)>
bool dx_cc_value_checkType(const cocos2d::Value &v) {
    return v.getType() == cocos2d::Value::Type::VECTOR;
}
//
template<typename T, DX_ENABLE_IF_IS_VEC(T)>
bool dx_cc_value_checkType(const cocos2d::Value &v) {
    return v.getType() == cocos2d::Value::Type::VECTOR;
}
//
template<typename T, DX_ENABLE_IF_IS_INT(T)>
bool dx_cc_value_checkType(const cocos2d::Value &v) {
    return v.getType() == cocos2d::Value::Type::INTEGER;
}
//
template<typename T, DX_ENABLE_IF_IS_FLOAT(T)>
bool dx_cc_value_checkType(const cocos2d::Value &v) {
    return v.getType() == cocos2d::Value::Type::FLOAT;
}
//
template<>
inline bool dx_cc_value_checkType<std::string>(const cocos2d::Value &v) {
    return v.getType() == cocos2d::Value::Type::STRING;
}
//
template<>
inline bool dx_cc_value_checkType<bool>(const cocos2d::Value &v) {
    return v.getType() == cocos2d::Value::Type::BOOLEAN;
}
//
template<typename T> class StructCCVP;
template <typename P>
DX_ENABLE_IF_IS_REF_TYPE(P) dx_cc_value_parse(const cocos2d::Value& v) {
    typedef typename std::remove_pointer<P>::type T;
    auto res = new (std::nothrow) T();
    if (res && StructCCVP<T>::parse(res, v)) {
        res->autorelease();
        return res;
    }
    CC_SAFE_DELETE(res);
    return nullptr;
}
//
template<typename T>
DX_ENABLE_IF_IS_CLASS_TYPE(T) dx_cc_value_parse(const cocos2d::Value& v) {
    T res;
    StructCCVP<T>::parse(&res, v);
    return res;
}
//
template<typename T>
DX_ENABLE_IF_IS_INT_TYPE(T) dx_cc_value_parse(const cocos2d::Value& v) {
    return T(v.asInt());
}
//
template<typename T>
DX_ENABLE_IF_IS_FLOAT_TYPE(T) dx_cc_value_parse(const cocos2d::Value& v) {
    return T(v.asFloat());
}
//
template<typename VecT>
DX_ENABLE_IF_IS_REF_VEC_TYPE(VecT) dx_cc_value_parse(const cocos2d::Value& v) {
    typedef typename VecT::iterator::value_type T;
    VecT vs;
    auto& vv = v.asValueVector();
    auto end_it = std::end(vv);
    for (auto it = std::begin(vv); it != end_it; ++it) {
        auto v = dx_cc_value_parse<T>(*it);
        vs.pushBack(v);
    }
    return vs;
}
//
template<typename VecT>
DX_ENABLE_IF_IS_VEC_TYPE(VecT) dx_cc_value_parse(const cocos2d::Value& v) {
    typedef typename VecT::value_type T;
    VecT vs;
    auto& vv = v.asValueVector();
    auto end_it = std::end(vv);
    for (auto it = std::begin(vv); it != end_it; ++it) {
        auto v = dx_cc_value_parse<T>(*it);
        vs.emplace_back(v);
    }
    return vs;
}
//
template<>
inline std::string dx_cc_value_parse<std::string>(const cocos2d::Value& v) {
    return v.asString();
}
//
template<>
inline uint8_t dx_cc_value_parse<uint8_t>(const cocos2d::Value& v) {
    return v.asByte();
}
//
template<>
inline bool dx_cc_value_parse<bool>(const cocos2d::Value& v) {
    return v.asBool();
}
//
template<typename T>
class FieldCCVP_ {
public:
    FieldCCVP_(bool required) :
        m_required(required)
    {}
    virtual ~FieldCCVP_() {}
    virtual bool parse(T* obj, const cocos2d::Value& v) = 0;
    bool required() const { return m_required; }
protected:
    bool m_required = false;
};
//
template<typename T1,
         typename T2>
class FieldCCVP : public FieldCCVP_<T1> {
public:
  FieldCCVP(T2 T1::*ptr) :
        m_ptr(ptr)
  {}
  FieldCCVP(T2 T1::* ptr, bool required) :
      FieldCCVP_<T1>(required),
      m_ptr(ptr)
  {}
  bool parse(T1* obj, const cocos2d::Value& v) {
      (obj->*(m_ptr)) = dx_cc_value_parse<T2>(v);
      return true;
  }
protected:
  T2 T1::* m_ptr = nullptr;
};
//
template <typename T1,
          typename T2>
class RefFieldCCVP : public FieldCCVP_<T1> {
public:
  RefFieldCCVP(T2 T1::*ptr) :
      m_ptr(ptr)
  {}
  RefFieldCCVP(T2 T1::* ptr, bool required) :
      FieldCCVP_<T1>(required),
      m_ptr(ptr)
  {}
  bool parse(T1* obj, const cocos2d::Value& v) {
      auto ptr = dx_cc_value_parse<T2>(v);
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
//
template <typename T1,
          typename T2>
class FuncFieldCCVP : public FieldCCVP_<T1> {
public:
    typedef std::function<void(T1*, const T2&)> SetFuncT;
    bool parse(T1* obj, const cocos2d::Value& v) {
        if (!dx_cc_value_checkType<T2>(v)) {
            cocos2d::log("Invalid object type: %u", v.getType());
            return false;
        }
        if (m_setF) {
            T2 v = dx_cc_value_parse<T2>(v);
            m_setF(obj, v);
            return true;
        }
        return false;
    }
protected:
    SetFuncT m_setF;
};
//
template<typename T>
class StructCCVP {
    typedef std::unique_ptr<FieldCCVP_<T>> FieldP_Ptr;
    typedef std::unordered_map<std::string, FieldP_Ptr> FieldP_MapT;
    static FieldP_MapT FieldP_Map;
public:
    template<typename T2>
    static bool addField(const std::string& key, T2 T::* ptr, bool required) {
        FieldP_Map.emplace(key, FieldP_Ptr(new FieldCCVP<T, T2>(ptr, required)));
        return true;
    }
    template<typename T2>
    static bool addRefField(const std::string& key, T2 T::* ptr, bool required) {
        FieldP_Map.emplace(key, FieldP_Ptr(new RefFieldCCVP<T, T2>(ptr, required)));
        return true;
    }
    static bool parse(T* obj, const cocos2d::Value& v) {
        if (!dx_cc_value_checkType<T>(v)) {
            cocos2d::log("Invalid object type: %d", int(v.getType()));
            return false;
        }
        auto fp_begin = std::begin(FieldP_Map);
        auto fp_end = std::end(FieldP_Map);
        auto& vm = v.asValueMap();
        auto member_end = vm.end();
        for (auto fp_it = fp_begin; fp_it != fp_end; ++fp_it) {
            auto key = fp_it->first.c_str();
            auto& fp = fp_it->second;
            auto member_it = vm.find(key);
            if (member_it != member_end) {
                if (!fp->parse(obj, member_it->second)) {
                    cocos2d::log("Error parsing field %s", key);
                    return false;
                }
            } else if (fp->required()) {
                cocos2d::log("Required field %s is missing", key);
                return false;
            }
        }
        return true;
    }
};

#endif // __dx_VALUEPARSER_H__
