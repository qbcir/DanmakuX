#include "GameAction.h"
#include "ObjectManager.h"
#include "ComponentManager.h"
#include "MovementComponent.h"
#include "Objects/GameObject.h"
#include "Objects/Vehicle.h"
#include "Objects/Bullet.h"
#include "Objects/PlayerObject.h"
#include "Layers/LevelLayer.h"
#include "Data/DataParser.h"

#include "CCLuaEngine.h"

NumericExpr::NumericExpr(float v) :
    m_type(NumericExprType::FLOAT),
    m_value(v)
{}

NumericExpr::NumericExpr(const FuncT& f) :
    m_type(NumericExprType::FUNC),
    m_func(f)
{}

bool NumericExpr::initFromJson(const rapidjson::Value& el) {
    if (el.IsInt()) {
        m_value = float(el.GetInt());
        m_type = NumericExprType::FLOAT;
    } else if (el.IsDouble()) {
        m_type = NumericExprType::FLOAT;
        m_value = float(el.GetDouble());
    } else if (el.IsString()) {
        m_type = NumericExprType::EXPR;
        m_expr = std::string(el.GetString());
    } else {
        return false;
    }
    return true;
}

bool NumericExpr::initFromLua(lua_State *L, int idx) {
    if (lua_isfunction(L, idx)) {
        m_type = NumericExprType::FUNC;
        int fidx = (idx < 0 ? lua_gettop(L) + idx + 1 : idx);
        LUA_FUNCTION handler = (toluafix_ref_function(L,fidx,0));
        m_func = FuncT([=](){
            cocos2d::LuaStack* stack = cocos2d::LuaEngine::getInstance()->getLuaStack();
            float res = 0;
            stack->executeFunction(handler, 0, 1, [&res](lua_State* ls, int numRes) {
                res = dx_lua_parseEl<float>(ls, -1);
            });
            stack->clean();
            return res;
        });
        cocos2d::ScriptHandlerMgr::getInstance()->addCustomHandler((void*)this, handler);
        return true;
    } else if (lua_isnumber(L, idx)) {
        m_type = NumericExprType::FLOAT;
        m_value = lua_tonumber(L, idx);
        return true;
    }
    return false;
}

float NumericExpr::eval() const {
    switch (m_type) {
        case NumericExprType::FLOAT :
            return m_value;
        case NumericExprType::FUNC :
            return m_func();
        default:
            return 0;
    }
}

std::unordered_map<std::string, PositionDescType> str2pdt {
    { "absolute", PositionDescType::ABSOLUTE },
    { "absolute-percent", PositionDescType::ABSOLUTE_PERCENT },
    { "random", PositionDescType::RANDOM_ABSOLUTE },
    { "random-top", PositionDescType::RANDOM_TOP }
};

PositionDesc::~PositionDesc() {
    CC_SAFE_RELEASE(m_x);
    CC_SAFE_RELEASE(m_y);
}

bool PositionDesc::initFromJson(const rapidjson::Value& el)
{
    auto type_it = el.FindMember("type");
    if (type_it == el.MemberEnd()) {
        m_type = PositionDescType::INVALID;
    } else {
        auto type = std::string(type_it->value.GetString());
        auto it = str2pdt.find(type);
        if (it != std::end(str2pdt)) {
            m_type = it->second;
        }
        CCASSERT(m_type != PositionDescType::INVALID, "");
    }
    DX_DESC_GET_OPT_REF_MEMBER(el, NumericExpr, x);
    DX_DESC_GET_OPT_REF_MEMBER(el, NumericExpr, y);
    CC_SAFE_RETAIN(m_x);
    CC_SAFE_RETAIN(m_y);
    return true;
}

bool PositionDesc::initFromLua(lua_State* L, int idx) {
    lua_pushstring(L, "type");
    lua_gettable(L, (idx < 0 ? idx - 1 : idx));
    if (lua_isnil(L, -1)) {
        m_type = PositionDescType::INVALID;
    } else {
        auto type = std::string(lua_tostring(L, -1));
        auto it = str2pdt.find(type);
        if (it != std::end(str2pdt)) {
            m_type = it->second;
        }
        CCASSERT(m_type != PositionDescType::INVALID, "");
    }
    lua_pop(L, 1);
    DX_LUA_DESC_GET_OPT_REF_MEMBER(L, idx, NumericExpr, x);
    DX_LUA_DESC_GET_OPT_REF_MEMBER(L, idx, NumericExpr, y);
    CC_SAFE_RETAIN(m_x);
    CC_SAFE_RETAIN(m_y);
    return true;
}

