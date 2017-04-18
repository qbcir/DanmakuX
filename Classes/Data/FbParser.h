#ifndef FBPARSER_H
#define FBPARSER_H

#include "flatbuffers/flexbuffers.h"
#include "TypeTraits.h"
#include <functional>

template<typename T, DX_ENABLE_IF_IS_REF(T)>
bool dx_fb_checkType(const flexbuffers::Reference &ref) {
    return ref.IsVector();
}

template<typename T, DX_ENABLE_IF_IS_CLASS(T)>
bool dx_fb_checkType(const flexbuffers::Reference &ref) {
    return ref.IsVector();
}

template<typename T, DX_ENABLE_IF_IS_INT(T)>
bool dx_fb_checkType(const flexbuffers::Reference &ref) {
    return ref.IsIntOrUint();
}

template<typename T, DX_ENABLE_IF_IS_FLOAT(T)>
bool dx_fb_checkType(const flexbuffers::Reference &ref) {
    return ref.IsFloat();
}

template<typename T, DX_ENABLE_IF_IS_REF_VEC(T)>
bool dx_fb_checkType(const flexbuffers::Reference &ref) {
    return ref.IsVector();
}

template<typename T, DX_ENABLE_IF_IS_VEC(T)>
bool dx_fb_checkType(const flexbuffers::Reference &ref) {
    return ref.IsVector();
}

/*
 * template<typename T> T dx_fb_parse(const flexbuffers::Reference &ref);
 */
template<typename T> class StructFBP;
template<typename P>
DX_ENABLE_IF_IS_REF_TYPE(P) dx_fb_parse(const flexbuffers::Reference &ref) {
    typedef typename std::remove_pointer<P>::type T;
    auto v = new (std::nothrow) T();
    if (v && StructFBP<T>::parse(v, ref)) {
        v->autorelease();
        return v;
    }
    CC_SAFE_DELETE(v);
    return nullptr;
}
//
template<typename T>
DX_ENABLE_IF_IS_CLASS_TYPE(T) dx_fb_parse(const flexbuffers::Reference &ref) {
    T v;
    StructFBP<T>::parse(&v, ref);
    return v;
}
//
template<>
inline std::string dx_fb_parse<std::string>(const flexbuffers::Reference &ref) {
    return ref.AsString().str();
}
//
template<typename T>
DX_ENABLE_IF_IS_FLOAT_TYPE(T) dx_fb_parse(const flexbuffers::Reference &ref) {
    return T(ref.AsFloat());
}
//
template<typename T>
DX_ENABLE_IF_IS_INT_TYPE(T) dx_fb_parse(const flexbuffers::Reference &ref) {
    return T(ref.AsInt64());
}
//
template<>
inline int8_t dx_fb_parse<int8_t>(const flexbuffers::Reference &ref) {
    return ref.AsInt8();
}
//
template<>
inline int16_t dx_fb_parse<int16_t>(const flexbuffers::Reference &ref) {
    return ref.AsInt16();
}
//
template<>
inline int32_t dx_fb_parse<int32_t>(const flexbuffers::Reference &ref) {
    return ref.AsInt32();
}
//
template<>
inline int64_t dx_fb_parse<int64_t>(const flexbuffers::Reference &ref) {
    return ref.AsInt64();
}
template<>
inline uint8_t dx_fb_parse<uint8_t>(const flexbuffers::Reference &ref) {
    return ref.AsUInt8();
}
//
template<>
inline uint16_t dx_fb_parse<uint16_t>(const flexbuffers::Reference &ref) {
    return ref.AsUInt16();
}
//
template<>
inline uint32_t dx_fb_parse<uint32_t>(const flexbuffers::Reference &ref) {
    return ref.AsUInt32();
}
//
template<>
inline uint64_t dx_fb_parse<uint64_t>(const flexbuffers::Reference &ref) {
    return ref.AsUInt64();
}
//
template<>
inline bool dx_fb_parse<bool>(const flexbuffers::Reference &ref) {
    return bool(ref.AsUInt8());
}
//
template<typename VecT>
DX_ENABLE_IF_IS_REF_VEC_TYPE(VecT) dx_fb_parse(const flexbuffers::Reference &ref) {
    typedef typename VecT::iterator::value_type T;
    VecT vs;
    auto vec = ref.AsTypedVector();
    for (size_t i = 0; i < vec.size(); ++i) {
        auto v = dx_fb_parse<T>(vec[i]);
        vs.pushBack(v);
    }
    return vs;
}
//
template<typename VecT>
DX_ENABLE_IF_IS_VEC_TYPE(VecT) dx_fb_parse(const flexbuffers::Reference &ref) {
    typedef typename VecT::value_type T;
    VecT vs;
    auto vec = ref.AsTypedVector();
    for (size_t i = 0; i < vec.size(); ++i) {
        auto v = dx_fb_parse<T>(vec[i]);
        vs.emplace_back(v);
    }
    return vs;
}
/*
 *
 */
