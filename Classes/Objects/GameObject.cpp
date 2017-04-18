#include "GameObject.h"
#include "GameManager.h"
#include "../Data/GameObjectDesc.h"
#include "CCComponentLua.h"
#include "../Data/ByteParser.h"

GameObject::~GameObject() {
    CC_SAFE_RELEASE(m_objDesc);
}

const cocos2d::Vec2 ZERO_DEG_VEC = cocos2d::Vec2::forAngle(0);

void GameObject::setOrientation(const cocos2d::Vec2& orientation) {
    this->setRotation(CC_RADIANS_TO_DEGREES(std::atan2(-orientation.y, orientation.x)) - 90);
}

void GameObject::setVelocity(const cocos2d::Vec2& vel) {
    m_velocity = vel;
}

void GameObject::setAccel(const cocos2d::Vec2& accel) {
    m_accel = accel;
}

void GameObject::applyAccel(const cocos2d::Vec2& accel) {
    m_accel += accel;
}

void GameObject::setSpeed(float speed) {
    m_velocity.normalize();
    m_velocity *= speed;
}

void GameObject::setAngularSpeed(float rotation) {
    m_rotation = rotation;
}

void GameObject::setDirection(float direction) {
    auto speed = m_velocity.length();
    m_velocity = cocos2d::Vec2::forAngle(CC_DEGREES_TO_RADIANS(direction));
    m_velocity *= speed;
}

void GameObject::changeDirection(float delta) {
    m_velocity.rotate(cocos2d::Vec2::ZERO, CC_DEGREES_TO_RADIANS(delta));
}

cocos2d::Vec2 GameObject::getOrientation() const {
    return cocos2d::Vec2::forAngle(this->getRotation());
}

const cocos2d::Vec2& GameObject::getVelocity() const {
    return m_velocity;
}

const cocos2d::Vec2& GameObject::getAccel() const {
    return m_accel;
}

float GameObject::getSpeed() const {
    return m_velocity.length();
}

float GameObject::getAngVelocity() const {
    return m_rotation;
}

float GameObject::getDirection() const {
    return CC_RADIANS_TO_DEGREES(std::atan2(m_velocity.y, m_velocity.x));
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
    auto pos = this->getPosition();
    pos += m_velocity * dt;
    m_velocity += m_accel * dt;
    float orientation = this->getRotation();
    float rotation = 0;
    if (m_rotation) {
        rotation = m_rotation * dt;
        orientation += rotation;
        this->setRotation(orientation);
        m_velocity.rotate(cocos2d::Vec2::ZERO, -CC_DEGREES_TO_RADIANS(rotation));
    }
    if (hasMaxSpeed() &&
            m_velocity.lengthSquared() > m_maxSpeedSquared) {
        m_velocity.normalize();
        m_velocity *= m_maxSpeed;
    }
    this->setPosition(pos);
    Node::update(dt);
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
    auto sp = cocos2d::Sprite::create(spriteDesc.filename);
    if (!sp) {
        cocos2d::log("Can't create sprite %s(%s)",
            spriteDesc.name.c_str(), spriteDesc.filename.c_str());
        return false;
    }
    sp->setPosition(spriteDesc.position);
    sp->setAnchorPoint(spriteDesc.anchorPoint);
    sp->setName(spriteDesc.name);
    sp->setTag(GAME_OBJECT_TAG_SPRITE);
    this->addChild(sp);
    auto& componentDescs = desc->getComponentDescs();
    for (auto& componentDesc : componentDescs) {
        auto comp = cocos2d::ComponentLua::create(componentDesc.filename);
        comp->setName(componentDesc.name);
        this->addComponent(comp);
    }
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

void GameObject::readStateBytes(uint8_t* &p) {
    cocos2d::log("readStateBytes");
    setPosition(dx_read_bytes<cocos2d::Vec2>(p));
    m_velocity = dx_read_bytes<cocos2d::Vec2>(p);
    m_accel = dx_read_bytes<cocos2d::Vec2>(p);
    setRotation(dx_read_bytes<float>(p));
    m_rotation = dx_read_bytes<float>(p);
}

size_t GameObject::writeStateBytes(std::vector<uint8_t>& buf) {
    dx_write_bytes<cocos2d::Vec2>(buf, getPosition());
    dx_write_bytes<cocos2d::Vec2>(buf, getVelocity());
    dx_write_bytes<cocos2d::Vec2>(buf, getAccel());
    dx_write_bytes<float>(buf, getRotation());
    dx_write_bytes<float>(buf, getAngVelocity());
}

