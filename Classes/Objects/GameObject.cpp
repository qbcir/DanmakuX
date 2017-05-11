#include "GameObject.h"
#include "../GameManager.h"
#include "../Data/GameObjectDesc.h"
#include "CCComponentLua.h"
#include "../Data/ByteParser.h"
#include "../Network/NetworkManager.h"
#include "../Input/InputPacketComponent.h"

GameObject::~GameObject() {
    CC_SAFE_RELEASE(m_objDesc);
    m_world->DestroyBody(m_body);
}

const cocos2d::Vec2 ZERO_DEG_VEC = cocos2d::Vec2::forAngle(0);

void GameObject::setOrientation(const cocos2d::Vec2& orientation) {
    this->setRotation(CC_RADIANS_TO_DEGREES(std::atan2(-orientation.y, orientation.x)) - 90);
}

void GameObject::setVelocity(const cocos2d::Vec2& vel) {
    m_body->SetLinearVelocity(b2Vec2(vel.x, vel.y));
}

void GameObject::applyAccel(const cocos2d::Vec2& accel) {
    m_body->ApplyForceToCenter(b2Vec2(accel.x, accel.y), true);
}

void GameObject::setSpeed(float speed) {
    b2Vec2 vel = m_body->GetLinearVelocity();
    vel.Normalize();
    vel.x *= speed;
    vel.y *= speed;
    m_body->SetLinearVelocity(vel);
}

void GameObject::setAngularSpeed(float rotation) {
    m_body->SetAngularVelocity(rotation);
}

void GameObject::setDirection(float direction) {
    /*
    auto speed = m_velocity.length();
    m_velocity = cocos2d::Vec2::forAngle(CC_DEGREES_TO_RADIANS(direction));
    m_velocity *= speed;
    */
}

void GameObject::changeDirection(float delta) {
    /*
    m_velocity.rotate(cocos2d::Vec2::ZERO, CC_DEGREES_TO_RADIANS(delta));
    */
}

cocos2d::Vec2 GameObject::getOrientation() const {
    return cocos2d::Vec2::forAngle(this->getRotation());
}

cocos2d::Vec2 GameObject::getVelocity() const {
    auto vel = m_body->GetLinearVelocity();
    return cocos2d::Vec2(vel.x, vel.y);
}

float GameObject::getSpeed() const {
    auto v = m_body->GetLinearVelocity();
    return v.Length();
}

float GameObject::getAngVelocity() const {
    return m_body->GetAngularVelocity();
}

float GameObject::getDirection() const {
    auto v = m_body->GetLinearVelocity();
    auto d = std::atan2(v.y, v.x);
    return CC_RADIANS_TO_DEGREES(d);
}

bool GameObject::isDead() const {
    return m_dead;
}

void GameObject::setDead() {
    m_dead = true;
}

void GameObject::setDead(bool dead) {
    m_dead = dead;
}

int GameObject::getCollisionClass() const {
    return m_collisionClass;
}

void GameObject::setCollisionClass(int klass) {
    m_collisionClass = klass;
}

uint32_t GameObject::getCollisionMask() const {
    return m_collisionMask;
}

void GameObject::setCollisionMask(uint32_t mask) {
    m_collisionMask = mask;
}

void GameObject::enableCollisionsWith(int klass) {
    m_collisionMask |= (uint32_t(1) << klass);
}

bool GameObject::canCollideWith(int klass) const {
    if (klass == -1) {
        return false;
    }
    return (m_collisionMask >> klass) & 1;
}

bool GameObject::canCollideWith(GameObject* obj) const {
    int cc = obj->getCollisionClass();
    return canCollideWith(cc);
}

void GameObject::onCollision(GameObject *obj) {
}

void GameObject::addSprite(cocos2d::Sprite* sp) {
    sp->setTag(GAME_OBJECT_TAG_SPRITE);
    this->addChild(sp);
}

void GameObject::update(float dt) {
    for (auto ch : this->getChildren()) {
        ch->update(dt);
    }
    auto pos = m_body->GetPosition();
    Node::setPosition(pos.x, pos.y);
    auto rot = -CC_RADIANS_TO_DEGREES(m_body->GetAngle());
    Node::setRotation(rot);
    /*
    if (m_rotation) {
        rotation = m_rotation * dt;
        orientation += rotation;
        this->setRotation(orientation);
        m_velocity.rotate(cocos2d::Vec2::ZERO, -CC_DEGREES_TO_RADIANS(rotation));
    } */
    /*
    if (hasMaxSpeed() &&
            m_velocity.lengthSquared() > m_maxSpeedSquared) {
        m_velocity.normalize();
        m_velocity *= m_maxSpeed;
    }
    */
    Node::update(dt);
}

void GameObject::setPosition(const cocos2d::Vec2 &pos) {
    setPosition(pos.x, pos.y);
}

void GameObject::setPosition(float x, float y) {
    m_body->SetTransform(b2Vec2(x, y), m_body->GetAngle());
    Node::setPosition(x, y);
}

void GameObject::setRotation(float angle) {
    m_body->SetTransform(m_body->GetPosition(), CC_DEGREES_TO_RADIANS(angle));
    Node::setRotation(angle);
}

GameObject* GameObject::createFromDesc(GameObjectDesc* desc) {
    auto obj = new (std::nothrow) GameObject();
    if (obj && obj->initFromDesc(desc)) {
        obj->autorelease();
        return obj;
    }
    CC_SAFE_DELETE(obj);
    return nullptr;
}

