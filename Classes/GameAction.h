#ifndef __danmakuX_GameAction_h__
#define __danmakuX_GameAction_h__

#include "cocos2d.h"
#include "json/document.h"
#include "Data/DataMacros.h"

enum class NumericExprType {
    FLOAT,
    EXPR,
    FUNC,
    INVALID = -1
};

class NumericExpr : public cocos2d::Ref {
public:
    typedef std::function<float()> FuncT;
    NumericExpr() {}
    NumericExpr(float v);
    NumericExpr(const FuncT& f);
    DX_CREATE_FROM_JSON_FUNC(NumericExpr)
    DX_CREATE_FROM_LUA_FUNC(NumericExpr)
    float eval() const;
protected:
    bool initFromJson(const rapidjson::Value& el);
    bool initFromLua(lua_State* L, int idx);
    NumericExprType m_type = NumericExprType::INVALID;
    float m_value;
    std::string m_expr;
    FuncT m_func;
};

template<typename T>
struct dx_is_num_expr {
    static const bool value = std::is_same<T, NumericExpr::FuncT>::value ||
        std::is_floating_point<T>::value;
};
#define DX_ENABLE_IF_IS_NUM_EXPR_TYPE(T) \
    typename std::enable_if<dx_is_num_expr<T>::value ,T>::type
#define DX_ENABLE_IF_IS_NUM_EXPR(T) \
    DX_ENABLE_IF_IS_NUM_EXPR_TYPE(T)* = nullptr

enum class NumericDescType {
    AIM,
    ABSOLUTE,
    RELATIVE,
    SEQUENCE,
    INVALID = -1
};

enum class GameActionType {
    ACTION,
    ACTIONREF,
    FIRE,
    FIREREF,
    CHANGE_SPEED,
    CHANGE_DIRECTION,
    ACCEL,
    VANISH,
    WAIT,
    REPEAT,
    FOREVER,
    WHILE,
    INVALID = -1
};

#define DX_CREATE_NUMERIC_DESC(klass) \
    template<typename T, DX_ENABLE_IF_IS_NUM_EXPR(T)> \
    static klass* create(const T& expr) { \
        auto nd = new (std::nothrow) klass(); \
        if (nd && nd->init(expr)) { \
            nd->autorelease(); \
            return nd; \
        } \
        CC_SAFE_DELETE(nd); \
        return nullptr; \
    }

class NumericDesc : public cocos2d::Ref {
public:
    DX_CREATE_NUMERIC_DESC(NumericDesc)
    DX_CREATE_FROM_JSON_FUNC(NumericDesc)
    DX_CREATE_FROM_LUA_FUNC(NumericDesc)
    virtual ~NumericDesc();
    virtual float eval();
    NumericDescType getType() const;
protected:
    virtual bool initFromJson(const rapidjson::Value& el);
    virtual bool initFromLua(lua_State* L, int idx);
    template<typename T, DX_ENABLE_IF_IS_NUM_EXPR(T)>
    bool init(const T& expr) {
        m_expr = new (std::nothrow) NumericExpr(expr);
        if (!m_expr) {
            return false;
        }
        return true;
    }
    NumericDescType m_type = NumericDescType::ABSOLUTE;
    NumericExpr* m_expr = nullptr;
};

class SpeedDesc : public NumericDesc {
public:
    DX_CREATE_NUMERIC_DESC(SpeedDesc)
    DX_CREATE_FROM_JSON_FUNC(SpeedDesc)
    DX_CREATE_FROM_LUA_FUNC(SpeedDesc)
};

class DirectionDesc : public NumericDesc {
public:
    DX_CREATE_NUMERIC_DESC(DirectionDesc)
    DX_CREATE_FROM_JSON_FUNC(DirectionDesc)
    DX_CREATE_FROM_LUA_FUNC(DirectionDesc)
};

class HorizontalDesc : public NumericDesc {
public:
    DX_CREATE_NUMERIC_DESC(HorizontalDesc)
    DX_CREATE_FROM_JSON_FUNC(HorizontalDesc)
    DX_CREATE_FROM_LUA_FUNC(HorizontalDesc)
};

