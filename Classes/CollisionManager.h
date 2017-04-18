#ifndef COLLISIONMANAGER_H
#define COLLISIONMANAGER_H

#include "cocos2d.h"
#include "Objects/GameObject.h"

class GLColorMaskNode : public cocos2d::Node {
public:
    static GLColorMaskNode* create(
        GLboolean r, GLboolean g, GLboolean b, GLboolean a);
    bool init(GLboolean r, GLboolean g, GLboolean b, GLboolean a);
    void draw(cocos2d::Renderer *renderer,
              const cocos2d::Mat4 &transform,
              uint32_t flags);
    void setGlColorMask();
protected:
    cocos2d::CustomCommand m_customCmd;
    GLboolean m_r = 1;
    GLboolean m_g = 1;
    GLboolean m_b = 1;
    GLboolean m_a = 1;
};

class PixelCollisionDetectorNode : public cocos2d::Node {
public:
    CREATE_FUNC(PixelCollisionDetectorNode)
    void setRect(const cocos2d::Rect& rect);
    void setSprites(GameObject* sp0, GameObject* sp1);
    void draw(cocos2d::Renderer *renderer,
              const cocos2d::Mat4 &transform,
              uint32_t flags);
    void checkCollision(cocos2d::Rect rect,
                        GameObject *sp0,
                        GameObject *sp1);
protected:
    cocos2d::CustomCommand m_customCmd;
    cocos2d::Rect m_rect;
    GameObject* m_sp0 = nullptr;
    GameObject* m_sp1 = nullptr;
};

class CollisionManager
{
public:
    static CollisionManager* getInstance();
    ~CollisionManager();
    bool init();
    void testCollision(GameObject* sp0,
                       GameObject* sp1);
    void sync();
protected:
    cocos2d::Renderer* m_renderer = nullptr;
    cocos2d::RenderTexture* m_rt = nullptr;
    GLColorMaskNode* m_glColorMaskR = nullptr;
    GLColorMaskNode* m_glColorMaskG = nullptr;
    GLColorMaskNode* m_glColorMaskAll = nullptr;
    PixelCollisionDetectorNode* m_pcd = nullptr;
};

#endif // COLLISIONMANAGER_H