cocos2d::Vec2 PositionDesc::eval() {
    cocos2d::Vec2 res;
    cocos2d::Size size = cocos2d::Director::getInstance()->getWinSize();
    float x = m_x ? m_x->eval() : 0;
    float y = m_y ? m_y->eval() : 0;
    switch (m_type) {
        case PositionDescType::ABSOLUTE :
        case PositionDescType::RELATIVE :
            res.x = x;
            res.y = y;
            break;
        case PositionDescType::ABSOLUTE_PERCENT :
            res.x = x * size.width;
            res.y = y * size.height;
            break;
        case PositionDescType::RANDOM_TOP :
            res.x = cocos2d::RandomHelper::random_int(0, int(size.width));
            res.y = size.height;
        default:;
    }
    return res;
}

std::unordered_map<std::string, NumericDescType> str2ndt {
    { "absolute", NumericDescType::ABSOLUTE },
    { "relative", NumericDescType::RELATIVE },
    { "aim", NumericDescType::AIM }
};

bool NumericDesc::initFromJson(const rapidjson::Value& el) {
    auto type_it = el.FindMember("type");
    if (type_it == el.MemberEnd()) {
        m_type = NumericDescType::RELATIVE;
    } else {
        auto type = std::string(type_it->value.GetString());
        auto it = str2ndt.find(type);
        if (it != std::end(str2ndt)) {
            m_type = it->second;
        }
        CCASSERT(m_type != NumericDescType::INVALID, "");
    }
    DX_DESC_GET_REF_MEMBER(el, NumericExpr, expr);
    CC_SAFE_RETAIN(m_expr);
    return true;
}

bool NumericDesc::initFromLua(lua_State* L, int idx) {
    lua_pushstring(L, "type");
    lua_gettable(L, (idx < 0 ? idx - 1 : idx));
    if (lua_isnil(L, lua_gettop(L))) {
        m_type = NumericDescType::RELATIVE;
    } else {
        auto type = std::string(lua_tostring(L, lua_gettop(L)));
        auto it = str2ndt.find(type);
        if (it != std::end(str2ndt)) {
            m_type = it->second;
        }
        CCASSERT(m_type != NumericDescType::INVALID, "");
    }
    lua_pop(L, 1);
    DX_LUA_DESC_GET_OPT_REF_MEMBER(L, idx, NumericExpr, expr);
    CC_SAFE_RETAIN(m_expr);
    return true;
}

float NumericDesc::eval() {
    return m_expr->eval();
}

NumericDesc::~NumericDesc() {
    CC_SAFE_RELEASE_NULL(m_expr);
}

NumericDescType NumericDesc::getType() const {
    return m_type;
}

ActionDesc* ActionDesc::createFromActions(
        const std::string& label,
        const cocos2d::Vector<GameAction*>& actions) {
    auto ad = new (std::nothrow) ActionDesc();
    if (ad) {
        ad->m_label = label;
        ad->m_actions = actions;
        ad->autorelease();
        return ad;
    }
    return nullptr;
}

bool ActionDesc::initFromJson(const rapidjson::Value& el) {
    DX_DESC_GET_MEMBER(el, string, label);
    DX_DESC_GET_VEC(el, GameAction, actions, m_actions);
    return true;
}

bool ActionDesc::initFromLua(lua_State* L, int idx) {
    DX_LUA_DESC_GET_MEMBER(L, idx, string, label);
    DX_LUA_DESC_GET_VEC(L, idx, GameAction, actions, m_actions);
    return true;
}

cocos2d::Action* ActionDesc::getAction() {
    cocos2d::Vector<cocos2d::FiniteTimeAction*> currSpawn;
    cocos2d::Vector<cocos2d::FiniteTimeAction*> seq;
    if (m_actions.size() == 1
            && m_actions.front()->getActionType() == GameActionType::FOREVER) {
        return m_actions.front()->getAction();
    }
    for (GameAction* ga : m_actions) {
        auto action = ga->getAction();
        if (!action) {
            cocos2d::log("internal action is null");
            return nullptr;
        }
        if (auto fta = dynamic_cast<cocos2d::FiniteTimeAction*>(action)) {
            currSpawn.pushBack(fta);
        } else {
            cocos2d::log("fatal error");
            return nullptr;
        }
        if (ga->getActionType() == GameActionType::WAIT) {
            auto spawn = cocos2d::Spawn::create(currSpawn);
            seq.pushBack(spawn);
            currSpawn.clear();
        }
    }
    if (!currSpawn.empty()) {
        auto spawn = cocos2d::Spawn::create(currSpawn);
        seq.pushBack(spawn);
    }
    auto actionSeq = cocos2d::Sequence::create(seq);
    if (!actionSeq) {
        cocos2d::log("'ActionDesc' generates null action sequence");
        return nullptr;
    }
    return actionSeq;
}