class VerticalDesc : public NumericDesc {
public:
    DX_CREATE_NUMERIC_DESC(VerticalDesc)
    DX_CREATE_FROM_JSON_FUNC(VerticalDesc)
    DX_CREATE_FROM_LUA_FUNC(VerticalDesc)
};

class OrientationDesc : public NumericDesc {
public:
    DX_CREATE_NUMERIC_DESC(OrientationDesc)
    DX_CREATE_FROM_JSON_FUNC(OrientationDesc)
    DX_CREATE_FROM_LUA_FUNC(OrientationDesc)
};

class RotationDesc : public NumericDesc {
public:
    DX_CREATE_NUMERIC_DESC(RotationDesc)
    DX_CREATE_FROM_JSON_FUNC(RotationDesc)
    DX_CREATE_FROM_LUA_FUNC(RotationDesc)
};

enum PositionDescType {
    ABSOLUTE,
    ABSOLUTE_PERCENT,
    RANDOM_ABSOLUTE,
    RANDOM_TOP,
    RANDOM_BOTTOM,
    RANDOM_LEFT,
    RANDOM_RIGHT,
    TOP_PERCENT,
    BOTTOM_PERCENT,
    LEFT_PERCENT,
    RIGHT_PERCENT,
    RELATIVE,
    INVALID = -1
};

class PositionDesc : public cocos2d::Ref {
public:
    ~PositionDesc();
    DX_CREATE_FROM_JSON_FUNC(PositionDesc)
    DX_CREATE_FROM_LUA_FUNC(PositionDesc)
    cocos2d::Vec2 eval();
protected:
    bool initFromJson(const rapidjson::Value& el);
    bool initFromLua(lua_State* L, int idx);
    PositionDescType m_type = PositionDescType::INVALID;
    NumericExpr* m_x = nullptr;
    NumericExpr* m_y = nullptr;
};
#undef DX_CREATE_NUMERIC_DESC

class GameAction : public cocos2d::Ref {
public:
    virtual ~GameAction() {}
    virtual cocos2d::Action* getAction() { return nullptr; }//= 0;
    virtual const std::string getActionName() = 0;
    template<typename T>
    static GameAction* create(const rapidjson::Value& el) {
        typename std::enable_if<std::is_base_of<GameAction, T>::value, T>::type* v = T::fromJson(el);
        return v;
    }
    template<typename T>
    static GameAction* create(lua_State* L, int idx) {
        typename std::enable_if<std::is_base_of<GameAction, T>::value, T>::type* v = T::fromLua(L, idx);
        return v;
    }
    typedef GameAction* (*GameActionCreator)(const rapidjson::Value& el);
    typedef std::unordered_map<std::string, GameActionCreator> GameActionCreatorMapT;
    typedef GameAction* (*GameActionCreatorLua)(lua_State* L, int idx);
    typedef std::unordered_map<std::string, GameActionCreatorLua> GameActionCreatorLuaMapT;
    static GameActionCreatorMapT GameActionCreatorMap;
    static GameActionCreatorLuaMapT GameActionCreatorLuaMap;
    static GameAction* fromLua(const std::string& filename);
    static GameAction* fromLua(lua_State* L, int idx);
    static GameAction* fromJson(const rapidjson::Value& el);
    GameActionType getActionType() const { return m_type; }
    void setActionType(GameActionType type) { m_type = type; }
protected:
    GameActionType m_type = GameActionType::INVALID;
};

class RepeatDesc : public GameAction {
public:
    ~RepeatDesc();
    DX_CREATE_FROM_JSON_FUNC(RepeatDesc)
    DX_CREATE_FROM_LUA_FUNC(RepeatDesc)

    cocos2d::Action* getAction();
    const std::string getActionName() { return "repeat"; }
protected:
    bool initFromJson(const rapidjson::Value& el);
    bool initFromLua(lua_State* L, int idx);