bool GameObject::initFromDesc(GameObjectDesc* desc) {
    m_objDesc = desc;
    CC_SAFE_RETAIN(m_objDesc);
    this->setMaxSpeed(desc->getMaxSpeed());
    m_autoUpdateDirection = desc->getAutoUpdateDirection();
    auto& spriteDesc = desc->getSpriteDesc();
    m_sprite = cocos2d::Sprite::create(spriteDesc.filename);
    if (!m_sprite) {
        cocos2d::log("Can't create sprite %s(%s)",
            spriteDesc.name.c_str(), spriteDesc.filename.c_str());
        return false;
    }
    m_sprite->setPosition(spriteDesc.position);
    m_sprite->setAnchorPoint(spriteDesc.anchorPoint);
    m_sprite->setName(spriteDesc.name);
    m_sprite->setTag(GAME_OBJECT_TAG_SPRITE);
    this->addChild(m_sprite);
    auto& componentDescs = desc->getComponentDescs();
    for (auto& componentDesc : componentDescs) {
        auto comp = cocos2d::ComponentLua::create(componentDesc.filename);
        comp->setName(componentDesc.name);
        this->addComponent(comp);
    }
    m_drawNode = cocos2d::DrawNode::create();
    this->addChild(m_drawNode, 2);
    auto mainActionDesc = desc->getAction();
    if (mainActionDesc) {
        auto mainAction = mainActionDesc->getAction();
        this->runAction(mainAction);
    }
    return true;
}

void GameObject::setPrevSpeed(GameObjectDesc* d, float speed) {
    m_prevSpeed[d] = speed;
}

void GameObject::setPrevDirection(GameObjectDesc* d, float direction) {
    m_prevDirection[d] = direction;
}

float GameObject::getMaxSpeed() const {
    return m_maxSpeed;
}

float GameObject::getMaxSpeedSquared() const {
    return m_maxSpeedSquared;
}

void GameObject::setMaxSpeed(float s) {
    m_maxSpeed = s;
    m_maxSpeedSquared = s * s;
}

bool GameObject::hasMaxSpeed() const {
    return m_maxSpeed != std::numeric_limits<float>::max();
}

float GameObject::getMaxRotation() const {
    return m_objDesc->getMaxRotation();
}

bool GameObject::hasMaxRotation() const {
    return m_objDesc->hasMaxRotation();
}

float GameObject::getMaxAngularAccel() const {
    return m_objDesc->getMaxAngularAccel();
}

bool GameObject::hasMaxAngularAccel() const {
    return m_objDesc->getMaxAngularAccel() != std::numeric_limits<float>::max();
}

void GameObject::setOnGrid(bool v) {
    m_onGrid = v;
}

bool GameObject::isOnGrid() const {
    return m_onGrid;
}

uint64_t GameObject::getNetId() const {
    return m_netId;
}

void GameObject::setNetId(uint64_t netId) {
    m_netId = netId;
}

GameObjectState GameObject::readStateBytes(uint8_t* &p) {
    GameObjectState old;
    old.position = getPosition();
    old.rotation = getRotation();
    auto pos = dx_read_bytes<b2Vec2>(p);
    auto vel = dx_read_bytes<b2Vec2>(p);
    auto angle = dx_read_bytes<float>(p);
    auto av = dx_read_bytes<float>(p);
    m_body->SetTransform(pos, angle);
    m_body->SetLinearVelocity(vel);
    m_body->SetAngularVelocity(av);
    Node::setRotation(angle);
    Node::setPosition(cocos2d::Vec2(pos.x, pos.y));
    return old;
}

size_t GameObject::writeStateBytes(std::vector<uint8_t>& buf) {
    dx_write_bytes<cocos2d::Vec2>(buf, getPosition());
    dx_write_bytes<cocos2d::Vec2>(buf, getVelocity());
    dx_write_bytes<float>(buf, getRotation());
    dx_write_bytes<float>(buf, getAngVelocity());
}

void GameObject::clientSideInterpolate(const GameObjectState &s) {
    auto nm = NetworkManager::getInstance();
    auto gm = GameManager::getInstance();
    float rtt = nm->getRoundTripTime();
    float time = gm->getFrameStartTime();
    auto position = getPosition();
    cocos2d::log("old.x=%f old.y=%f new.x=%f new.y=%f", s.position.x, s.position.y, position.x, position.y);
    //cocos2d::log("oldrot=%f newrot=%f", s.rotation, getRotation());
    //CCASSERT(position.equals(s.position),"POSITION REPLAY INCORRECT");
    auto delta = position - s.position;
    cocos2d::log("REPLAY dx=%f dy=%f", delta.x, delta.y);
    CCASSERT(delta.length() < 10.f, "POSITION REPLAY INCORRECT");
    //auto ipos = position.lerp(s.position, 0.1);
    //setPosition(ipos);
}

b2Body* GameObject::getPhysBody() {
    return m_body;
}

void GameObject::setPhysBody(b2Body* body) {
    m_body = body;
}

void GameObject::setPhysWorld(b2World* world) {
    m_world = world;
}

LevelLayer* GameObject::getLevelLayer() {
    return m_levelLayer;
}

void GameObject::setLevelLayer(LevelLayer* ll) {
    m_levelLayer = ll;
}

const std::string& GameObject::getSpriteFilename() const {
    return m_objDesc->getSpriteDesc().filename;
}