bool ActionRefDesc::initFromJson(const rapidjson::Value& el) {
    DX_DESC_GET_MEMBER(el, string, label);
    return true;
}

bool ActionRefDesc::initFromLua(lua_State* L, int idx) {
    DX_LUA_DESC_GET_MEMBER(L, idx, string, label);
    return true;
}

cocos2d::Action* ActionRefDesc::getAction() {
    auto action = cocos2d::CallFuncN::create([=](cocos2d::Node* tgt) {
        auto objMgr = ObjectManager::getInstance();
        auto actionDesc = objMgr->getActionDesc(m_label);
        CCASSERT(actionDesc != nullptr, cocos2d::StringUtils::format(
            "'ObjectManagers' return nil for %s action reference", m_label.c_str()).c_str());
        auto inner = actionDesc->getAction();
        tgt->runAction(inner);
    });
    return action;
}

RepeatDesc::~RepeatDesc() {
    CC_SAFE_RELEASE(m_times);
}

bool RepeatDesc::initFromJson(const rapidjson::Value& el) {
    DX_DESC_GET_REF_MEMBER(el, NumericExpr, times);
    DX_DESC_GET_VEC(el, GameAction, actions, m_actions);
    CC_SAFE_RETAIN(m_times);
    return true;
}

bool RepeatDesc::initFromLua(lua_State* L, int idx) {
    DX_LUA_DESC_GET_REF_MEMBER(L, idx, NumericExpr, times);
    DX_LUA_DESC_GET_VEC(L, idx, GameAction, actions, m_actions);
    CC_SAFE_RETAIN(m_times);
    return true;
}

cocos2d::Action* RepeatDesc::getAction() {
    auto actionDesc = ActionDesc::createFromActions("", m_actions);
    auto action = actionDesc->getAction();
    unsigned int ntimes = (unsigned int)(m_times->eval());
    if (auto fta = dynamic_cast<cocos2d::FiniteTimeAction*>(action)) {
        auto r = cocos2d::Repeat::create(fta, ntimes);
        return r;
    }
    return nullptr;
}

bool ForeverDesc::initFromJson(const rapidjson::Value& el) {
    DX_DESC_GET_VEC(el, GameAction, actions, m_actions);
    m_type = GameActionType::FOREVER;
    return true;
}

bool ForeverDesc::initFromLua(lua_State* L, int idx) {
    DX_LUA_DESC_GET_VEC(L, idx, GameAction, actions, m_actions);
    m_type = GameActionType::FOREVER;
    return true;
}

cocos2d::Action* ForeverDesc::getAction() {
    auto actionDesc = ActionDesc::createFromActions("", m_actions);
    auto action = actionDesc->getAction();
    if (auto fta = dynamic_cast<cocos2d::ActionInterval*>(action)) {
        auto r = cocos2d::RepeatForever::create(fta);
        return r;
    }
    return nullptr;
}

bool WhileDesc::initFromJson(const rapidjson::Value &el) {
    DX_DESC_GET_MEMBER(el, string, expr);
    DX_DESC_GET_VEC(el, GameAction, actions, m_actions);
    m_type = GameActionType::WHILE;
    return true;
}

cocos2d::Action* WhileDesc::getAction() {
    auto actionDesc = ActionDesc::createFromActions("", m_actions);
    auto action = actionDesc->getAction();
    return nullptr;
}

PlaceDescBase::~PlaceDescBase() {
    CC_SAFE_RELEASE(m_rotation);
    CC_SAFE_RELEASE(m_orientation);
    CC_SAFE_RELEASE(m_position);
    CC_SAFE_RELEASE(m_speed);
    CC_SAFE_RELEASE(m_direction);
}

bool PlaceDescBase::initFromJson(const rapidjson::Value& el) {
    DX_DESC_GET_OPT_MEMBER(el, int, tag);
    DX_DESC_GET_OPT_MEMBER(el, vector<string>, components);
    DX_DESC_GET_OPT_MEMBER(el, int, zOrder);
    DX_DESC_GET_OPT_MEMBER(el, bool, child);
    DX_DESC_GET_OPT_REF_MEMBER(el, RotationDesc, rotation);
    DX_DESC_GET_OPT_REF_MEMBER(el, OrientationDesc, orientation);
    DX_DESC_GET_OPT_REF_MEMBER(el, PositionDesc, position);
    DX_DESC_GET_OPT_REF_MEMBER(el, SpeedDesc, speed);
    DX_DESC_GET_OPT_REF_MEMBER(el, DirectionDesc, direction);
    CC_SAFE_RETAIN(m_rotation);
    CC_SAFE_RETAIN(m_orientation);
    CC_SAFE_RETAIN(m_position);
    CC_SAFE_RETAIN(m_speed);
    CC_SAFE_RETAIN(m_direction);
    return true;
}

