#ifndef __DX_GameObject_H__
#define __DX_GameObject_H__

#include "cocos2d.h"
#include "Box2D/Box2D.h"

enum class GameObjectType : uint8_t {
    GENERIC,
    VEHICLE,
    BULLET,
    PLAYER
};

enum {
    GAME_OBJECT_TAG_SPRITE = 1,
    GAME_OBJECT_TAG_TURRET,
    GAME_OBJECT_TAG_ROTOR,
    GAME_OBJECT_TAG_INVALID = -1
};

enum {
    COLLISION_CLASS_PLAYER,
    COLLISION_CLASS_PLAYER_BULLET,
    COLLISION_CLASS_ENEMY,
    COLLISION_CLASS_ENEMY_BULLET,
    COLLISION_CLASS_POWERUP,
    COLLISION_CLASS_INVALID = -1
};

enum {
    GAME_OBJECT_PROP_POSITION,
    GAME_OBJECT_PROP_VELOCITY,
    GAME_OBJECT_PROP_ACCEL,
    GAME_OBJECT_PROP_ORIENTATION,
    GAME_OBJECT_PROP_ANG_VELOCITY,
    GAME_OBJECT_PROP_MAX
};

struct GameObjectState {
    cocos2d::Vec2 position;
    float rotation;
};

class LevelLayer;
class GameObjectDesc;
class GameObject : public cocos2d::Node {
public:
    static GameObject* createFromDesc(GameObjectDesc* desc);
    virtual ~GameObject();
    virtual void update(float dt);
    virtual GameObjectState readStateBytes(uint8_t* &p);
    virtual size_t writeStateBytes(std::vector<uint8_t> &buf);
    virtual void setPosition(const cocos2d::Vec2& pos) override;
    virtual void setPosition(float x, float y) override;
    virtual void setRotation(float angle) override;
    void setOrientation(const cocos2d::Vec2& orientation);
    void setVelocity(const cocos2d::Vec2& vel);
    void applyAccel(const cocos2d::Vec2& accel);
    void setAngularSpeed(float rotation);
    void setSpeed(float speed);
    void setDirection(float direction);
    void changeDirection(float delta);
    cocos2d::Vec2 getOrientation() const;
    cocos2d::Vec2 getVelocity() const;
    float getAngVelocity() const;
    float getSpeed() const;
    virtual float getDirection() const;
    bool isDead() const;
    void setDead();
    virtual void setDead(bool dead);
    int getCollisionClass() const;
    void setCollisionClass(int klass);
    uint32_t getCollisionMask() const;
    void setCollisionMask(uint32_t mask);
    void enableCollisionsWith(int klass);
    bool canCollideWith(GameObject* obj) const;
    bool canCollideWith(int klass) const;
    void addSprite(cocos2d::Sprite* sp);
    virtual void onCollision(GameObject* obj);
    void setPrevSpeed(GameObjectDesc* d, float speed);
    void setPrevDirection(GameObjectDesc* d, float direction);
    float getMaxSpeed() const;
    float getMaxSpeedSquared() const;
    void setMaxSpeed(float s);
    bool hasMaxSpeed() const;
    float getMaxRotation() const;
    bool hasMaxRotation() const;
    float getMaxAngularAccel() const;
    bool hasMaxAngularAccel() const;
    void setOnGrid(bool v);
    bool isOnGrid() const;
    uint64_t getNetId() const;
    void setNetId(uint64_t netId);
    void clientSideInterpolate(const GameObjectState& s);
    b2Body* getPhysBody();
    void setPhysBody(b2Body* body);
    void setPhysWorld(b2World* world);
    LevelLayer* getLevelLayer();
    void setLevelLayer(LevelLayer* ll);
    const std::string& getSpriteFilename() const;
protected:
    bool initFromDesc(GameObjectDesc* desc);
    bool m_autoUpdateDirection = false;

    bool m_onGrid = true;
    bool m_paused = false;
    bool m_dead = false;
    int m_collisionClass = -1;
    uint32_t m_collisionMask = 0;
    GameObjectDesc* m_objDesc = nullptr;
    std::unordered_map<GameObjectDesc*, float> m_prevSpeed;
    std::unordered_map<GameObjectDesc*, float> m_prevDirection;
    float m_maxSpeed = std::numeric_limits<float>::max();
    float m_maxSpeedSquared = std::numeric_limits<float>::max();
    //
    uint64_t m_netId = 0;
    float m_timeOutOfSync;
    //
    b2Body* m_body = nullptr;
    b2World* m_world = nullptr;
    //
    cocos2d::DrawNode* m_drawNode = nullptr;
    cocos2d::Sprite* m_sprite = nullptr;
    LevelLayer* m_levelLayer = nullptr;
};

#endif /* __DX_GameObject_H__ */