    NumericExpr* m_times = nullptr;
    cocos2d::Vector<GameAction*> m_actions;
};

class ForeverDesc : public GameAction {
public:
    DX_CREATE_FROM_JSON_FUNC(ForeverDesc)
    DX_CREATE_FROM_LUA_FUNC(ForeverDesc)

    cocos2d::Action* getAction();
    const std::string getActionName() { return "forever"; }
protected:
    bool initFromJson(const rapidjson::Value& el);
    bool initFromLua(lua_State* L, int idx);
    cocos2d::Vector<GameAction*> m_actions;
};

class WhileDesc : public GameAction {
public:
    DX_CREATE_FROM_JSON_FUNC(WhileDesc)
    bool initFromJson(const rapidjson::Value& el);
    cocos2d::Action* getAction();
    const std::string getActionName() { return "while"; }
protected:
    std::string m_expr;
    cocos2d::Vector<GameAction*> m_actions;
};

class ActionDesc : public GameAction {
public:
    static ActionDesc* createFromActions(
            const std::string& label,
            const cocos2d::Vector<GameAction*>& actions);
    DX_CREATE_FROM_JSON_FUNC(ActionDesc)
    DX_CREATE_FROM_LUA_FUNC(ActionDesc)

    const std::string& getLabel() { return m_label; }
    void setLabel(const std::string& label) { m_label = label; }
    cocos2d::Action* getAction();
    const std::string getActionName() { return "action"; }
protected:
    bool initFromJson(const rapidjson::Value& el);
    bool initFromLua(lua_State* L, int idx);
    std::string m_label;
    cocos2d::Vector<GameAction*> m_actions;
};

class ActionRefDesc : public GameAction {
public:
    DX_CREATE_FROM_JSON_FUNC(ActionRefDesc)
    DX_CREATE_FROM_LUA_FUNC(ActionRefDesc)
    const std::string& getLabel() { return m_label; }
    cocos2d::Action* getAction();
    const std::string getActionName() { return "actionRef"; }
protected:
    bool initFromJson(const rapidjson::Value& el);
    bool initFromLua(lua_State* L, int idx);
    std::string m_label;
    cocos2d::Vector<GameAction*> m_actions;
};

class GameObject;
class GameObjectDesc;
class PlaceDescBase : public GameAction {
public:
    virtual ~PlaceDescBase();
    virtual float evalSpeed(GameObject* p);
    virtual float evalDirection(const cocos2d::Vec2& pos, GameObject *p);
    virtual void placeObject(GameObject* obj, cocos2d::Node* tgt);
protected:
    bool initFromJson(const rapidjson::Value& el);
    bool initFromLua(lua_State* L, int idx);
    int m_tag = -1;
    bool m_child = false;
    int m_zOrder = 1;
    RotationDesc* m_rotation = nullptr;
    OrientationDesc* m_orientation = nullptr;
    PositionDesc* m_position = nullptr;
    SpeedDesc* m_speed = nullptr;
    DirectionDesc* m_direction = nullptr;
    std::vector<std::string> m_components;
};

class PlaceObjectDesc : public PlaceDescBase {
public:
    DX_CREATE_FROM_JSON_FUNC(PlaceObjectDesc)
    DX_CREATE_FROM_LUA_FUNC(PlaceObjectDesc)
    ~PlaceObjectDesc();
    cocos2d::Action* getAction();
    const std::string getActionName() { return "placeObject"; }
protected:
    bool initFromJson(const rapidjson::Value& el);
    bool initFromLua(lua_State* L, int idx);
    GameObjectDesc* m_objectDesc = nullptr;
};

class VehicleDesc;
class PlaceVehicleDesc : public PlaceDescBase {
public:
    DX_CREATE_FROM_JSON_FUNC(PlaceVehicleDesc)
    DX_CREATE_FROM_LUA_FUNC(PlaceVehicleDesc)
    ~PlaceVehicleDesc();
    cocos2d::Action* getAction();
    const std::string getActionName() { return "placeVehicle"; }
protected:
    bool initFromJson(const rapidjson::Value& el);
    bool initFromLua(lua_State* L, int idx);
    VehicleDesc* m_vehicleDesc = nullptr;
};