template<typename T, DX_ENABLE_IF_IS_CLASS(T)>
void dx_fb_dump(flexbuffers::Builder& b, const T& v) {
    StructFBP<T>::dump(b, &v);
}
//
template<typename P, DX_ENABLE_IF_IS_REF(P)>
void dx_fb_dump(flexbuffers::Builder& b, const P& v) {
    typedef typename std::remove_pointer<P>::type T;
    StructFBP<T>::dump(b, v);
}
//
template<typename T, DX_ENABLE_IF_IS_SINT(T)>
void dx_fb_dump(flexbuffers::Builder& b, const T& v) {
    b.Int(int64_t(v));
}
//
template<typename T, DX_ENABLE_IF_IS_UINT(T)>
void dx_fb_dump(flexbuffers::Builder& b, const T& v) {
    b.UInt(uint64_t(v));
}
//
template<>
inline void dx_fb_dump(flexbuffers::Builder &b, const bool &v) {
    b.Bool(v);
}
//
template<typename T, DX_ENABLE_IF_IS_FLOAT(T)>
inline void dx_fb_dump(flexbuffers::Builder& b, const T& v) {
    b.Float(v);
}
//
template<>
inline void dx_fb_dump(flexbuffers::Builder &b, const std::string &v) {
    b.String(v);
}
//
template<typename VecT, DX_ENABLE_IF_IS_VEC(VecT)>
void dx_fb_dump(flexbuffers::Builder& b, const VecT& vec) {
    typedef typename VecT::value_type T;
    auto start = b.StartVector();
    for (auto& v : vec) {
        dx_fb_dump<T>(b, v);
    }
    b.EndVector(start, true, false);
}
//
template<typename VecT, DX_ENABLE_IF_IS_REF_VEC(VecT)>
void dx_fb_dump(flexbuffers::Builder& b, const VecT& vec) {
    typedef typename VecT::iterator::value_type T;
    auto start = b.StartVector();
    for (const auto& v : vec) {
        dx_fb_dump<T>(b, v);
    }
    b.EndVector(start, true, false);
}
/*
 *
 */
template<typename T>
class FieldFBP_ {
public:
    virtual ~FieldFBP_() {}
    virtual void dump(flexbuffers::Builder& b, const T* obj) = 0;
    virtual bool parse(T* obj, const flexbuffers::Reference &ref) = 0;
};
//
template<typename T1,
         typename T2>
class FieldFBP : public FieldFBP_<T1> {
public:
  FieldFBP(T2 T1::*ptr) :
        m_ptr(ptr)
  {}
  void dump(flexbuffers::Builder& b, const T1* obj) {
      dx_fb_dump<T2>(b, (obj->*(m_ptr)));
  }
  bool parse(T1* obj, const flexbuffers::Reference &ref) {
      if (!dx_fb_checkType<T2>(ref)) {
          return false;
      }
      (obj->*(m_ptr)) = dx_fb_parse<T2>(ref);
      return true;
  }
protected:
  T2 T1::* m_ptr = nullptr;
};
//
template <typename T1,
          typename T2>
