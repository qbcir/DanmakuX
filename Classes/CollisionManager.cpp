#include "GameManager.h"
#include "CollisionManager.h"

GLColorMaskNode* GLColorMaskNode::create(
        GLboolean r,
        GLboolean g,
        GLboolean b,
        GLboolean a) {
    auto node = new (std::nothrow) GLColorMaskNode();
    if (node && node->init(r, g, b, a)) {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

bool GLColorMaskNode::init(GLboolean r, GLboolean g, GLboolean b, GLboolean a) {
    if (!Node::init()) {
        return false;
    }
    m_r = r;
    m_g = g;
    m_b = b;
    m_a = a;
    return true;
}

void GLColorMaskNode::draw(cocos2d::Renderer *renderer,
          const cocos2d::Mat4 &transform,
          uint32_t flags) {
    m_customCmd.init(_globalZOrder);
    m_customCmd.func = [=](){
        glColorMask(m_r, m_g, m_b, m_a);
    };
    renderer->addCommand(&m_customCmd);
}

void PixelCollisionDetectorNode::setRect(const cocos2d::Rect& rect) {
    m_rect = rect;
}

void PixelCollisionDetectorNode::setSprites(GameObject *sp0,
        GameObject *sp1) {
    m_sp0 = sp0;
    m_sp1 = sp1;
}

void PixelCollisionDetectorNode::draw(
        cocos2d::Renderer *renderer,
        const cocos2d::Mat4 &transform,
        uint32_t flags) {
    m_customCmd.init(_globalZOrder);
    m_customCmd.func = [=](){
        this->checkCollision(m_rect, m_sp0, m_sp1);
    };
    renderer->addCommand(&m_customCmd);
}

void PixelCollisionDetectorNode::checkCollision(
        cocos2d::Rect rect,
        GameObject* sp0,
        GameObject* sp1
        ) {
    GLint x = rect.origin.x;
    GLint y = rect.origin.y;
    GLsizei w = rect.size.width;
    GLsizei h = rect.size.height;
    GLsizei numPixels = w * h;
    if (numPixels <= 0) {
        return;
    }
    std::vector<cocos2d::Color4B> buffer(numPixels);
    glReadPixels(x, y, w, h, GL_RGBA, GL_UNSIGNED_BYTE, buffer.data());
    bool collisionDetected = false;
    for(size_t i = 0; i < numPixels; i++) {
        auto& color = buffer[i];
        if (color.r > 0 && color.g > 0) {
            collisionDetected = true;
            break;
        }
    }
    if (collisionDetected) {
        if (sp0->canCollideWith(sp1)) {
            sp0->onCollision(sp1);
        }
        if (sp1->canCollideWith(sp0)) {
            sp1->onCollision(sp0);
        }
    }
}

CollisionManager* CollisionManager::getInstance() {
    return GameManager::getInstance()->getCollisionManager();
}

bool CollisionManager::init() {
    auto director = cocos2d::Director::getInstance();
    m_renderer = director->getRenderer();
    auto winSize = director->getWinSize();
    m_rt = cocos2d::RenderTexture::create(winSize.width, winSize.height,
        cocos2d::Texture2D::PixelFormat::RGBA8888);
    m_pcd = PixelCollisionDetectorNode::create();
    m_glColorMaskR = GLColorMaskNode::create(1, 0, 0, 1);
    m_glColorMaskG = GLColorMaskNode::create(0, 1, 0, 1);
    m_glColorMaskAll = GLColorMaskNode::create(1, 1, 1, 1);
    CC_SAFE_RETAIN(m_rt);
    CC_SAFE_RETAIN(m_pcd);
    CC_SAFE_RETAIN(m_glColorMaskR);
    CC_SAFE_RETAIN(m_glColorMaskG);
    CC_SAFE_RETAIN(m_glColorMaskAll);
    return true;
}

CollisionManager::~CollisionManager() {
    CC_SAFE_RELEASE(m_rt);
    CC_SAFE_RELEASE(m_pcd);
    CC_SAFE_RELEASE(m_glColorMaskR);
    CC_SAFE_RELEASE(m_glColorMaskG);
    CC_SAFE_RELEASE(m_glColorMaskAll);
}

void CollisionManager::testCollision(
        GameObject* sp0,
        GameObject* sp1) {
    auto bbox0 = cocos2d::utils::getCascadeBoundingBox(sp0);
    auto bbox1 = cocos2d::utils::getCascadeBoundingBox(sp1);
    if (!bbox0.intersectsRect(bbox1)) {
        return;
    }
    float tempX;
    float tempY;
    float tempWidth;
    float tempHeight;
    if (bbox0.getMinX() < bbox1.getMinX()) {
        tempX = bbox1.getMinX();
        tempWidth = bbox0.getMaxX() - bbox1.getMinX();
    } else {
        tempX = bbox0.getMinX();
        tempWidth = bbox1.getMaxX() - bbox0.getMinX();
    }
    if (bbox0.getMinY() > bbox1.getMinY()) {
        tempY = bbox0.getMinY();
        tempHeight = bbox1.getMaxY() - bbox0.getMinY();
    } else {
        tempY = bbox1.getMinY();
        tempHeight = bbox0.getMaxY() - bbox1.getMinY();
    }
    cocos2d::Rect intersection(tempX, tempY, tempWidth, tempHeight);
    m_pcd->setRect(intersection);
    m_pcd->setSprites(sp0, sp1);
    m_rt->beginWithClear(0, 0, 0, 0);
    m_glColorMaskR->visit();
    sp0->visit();
    m_glColorMaskG->visit();
    sp1->visit();
    m_pcd->visit();
    m_glColorMaskAll->visit();
    m_rt->end();
    m_renderer->render(); // XXX: enforce sync behaviour
}

void CollisionManager::sync() {
    m_renderer->render(); // XXX: enforce sync behaviour
}