class BulletDesc;
class FireBulletDesc : public PlaceDescBase {
public:
    DX_CREATE_FROM_JSON_FUNC(FireBulletDesc)
    DX_CREATE_FROM_LUA_FUNC(FireBulletDesc)
    ~FireBulletDesc();
    cocos2d::Action* getAction();
    const std::string getActionName() { return "fireBullet"; }
protected:
    bool initFromJson(const rapidjson::Value& el);
    bool initFromLua(lua_State* L, int idx);
    BulletDesc* m_bulletDesc = nullptr;
};

class PlaceObjectRefDesc : public PlaceDescBase {
public:
    DX_CREATE_FROM_JSON_FUNC(PlaceObjectRefDesc)
    DX_CREATE_FROM_LUA_FUNC(PlaceObjectRefDesc)
    const std::string& getLabel() { return m_label; }
    virtual cocos2d::Action* getAction();
    virtual const std::string getActionName() { return "placeObjectRef"; }
protected:
    bool initFromJson(const rapidjson::Value& el);
    bool initFromLua(lua_State* L, int idx);
    std::string m_label;
};

class PlaceVehicleRefDesc : public PlaceObjectRefDesc {
public:
    DX_CREATE_FROM_JSON_FUNC(PlaceVehicleRefDesc)
    DX_CREATE_FROM_LUA_FUNC(PlaceVehicleRefDesc)
    cocos2d::Action* getAction();
    const std::string getActionName() { return "placeVehicleRef"; }
protected:
    bool initFromJson(const rapidjson::Value& el);
    bool initFromLua(lua_State* L, int idx);
};

class FireBulletRefDesc : public PlaceObjectRefDesc {
public:
    DX_CREATE_FROM_JSON_FUNC(FireBulletRefDesc)
    DX_CREATE_FROM_LUA_FUNC(FireBulletRefDesc)
    cocos2d::Action* getAction();
    const std::string getActionName() { return "fireBulletRef"; }
protected:
    bool initFromJson(const rapidjson::Value& el);
    bool initFromLua(lua_State* L, int idx);
};

class BulletRefDesc : public cocos2d::Ref {
public:
    DX_CREATE_FROM_JSON_FUNC(BulletRefDesc)
    bool initFromJson(const rapidjson::Value& el);
    const std::string& getLabel() { return m_label; }
protected:
    std::string m_label;
};

class ChangeSpeedDesc : public GameAction {
public:
    class Action : public cocos2d::ActionInterval {
    public:
        void update(float dt);
    protected:
        cocos2d::Vec2 m_initV;
        cocos2d::Vec2 m_finV;
    };
    template<typename T1, typename T2,
             DX_ENABLE_IF_IS_NUM_EXPR(T1),
             DX_ENABLE_IF_IS_NUM_EXPR(T2)>
    static ChangeSpeedDesc* create(const T1& speed, const T2& term) {
        auto cs = new (std::nothrow) ChangeSpeedDesc();
        if (cs && cs->init(speed, term)) {
            cs->autorelease();
            return cs;
        }
        CC_SAFE_DELETE(cs);
        return nullptr;
    }
    DX_CREATE_FROM_JSON_FUNC(ChangeSpeedDesc)
    DX_CREATE_FROM_LUA_FUNC(ChangeSpeedDesc)
    ~ChangeSpeedDesc();
    const std::string getActionName() { return "changeSpeed"; }
protected:
    bool initFromJson(const rapidjson::Value& el);
    bool initFromLua(lua_State* L, int idx);
    template<typename T1, typename T2,
             DX_ENABLE_IF_IS_NUM_EXPR(T1),
             DX_ENABLE_IF_IS_NUM_EXPR(T2)>
    bool init(const T1& speed, const T2& term) {
        m_speed = SpeedDesc::create(speed);
        if (!m_speed) {
            return false;
        }
        CC_SAFE_RETAIN(m_speed);
        m_term = new (std::nothrow) NumericExpr(term);
        if (!m_term) {
            return false;
        }
        return true;
    }
    SpeedDesc* m_speed = nullptr;
    NumericExpr* m_term = nullptr;
};