bool PlaceDescBase::initFromLua(lua_State* L, int idx) {
    DX_LUA_DESC_GET_OPT_MEMBER(L, idx, int, tag);
    DX_LUA_DESC_GET_OPT_MEMBER(L, idx, vector<string>, components);
    DX_LUA_DESC_GET_OPT_MEMBER(L, idx, int, zOrder);
    DX_LUA_DESC_GET_OPT_MEMBER(L, idx, bool, child);
    DX_LUA_DESC_GET_OPT_REF_MEMBER(L, idx, RotationDesc, rotation);
    DX_LUA_DESC_GET_OPT_REF_MEMBER(L, idx, OrientationDesc, orientation);
    DX_LUA_DESC_GET_OPT_REF_MEMBER(L, idx, PositionDesc, position);
    DX_LUA_DESC_GET_OPT_REF_MEMBER(L, idx, SpeedDesc, speed);
    DX_LUA_DESC_GET_OPT_REF_MEMBER(L, idx, DirectionDesc, direction);
    CC_SAFE_RETAIN(m_rotation);
    CC_SAFE_RETAIN(m_orientation);
    CC_SAFE_RETAIN(m_position);
    CC_SAFE_RETAIN(m_speed);
    CC_SAFE_RETAIN(m_direction);
    return true;
}

PlaceObjectDesc::~PlaceObjectDesc() {
    CC_SAFE_RELEASE(m_objectDesc);
}

bool PlaceObjectDesc::initFromJson(const rapidjson::Value& el) {
    if (!PlaceDescBase::initFromJson(el)) {
        return false;
    }
    DX_DESC_GET_REF_MEMBER(el, GameObjectDesc, objectDesc);
    CC_SAFE_RETAIN(m_objectDesc);
    return true;
}

bool PlaceObjectDesc::initFromLua(lua_State* L, int idx) {
    if (!PlaceDescBase::initFromLua(L, idx)) {
        return false;
    }
    DX_LUA_DESC_GET_REF_MEMBER(L, idx, GameObjectDesc, objectDesc);
    CC_SAFE_RETAIN(m_objectDesc);
    return true;
}

cocos2d::Action* PlaceObjectDesc::getAction() {
    auto action = cocos2d::CallFuncN::create([=](cocos2d::Node* tgt) {
        auto go = GameObject::createFromDesc(m_objectDesc);
        CCASSERT(go != nullptr, "Can't create 'GameObject' from 'GameObjectDesc'");
        this->placeObject(go, tgt);
    });
    return action;
}

PlaceVehicleDesc::~PlaceVehicleDesc() {
    CC_SAFE_RELEASE(m_vehicleDesc);
}

bool PlaceVehicleDesc::initFromJson(const rapidjson::Value& el) {
    if (!PlaceDescBase::initFromJson(el)) {
        return false;
    }
    DX_DESC_GET_REF_MEMBER(el, VehicleDesc, vehicleDesc);
    CC_SAFE_RETAIN(m_vehicleDesc);
    return true;
}

bool PlaceVehicleDesc::initFromLua(lua_State* L, int idx) {
    if (!PlaceDescBase::initFromLua(L, idx)) {
        return false;
    }
    DX_LUA_DESC_GET_REF_MEMBER(L, idx, VehicleDesc, vehicleDesc);
    CC_SAFE_RETAIN(m_vehicleDesc);
    return true;
}

cocos2d::Action* PlaceVehicleDesc::getAction() {
    auto action = cocos2d::CallFuncN::create([=](cocos2d::Node* tgt) {
        auto go = Vehicle::createFromDesc(m_vehicleDesc);
        CCASSERT(go != nullptr, "Can't create 'Vehicle' from 'VehicleDesc'");
        this->placeObject(go, tgt);
    });
    return action;
}

FireBulletDesc::~FireBulletDesc() {
    CC_SAFE_RELEASE(m_bulletDesc);
}

bool FireBulletDesc::initFromJson(const rapidjson::Value& el) {
    if (!PlaceDescBase::initFromJson(el)) {
        return false;
    }
    DX_DESC_GET_REF_MEMBER(el, BulletDesc, bulletDesc);
    CC_SAFE_RETAIN(m_bulletDesc);
    return true;
}

bool FireBulletDesc::initFromLua(lua_State* L, int idx) {
    if (!PlaceDescBase::initFromLua(L, idx)) {
        return false;
    }
    DX_LUA_DESC_GET_REF_MEMBER(L, idx, BulletDesc, bulletDesc);
    CC_SAFE_RETAIN(m_bulletDesc);
    return true;
}