class RefFieldFBP : public FieldFBP_<T1> {
public:
  RefFieldFBP(T2 T1::*ptr) :
      m_ptr(ptr)
  {}
  void dump(flexbuffers::Builder& b, const T1* obj) {
      dx_fb_dump<T2>(b, (obj->*(m_ptr)));
  }
  bool parse(T1* obj, const flexbuffers::Reference &ref) {
      if (!dx_fb_checkType<T2>(ref)) {
          return false;
      }
      auto ptr = dx_fb_parse<T2>(ref);
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
class FuncFieldFBP : public FieldFBP_<T1> {
public:
    typedef std::function<void(T1*, const T2&)> SetFuncT;
    typedef std::function<const T2&(T1*)> GetFuncT;
    void dump(flexbuffers::Builder &b, const T1 *obj) {
        if (m_getF) {
            dx_fb_dump<T2>(b, m_getF(obj));
        }
    }
    bool parse(T1* obj, const flexbuffers::Reference &ref) {
        if (!dx_fb_checkType<T2>(ref)) {
            return false;
        }
        if (m_setF) {
            T2 v = dx_fb_parse<T2>(ref);
            m_setF(obj, v);
            return true;
        }
        return false;
    }
protected:
    SetFuncT m_setF;
    GetFuncT m_getF;
};
//
template<typename T>
class StructFBP {
    typedef std::unique_ptr<FieldFBP_<T>> FieldFBP_Ptr;
    typedef std::vector<FieldFBP_Ptr> FieldP_VecT;
    static FieldP_VecT FieldP_Vec;
public:
    template<typename T2>
    static bool addField(T2 T::* ptr) {
        FieldP_Vec.emplace_back(FieldFBP_Ptr(new FieldFBP<T, T2>(ptr)));
        return true;
    }
    template<typename T2>
    static bool addRefField(T2 T::* ptr) {
        FieldP_Vec.emplace_back(FieldFBP_Ptr(new RefFieldFBP<T, T2>(ptr)));
        return true;
    }
    static void dump(flexbuffers::Builder& b, const T* obj) {
        auto fp_begin = std::begin(FieldP_Vec);
        auto fp_end = std::end(FieldP_Vec);
        auto start = b.StartVector();
        for (auto fp_it = fp_begin; fp_it != fp_end; ++fp_it) {
            (*fp_it)->dump(b, obj);
        }
        b.EndVector(start, false, false);
    }
    static bool parse(uint64_t mask, T* obj, const flexbuffers::Reference &ref) {
    }
    static bool parse(T* obj, const flexbuffers::Reference &ref) {
        if (!dx_fb_checkType<T>(ref)) {
            cocos2d::log("Invalid object type for %s", typeid(T).name());
            return false;
        }
        auto fp_begin = std::begin(FieldP_Vec);
        auto fp_end = std::end(FieldP_Vec);
        for (auto fp_it = fp_begin; fp_it != fp_end; ++fp_it) {
            if (!(*fp_it)->parse(obj, ref)) {
                return false;
            }
        }
        return true;
    }
};
//
template<typename T>
class StructFBPartialP {
    typedef std::unique_ptr<FieldFBP_<T>> FieldFBP_Ptr;
    typedef std::unordered_map<std::string, FieldFBP_Ptr> FieldP_MapT;
    static FieldP_MapT FieldP_Map;
public:
    static bool parse(T* obj, const flexbuffers::Reference &ref) {
        if (!ref.IsMap()) {
            cocos2d::log("Invalid object type for %s", typeid(T).name());
            return false;
        }
        auto map = ref.AsMap();
        auto keys = map.Keys();
        auto values = map.Values();
        for (size_t i = 0; i < map.size(); ++i) {
            auto key = keys[i].AsString().str();
            auto val = values[i];
            auto fp_it = FieldP_Map.find(key);
            if (fp_it != std::end(FieldP_Map)) {
                auto& fp = fp_it->second;
                fp->parse(obj, val);
            }
        }
    }
};

#endif // FBPARSER_H