class ChangeDirectionDesc : public GameAction {
public:
    class Action : public cocos2d::ActionInterval {
    public:
        Action(float direction, float dt);
        void update(float dt);
    protected:
        float m_delta = 0;
    };
    template<typename T1, typename T2,
             DX_ENABLE_IF_IS_NUM_EXPR(T1),
             DX_ENABLE_IF_IS_NUM_EXPR(T2)>
    static ChangeDirectionDesc* create(const T1& direction, const T2& term) {
        auto cd = new (std::nothrow) ChangeDirectionDesc();
        if (cd && cd->init(direction, term)) {
            cd->autorelease();
            return cd;
        }
        CC_SAFE_DELETE(cd);
        return nullptr;
    }
    DX_CREATE_FROM_JSON_FUNC(ChangeDirectionDesc)
    DX_CREATE_FROM_LUA_FUNC(ChangeDirectionDesc)
    ~ChangeDirectionDesc();
    const std::string getActionName() { return "changeDirection"; }
protected:
    bool initFromJson(const rapidjson::Value& el);
    bool initFromLua(lua_State* L, int idx);
    template<typename T1, typename T2,
             DX_ENABLE_IF_IS_NUM_EXPR(T1),
             DX_ENABLE_IF_IS_NUM_EXPR(T2)>
    bool init(const T1& dir, const T2& term) {
        m_direction = DirectionDesc::create(dir);
        if (!m_direction) {
            return false;
        }
        CC_SAFE_RETAIN(m_direction);
        m_term = new (std::nothrow) NumericExpr(term);
        if (!m_term) {
            return false;
        }
        return true;
    }
    DirectionDesc* m_direction = nullptr;
    NumericExpr* m_term = nullptr;
};