cocos2d::Action* FireBulletDesc::getAction() {
    auto action = cocos2d::CallFuncN::create([=](cocos2d::Node* tgt) {
        auto go = Bullet::createFromDesc(m_bulletDesc);
        CCASSERT(go != nullptr, "Can't create 'Bullet' from 'BulletDesc'");
        this->placeObject(go, tgt);
    });
    return action;
}

bool PlaceObjectRefDesc::initFromJson(const rapidjson::Value& el) {
    if (!PlaceDescBase::initFromJson(el)) {
        return false;
    }
    DX_DESC_GET_MEMBER(el, string, label);
    return true;
}

bool PlaceObjectRefDesc::initFromLua(lua_State* L, int idx) {
    if (!PlaceDescBase::initFromLua(L, idx)) {
        return false;
    }
    DX_LUA_DESC_GET_MEMBER(L, idx, string, label);
    return true;
}

float PlaceDescBase::evalSpeed(GameObject *p) {
    if (!m_speed) {
        return 0;
    }
    switch (m_speed->getType()) {
        case NumericDescType::ABSOLUTE :
            return m_speed->eval();
        case NumericDescType::RELATIVE :
            return m_speed->eval() + p->getSpeed();
        default:
            return 0;
    }
}

float PlaceDescBase::evalDirection(const cocos2d::Vec2& pos, GameObject *p) {
    if (!m_direction) {
        if (p) {
            return p->getDirection();
        } else {
            return 90;
        }
    }
    switch (m_direction->getType()) {
        case NumericDescType::ABSOLUTE :
            return m_direction->eval();
        case NumericDescType::RELATIVE :
            return m_direction->eval() + p->getDirection();
        case NumericDescType::AIM : {
            auto compMgr = ComponentManager::getInstance();
            auto player = compMgr->getPlayerObject();
            auto playerPos = player->getPosition();
            auto v = playerPos - pos;
            return CC_RADIANS_TO_DEGREES(
                std::atan2(v.y, v.x));
        }
        default:
            return 0;
    }
}

cocos2d::Action* PlaceObjectRefDesc::getAction() {
    auto action = cocos2d::CallFuncN::create([=](cocos2d::Node* tgt) {
        auto objMgr = ObjectManager::getInstance();
        auto objDesc = objMgr->getGameObjectDesc(m_label);
        CCASSERT(objDesc != nullptr, cocos2d::StringUtils::format(
            "No 'GameObject' defined for %s", m_label.c_str()).c_str());
        auto go = GameObject::createFromDesc(objDesc);
        CCASSERT(go != nullptr, "Can't create 'GameObject' from 'GameObjectDesc'");
        this->placeObject(go, tgt);
    });
    return action;
}

void PlaceDescBase::placeObject(GameObject* obj, cocos2d::Node* tgt) {
    auto compMgr = ComponentManager::getInstance();
    for (const auto& compName: m_components) {
        auto comp = compMgr->createComponent(compName);
        obj->addComponent(comp);
    }
    if (auto pobj = dynamic_cast<GameObject*>(tgt)) {
        auto ll = pobj->getLevelLayer();
        ll->createPhysBody(obj);
        auto rPos = m_position->eval();
        auto bbox = cocos2d::utils::getCascadeBoundingBox(pobj);
        rPos = cocos2d::Vec2(-bbox.size.width/2 + rPos.x,
                             bbox.size.height/2 -rPos.y);
        auto vPos = pobj->getPosition();
        if (!m_child) {
            rPos += vPos;
        }
        auto vel = cocos2d::Vec2(1, 0);
        auto direction = this->evalDirection(rPos, pobj);
        auto speed = this->evalSpeed(pobj);
        vel *= speed;
        vel.rotate(cocos2d::Vec2::ZERO, CC_DEGREES_TO_RADIANS(direction));
        obj->setVelocity(vel);
        obj->setPosition(rPos);
        if (m_orientation) {
            obj->setRotation(m_orientation->eval());
        }
        if (m_rotation) {
            obj->setAngularSpeed(m_rotation->eval());
        }
        if (m_child) {
            if (m_tag != -1) {
                obj->setTag(m_tag);
            }
            pobj->addChild(obj, m_zOrder);
        } else {
            ll->addObject(obj);
        }
    } else if (auto ll = dynamic_cast<LevelLayer*>(tgt)) {
        auto pos = m_position->eval();
        auto vel = cocos2d::Vec2(1, 0);
        auto direction = this->evalDirection(pos, nullptr);
        auto speed = this->evalSpeed(nullptr);
        ll->createPhysBody(obj);
        vel *= speed;
        vel.rotate(cocos2d::Vec2::ZERO, CC_DEGREES_TO_RADIANS(direction));
        obj->setVelocity(vel);
        obj->setPosition(pos);
        if (m_orientation) {
            obj->setRotation(m_orientation->eval());
        }
        ll->addObject(obj);
    }
}

