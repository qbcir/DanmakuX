#ifndef __danmakuX_TypeTraits_h__
#define __danmakuX_TypeTraits_h__

#include "cocos2d.h"
#include <vector>
#include <unordered_map>
#include <type_traits>

template<typename P>
struct dx_is_ref {
    static const bool value = std::is_pointer<P>::value
            && std::is_base_of<cocos2d::Ref, typename std::remove_pointer<P>::type>::value;
};

template<typename T>
struct dx_is_ref_vec {
    static const bool value = false;
};

template<typename T>
struct dx_is_ref_vec<cocos2d::Vector<T>> {
    static const bool value = true;
};

template<typename T>
struct dx_is_vec {
    static const bool value = false;
};

template<typename T>
struct dx_is_vec<std::vector<T>> {
    static const bool value = true;
};

template<typename T>
struct dx_is__vec {
    static const bool value = dx_is_ref_vec<T>::value || dx_is_vec<T>::value;
};

template<typename T>
struct dx_is_cc_value {
    static const bool value = std::is_same<T, cocos2d::Value>::value;
};

template<typename T>
struct dx_is_cc_value_vec {
    static const bool value = std::is_same<T, cocos2d::ValueVector>::value;
};

template<typename T>
struct dx_is_cc_value_map {
    static const bool value = std::is_same<T, cocos2d::ValueMap>::value;
};

template<typename T>
struct dx_is_func {
    static const bool value = false;
};

template<typename R, typename ...Args>
struct dx_is_func<std::function<R(Args...)>> {
    static const bool value = true;
};

template<typename T>
struct dx_is_class {
    static const bool value = std::is_class<T>::value
            && !dx_is_func<T>::value
            && !dx_is_ref_vec<T>::value
            && !dx_is_vec<T>::value
            && !dx_is_cc_value<T>::value
            && !dx_is_cc_value_vec<T>::value
            && !dx_is_cc_value_map<T>::value;
};

#define DX_ENABLE_IF_IS_FUNC_TYPE(T) \
    typename std::enable_if<dx_is_func<T>::value, T>::type
#define DX_ENABLE_IF_IS_REF_TYPE(T) \
    typename std::enable_if<dx_is_ref<T>::value, T>::type
#define DX_ENABLE_IF_IS_CLASS_TYPE(T) \
    typename std::enable_if<dx_is_class<T>::value, T>::type
#define DX_ENABLE_IF_IS_INT_TYPE(T) \
    typename std::enable_if<std::is_integral<T>::value, T>::type
#define DX_ENABLE_IF_IS_ENUM_TYPE(T) \
    typename std::enable_if<std::is_enum<T>::value, T>::type
#define DX_ENABLE_IF_IS_UINT_TYPE(T) \
    typename std::enable_if<std::is_integral<T>::value && std::is_unsigned<T>::value, T>::type
#define DX_ENABLE_IF_IS_SINT_TYPE(T) \
    typename std::enable_if<std::is_integral<T>::value && std::is_signed<T>::value, T>::type
#define DX_ENABLE_IF_IS_FLOAT_TYPE(T) \
    typename std::enable_if<std::is_floating_point<T>::value ,T>::type
#define DX_ENABLE_IF_IS_REF_VEC_TYPE(T) \
    typename std::enable_if<dx_is_ref_vec<T>::value, T>::type
#define DX_ENABLE_IF_IS_VEC_TYPE(T) \
    typename std::enable_if<dx_is_vec<T>::value, T>::type
#define DX_ENABLE_IF_IS_CC_VALUE_TYPE(T) \
    typename std::enable_if<dx_is_cc_value<T>::value, T>::type
#define DX_ENABLE_IF_IS_CC_VALUE_VEC_TYPE(T) \
    typename std::enable_if<dx_is_cc_value_vec<T>::value, T>::type
#define DX_ENABLE_IF_IS_CC_VALUE_MAP_TYPE(T) \
    typename std::enable_if<dx_is_cc_value_map<T>::value, T>::type
#define DX_ENABLE_IF_IS_FUNC(T) \
    DX_ENABLE_IF_IS_FUNC_TYPE(T)* = nullptr
#define DX_ENABLE_IF_IS_REF(T) \
    DX_ENABLE_IF_IS_REF_TYPE(T)* = nullptr
#define DX_ENABLE_IF_IS_CLASS(T) \
    DX_ENABLE_IF_IS_CLASS_TYPE(T)* = nullptr
#define DX_ENABLE_IF_IS_FLOAT(T) \
    DX_ENABLE_IF_IS_FLOAT_TYPE(T)* = nullptr
#define DX_ENABLE_IF_IS_INT(T) \
    DX_ENABLE_IF_IS_INT_TYPE(T)* = nullptr
#define DX_ENABLE_IF_IS_ENUM(T) \
    DX_ENABLE_IF_IS_ENUM_TYPE(T)* = nullptr
#define DX_ENABLE_IF_IS_SINT(T) \
    DX_ENABLE_IF_IS_SINT_TYPE(T)* = nullptr
#define DX_ENABLE_IF_IS_UINT(T) \
    DX_ENABLE_IF_IS_UINT_TYPE(T)* = nullptr
#define DX_ENABLE_IF_IS_REF_VEC(T) \
    DX_ENABLE_IF_IS_REF_VEC_TYPE(T)* = nullptr
#define DX_ENABLE_IF_IS_VEC(T) \
    DX_ENABLE_IF_IS_VEC_TYPE(T)* = nullptr
#define DX_ENABLE_IF_IS_CC_VALUE(T) \
    DX_ENABLE_IF_IS_CC_VALUE_TYPE(T)* = nullptr
#define DX_ENABLE_IF_IS_CC_VALUE_VEC(T) \
    DX_ENABLE_IF_IS_CC_VALUE_VEC_TYPE(T)* = nullptr
#define DX_ENABLE_IF_IS_CC_VALUE_MAP(T) \
    DX_ENABLE_IF_IS_CC_VALUE_MAP_TYPE(T)* = nullptr


#endif /* __danmakuX_TypeTraits_h__ */