class AccelDesc : public GameAction {
public:
    class Action : public cocos2d::ActionInterval {
    public:
        static Action* create(const cocos2d::Vec2& accel, float dt);
        bool init(const cocos2d::Vec2& accel, float dt);
        void startWithTarget(cocos2d::Node* tgt);
        void stop();
        void update(float dt) {}
    protected:
        cocos2d::Vec2 m_accel;
    };
    template<typename T1, typename T2,
             DX_ENABLE_IF_IS_NUM_EXPR(T1),
             DX_ENABLE_IF_IS_NUM_EXPR(T2)>
    static AccelDesc* createWithHorizontal(
            const T1& h,
            const T2& term) {
        auto ad = new (std::nothrow) AccelDesc();
        if (ad && ad->initWithHorizontal(h, term)) {
            ad->autorelease();
            return ad;
        }
        CC_SAFE_DELETE(ad);
        return nullptr;
    }
    template<typename T1, typename T2,
             DX_ENABLE_IF_IS_NUM_EXPR(T1),
             DX_ENABLE_IF_IS_NUM_EXPR(T2)>
    static AccelDesc* createWithVertical(
            const T1& v,
            const T2& term) {
        auto ad = new (std::nothrow) AccelDesc();
        if (ad && ad->initWithVertical(v, term)) {
            ad->autorelease();
            return ad;
        }
        CC_SAFE_DELETE(ad);
        return nullptr;
    }
    template<typename T1, typename T2, typename T3,
             DX_ENABLE_IF_IS_NUM_EXPR(T1),
             DX_ENABLE_IF_IS_NUM_EXPR(T2),
             DX_ENABLE_IF_IS_NUM_EXPR(T3)>
    static AccelDesc* create(
            const T1& h,
            const T2& v,
            const T3& term) {
        auto ad = new (std::nothrow) AccelDesc();
        if (ad && ad->init(h, v, term)) {
            ad->autorelease();
            return ad;
        }
        CC_SAFE_DELETE(ad);
        return nullptr;
    }
    DX_CREATE_FROM_JSON_FUNC(AccelDesc)
    DX_CREATE_FROM_LUA_FUNC(AccelDesc)
    cocos2d::Action* getAction();
    ~AccelDesc();
    const std::string getActionName() { return "accel"; }
protected:
    bool initFromJson(const rapidjson::Value& el);
    bool initFromLua(lua_State* L, int idx);
    template<typename T1, typename T2, typename T3,
             DX_ENABLE_IF_IS_NUM_EXPR(T1),
             DX_ENABLE_IF_IS_NUM_EXPR(T2),
             DX_ENABLE_IF_IS_NUM_EXPR(T3)>
    bool init(const T1& h, const T2& v, const T3& term) {
        m_horizontal = HorizontalDesc::create(h);
        if (!m_horizontal) {
            return false;
        }
        CC_SAFE_RETAIN(m_horizontal);
        m_vertical = VerticalDesc::create(v);
        if (!m_vertical) {
            return false;
        }
        CC_SAFE_RETAIN(m_vertical);
        m_term = new (std::nothrow) NumericExpr(term);
        if (!m_term) {
            return false;
        }
        return true;
    }
    template <typename T1, typename T2,
              DX_ENABLE_IF_IS_NUM_EXPR(T1),
              DX_ENABLE_IF_IS_NUM_EXPR(T2)>
    bool initWithHorizontal(const T1& h, const T2& term) {
        m_horizontal = HorizontalDesc::create(h);
        if (!m_horizontal) {
            return false;
        }
        m_term = new (std::nothrow) NumericExpr(term);
        if (!m_term) {
            return false;
        }
        return true;
    }
    template <typename T1, typename T2,
              DX_ENABLE_IF_IS_NUM_EXPR(T1),
              DX_ENABLE_IF_IS_NUM_EXPR(T2)>
    bool initWithVertical(const T1& v, const T2& term) {
        m_vertical = VerticalDesc::create(v);
        if (!m_vertical) {
            return false;
        }
        m_term = new (std::nothrow) NumericExpr(term);
        if (!m_term) {
            return false;
        }
        return true;
    }
    HorizontalDesc* m_horizontal = nullptr;
    VerticalDesc* m_vertical = nullptr;
    NumericExpr* m_term = nullptr;
};

class VanishDesc : public GameAction {
public:
    DX_CREATE_FROM_JSON_FUNC(VanishDesc)
    DX_CREATE_FROM_LUA_FUNC(VanishDesc)
    cocos2d::Action* getAction();
    const std::string getActionName() { return "vanish"; }
protected:
    bool initFromJson(const rapidjson::Value& el);
    bool initFromLua(lua_State* L, int idx);
};

class WaitDesc : public GameAction {
public:
    ~WaitDesc();
    template<typename T, DX_ENABLE_IF_IS_NUM_EXPR(T)>
    static WaitDesc* create(const T& term) {
        auto wd = new (std::nothrow) WaitDesc();
        if (wd && wd->init(term)) {
            wd->autorelease();
            return wd;
        }
        CC_SAFE_DELETE(wd);
        return nullptr;
    }
    DX_CREATE_FROM_JSON_FUNC(WaitDesc)
    DX_CREATE_FROM_LUA_FUNC(WaitDesc)
    cocos2d::Action* getAction();
    inline const std::string getActionName() { return "wait"; }
protected:
    bool initFromJson(const rapidjson::Value& el);
    bool initFromLua(lua_State* L, int idx);

    template<typename T, DX_ENABLE_IF_IS_NUM_EXPR(T)>
    bool init(const T& term) {
        m_term = new (std::nothrow) NumericExpr(term);
        if (!m_term) {
            return false;
        }
        return true;
    }
    NumericExpr* m_term = nullptr;
};

#endif /* __danmakuX_GameAction_h__ */