bool PlaceVehicleRefDesc::initFromJson(const rapidjson::Value& el) {
    if (!PlaceObjectRefDesc::initFromJson(el)) {
        return false;
    }
    return true;
}

bool PlaceVehicleRefDesc::initFromLua(lua_State* L, int idx) {
    if (!PlaceObjectRefDesc::initFromLua(L, idx)) {
        return false;
    }
    return true;
}

cocos2d::Action* PlaceVehicleRefDesc::getAction() {
    auto action = cocos2d::CallFuncN::create([=](cocos2d::Node* tgt) {
        auto objMgr = ObjectManager::getInstance();
        auto vDesc = objMgr->getVehicleDesc(m_label);
        //CCASSERT(vDesc != nullptr, cocos2d::StringUtils::format(
        //    "No 'VehicleDesc' defined for '%s'", m_label.c_str()).c_str());
        auto v = Vehicle::createFromDesc(vDesc);
        //CCASSERT(v != nullptr, "Can't create 'Vehicle' from 'VehicleDesc'");
        this->placeObject(v, tgt);
    });
    return action;
}

bool FireBulletRefDesc::initFromJson(const rapidjson::Value& el) {
    if (!PlaceObjectRefDesc::initFromJson(el)) {
        return false;
    }
    return true;
}

bool FireBulletRefDesc::initFromLua(lua_State* L, int idx) {
    if (!PlaceObjectRefDesc::initFromLua(L, idx)) {
        return false;
    }
    return true;
}

cocos2d::Action* FireBulletRefDesc::getAction() {
    auto action = cocos2d::CallFuncN::create([=](cocos2d::Node* tgt) {
        auto objMgr = ObjectManager::getInstance();
        auto bDesc = objMgr->getBulletDesc(m_label);
        CCASSERT(bDesc != nullptr, cocos2d::StringUtils::format(
            "No 'BulletDesc' defined for '%s'", m_label.c_str()).c_str());
        auto b = Bullet::createFromDesc(bDesc);
        CCASSERT(b != nullptr, "Can't create 'Bullet' from 'BulletDesc'");
        this->placeObject(b, tgt);
    });
    return action;
}

bool ChangeSpeedDesc::initFromJson(const rapidjson::Value& el) {
    DX_DESC_GET_REF_MEMBER(el, SpeedDesc, speed);
    DX_DESC_GET_REF_MEMBER(el, NumericExpr, term);
    CC_SAFE_RETAIN(m_speed);
    CC_SAFE_RETAIN(m_term);
    return true;
}

bool ChangeSpeedDesc::initFromLua(lua_State* L, int idx) {
    DX_LUA_DESC_GET_REF_MEMBER(L, idx, SpeedDesc, speed);
    DX_LUA_DESC_GET_REF_MEMBER(L, idx, NumericExpr, term);
    CC_SAFE_RETAIN(m_speed);
    CC_SAFE_RETAIN(m_term);
    return true;
}

void ChangeSpeedDesc::Action::update(float dt) {
    auto tgt = static_cast<GameObject*>(this->getTarget());
    auto vel = m_initV.lerp(m_finV, dt);
    tgt->setVelocity(vel);
}

ChangeSpeedDesc::~ChangeSpeedDesc() {
    CC_SAFE_RELEASE_NULL(m_speed);
    CC_SAFE_RELEASE_NULL(m_term);
}

bool ChangeDirectionDesc::initFromJson(const rapidjson::Value& el) {
    DX_DESC_GET_REF_MEMBER(el, DirectionDesc, direction);
    DX_DESC_GET_REF_MEMBER(el, NumericExpr, term);
    CC_SAFE_RETAIN(m_direction);
    CC_SAFE_RETAIN(m_term);
    return true;
}

bool ChangeDirectionDesc::initFromLua(lua_State* L, int idx) {
    DX_LUA_DESC_GET_REF_MEMBER(L, idx, DirectionDesc, direction);
    DX_LUA_DESC_GET_REF_MEMBER(L, idx, NumericExpr, term);
    CC_SAFE_RETAIN(m_direction);
    CC_SAFE_RETAIN(m_term);
    return true;
}

void ChangeDirectionDesc::Action::update(float dt) {
    auto tgt = static_cast<GameObject*>(this->getTarget());
    auto vel = tgt->getVelocity();
    auto newVel = vel.rotateByAngle(cocos2d::Vec2::ZERO, m_delta*dt);
    tgt->setVelocity(newVel);
}

ChangeDirectionDesc::~ChangeDirectionDesc() {
    CC_SAFE_RELEASE_NULL(m_direction);
    CC_SAFE_RELEASE_NULL(m_term);
}

bool AccelDesc::initFromJson(const rapidjson::Value& el) {
    DX_DESC_GET_OPT_REF_MEMBER(el, HorizontalDesc, horizontal);
    DX_DESC_GET_OPT_REF_MEMBER(el, VerticalDesc, vertical);
    DX_DESC_GET_REF_MEMBER(el, NumericExpr, term);
    CC_SAFE_RETAIN(m_horizontal);
    CC_SAFE_RETAIN(m_vertical);
    CC_SAFE_RETAIN(m_term);
    m_type = GameActionType::ACCEL;
    return true;
}

bool AccelDesc::initFromLua(lua_State* L, int idx) {
    DX_LUA_DESC_GET_OPT_REF_MEMBER(L, idx, HorizontalDesc, horizontal);
    DX_LUA_DESC_GET_OPT_REF_MEMBER(L, idx, VerticalDesc, vertical);
    DX_LUA_DESC_GET_REF_MEMBER(L, idx, NumericExpr, term);
    CC_SAFE_RETAIN(m_horizontal);
    CC_SAFE_RETAIN(m_vertical);
    CC_SAFE_RETAIN(m_term);
    m_type = GameActionType::ACCEL;
    return true;
}

AccelDesc::~AccelDesc() {
    CC_SAFE_RELEASE_NULL(m_horizontal);
    CC_SAFE_RELEASE_NULL(m_vertical);
    CC_SAFE_RELEASE_NULL(m_term);
}


AccelDesc::Action* AccelDesc::Action::create(const cocos2d::Vec2& accel, float dt) {
    auto action = new (std::nothrow) AccelDesc::Action();
    if (action && action->init(accel, dt)) {
        action->autorelease();
        return action;
    }
    CC_SAFE_DELETE(action);
    return nullptr;
}

bool AccelDesc::Action::init(const cocos2d::Vec2& accel, float dt) {
    m_accel = accel;
    return this->initWithDuration(dt);
}

void AccelDesc::Action::startWithTarget(cocos2d::Node* tgt) {
    ActionInterval::startWithTarget(tgt);
    auto obj = static_cast<GameObject*>(tgt);
    obj->applyAccel(m_accel);
}

void AccelDesc::Action::stop() {
    auto obj = static_cast<GameObject*>(this->getTarget());
    obj->applyAccel(-m_accel);
    ActionInterval::stop();
}

cocos2d::Action* AccelDesc::getAction() {
    auto accelX = m_horizontal ? m_horizontal->eval() : 0;
    auto accelY = m_vertical ? m_vertical->eval() : 0;
    auto dt = m_term->eval();
    cocos2d::Vec2 accel(accelX, accelY);
    return Action::create(accel, dt);
}

bool VanishDesc::initFromJson(const rapidjson::Value& el) {
    m_type = GameActionType::VANISH;
    return true;
}

bool VanishDesc::initFromLua(lua_State* L, int idx) {
    m_type = GameActionType::VANISH;
    return true;
}

cocos2d::Action* VanishDesc::getAction() {
    auto r = cocos2d::CallFuncN::create([](cocos2d::Node* tgt) {
        auto obj = static_cast<GameObject*>(tgt);
        obj->setDead(true);
    });
    return r;
}

bool WaitDesc::initFromJson(const rapidjson::Value& el) {
    DX_DESC_GET_REF_MEMBER(el, NumericExpr, term);
    CC_SAFE_RETAIN(m_term);
    m_type = GameActionType::WAIT;
    return true;
}

bool WaitDesc::initFromLua(lua_State* L, int idx) {
    DX_LUA_DESC_GET_REF_MEMBER(L, idx, NumericExpr, term);
    CC_SAFE_RETAIN(m_term);
    m_type = GameActionType::WAIT;
    return true;
}

cocos2d::Action* WaitDesc::getAction() {
    float dt = m_term->eval();
    return cocos2d::DelayTime::create(dt);
}

WaitDesc::~WaitDesc() {
    CC_SAFE_RELEASE_NULL(m_term);
}

#define GAME_ACTION_CREATORS() \
    GAME_ACTION_CREATOR(forever, ForeverDesc) \
    GAME_ACTION_CREATOR(repeat, RepeatDesc) \
    GAME_ACTION_CREATOR(actionRef, ActionRefDesc) \
    GAME_ACTION_CREATOR(placeObject, PlaceObjectDesc) \
    GAME_ACTION_CREATOR(placeVehicle, PlaceVehicleDesc) \
    GAME_ACTION_CREATOR(fireBullet, FireBulletDesc) \
    GAME_ACTION_CREATOR(placeObjectRef, PlaceObjectRefDesc) \
    GAME_ACTION_CREATOR(placeVehicleRef, PlaceVehicleRefDesc) \
    GAME_ACTION_CREATOR(fireBulletRef, FireBulletRefDesc) \
    GAME_ACTION_CREATOR(changeSpeed, ChangeSpeedDesc) \
    GAME_ACTION_CREATOR(changeDirection, ChangeDirectionDesc) \
    GAME_ACTION_CREATOR(accel, AccelDesc) \
    GAME_ACTION_CREATOR(vanish, VanishDesc) \
    GAME_ACTION_CREATOR(wait, WaitDesc) \
    GAME_ACTION_CREATOR(action, ActionDesc)

#define GAME_ACTION_CREATOR(name, type) { #name, &GameAction::create<type> },
GameAction::GameActionCreatorMapT GameAction::GameActionCreatorMap = {
    GAME_ACTION_CREATORS()
};
GameAction::GameActionCreatorLuaMapT GameAction::GameActionCreatorLuaMap = {
    GAME_ACTION_CREATORS()
};
#undef GAME_ACTION_CREATOR

GameAction* GameAction::fromJson(const rapidjson::Value& el) {
    auto member_end = el.MemberEnd();
    rapidjson::Value::ConstMemberIterator it;
    it = el.FindMember("type");
    if (it == member_end) {
        cocos2d::log("'GameAction' should define 'type' member");
        return nullptr;
    }
    if (!it->value.IsString()) {
        cocos2d::log("'GameAction' type member should have string type");
        return nullptr;
    }
    auto typestr = it->value.GetString();
    auto cons_it = GameActionCreatorMap.find(typestr);
    if (cons_it == std::end(GameActionCreatorMap)) {
        cocos2d::log("Invalid 'GameAction' type: '%s'", typestr);
        return nullptr;
    }
    auto ga = cons_it->second(el);
    if (!ga) {
        cocos2d::log("Can't create action of type '%s'", typestr);
        return nullptr;
    }
    return ga;
}

GameAction* GameAction::fromLua(lua_State* L, int idx) {
    lua_pushstring(L, "type");
    lua_gettable(L, (idx < 0 ? idx - 1 : idx));
    if (lua_isnil(L, -1)) {
        cocos2d::log("'GameAction' should define 'type' member");
        lua_pop(L, 1);
        return nullptr;
    }
    if (!lua_isstring(L, -1)) {
        cocos2d::log("'GameAction' type member should have string type");
        lua_pop(L, 1);
        return nullptr;
    }
    auto typestr = lua_tostring(L, -1);
    auto cons_it = GameActionCreatorLuaMap.find(typestr);
    if (cons_it == std::end(GameActionCreatorLuaMap)) {
        cocos2d::log("Invalid 'GameAction' type: '%s'", typestr);
        lua_pop(L, 1);
        return nullptr;
    }
    lua_pop(L, 1);
    auto ga = cons_it->second(L, idx);
    if (!ga) {
        cocos2d::log("Can't create action of type '%s'", typestr);
        return nullptr;
    }
    return ga;
}

GameAction* GameAction::fromLua(const std::string& filename) {
    auto engine = cocos2d::LuaEngine::getInstance();
    lua_State *l = engine->getLuaStack()->getLuaState();
    auto fileUtils = cocos2d::FileUtils::getInstance();
    std::string fullPathOfScript = fileUtils->fullPathForFilename(filename);
    cocos2d::Data data = fileUtils->getDataFromFile(fullPathOfScript);
    int error = LUA_ERRFILE;
    if(data.getSize() > 0) {
        error = engine->getLuaStack()->luaLoadBuffer(l, (const char*)data.getBytes(), (int)data.getSize(), fullPathOfScript.c_str());
    }
    if (error) {
        cocos2d::log("GameAction::fromLua: %s", lua_tostring(l, -1));
        lua_pop(l, 1);
        return nullptr;
    }
    // execute script
    error = lua_pcall(l, 0, 1, 0);
    if (error) {
        cocos2d::log("call GameAction::fromLua: %s", lua_tostring(l, -1));
        lua_pop(l, 1);
        return nullptr;
    }
    auto action = static_cast<GameAction*>(tolua_tousertype(l, -1, 0));
    lua_pop(l, 1);
    return action;
}
