#include "GameManager.h"
#include "CollisionManager.h"
#include "Data/DataParser.h"

CollisionManager* CollisionManager::getInstance() {
    return GameManager::getInstance()->getCollisionManager();
}

void CollisionManager::loadPolygonData(const std::string& filename) {
    m_polygons = parseFromJson<TrianglesMapT>(filename);
}

const CollisionManager::TrianglesT &CollisionManager::getPolygonData(const std::string& sp) {
    auto fu = cocos2d::FileUtils::getInstance();
    auto it = m_polygons.find(sp);
    if (it == std::end(m_polygons)) {
        cocos2d::log("No such resource %s", sp.c_str());
        exit(1);
    }
    return it->second;
}

b2ContactListener* CollisionManager::getContactListener() {
    return &m_contactListener;
}

b2Draw* CollisionManager::getDebugDraw() {
    return &m_debugDraw;
}

void CollisionManager::setDrawNode(cocos2d::DrawNode* drawNode) {
    m_debugDraw.setDrawNode(drawNode);
}

void CollisionManager::ContactListener::BeginContact(b2Contact* contact) {
    auto fixA = contact->GetFixtureA();
    auto fixB = contact->GetFixtureB();
    auto objA = static_cast<GameObject*>(fixA->GetUserData());
    auto objB = static_cast<GameObject*>(fixA->GetUserData());
    objA->onCollision(objB);
    objB->onCollision(objA);
}

void CollisionManager::DebugDraw::setDrawNode(cocos2d::DrawNode* drawNode) {
    m_drawNode = drawNode;
}

void CollisionManager::DebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
    std::vector<cocos2d::Point> verts;
    for (int32 i = 0; i < vertexCount; ++i) {
        const b2Vec2& v = vertices[i];
        verts.push_back(cocos2d::Point(v.x, v.y));
    }
    m_drawNode->drawPoly(verts.data(), verts.size(), true, cocos2d::Color4F::GREEN);
}

/// Draw a solid closed polygon provided in CCW order.
void CollisionManager::DebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
    std::vector<cocos2d::Point> verts;
    for (int32 i = 0; i < vertexCount; ++i) {
        const b2Vec2& v = vertices[i];
        verts.push_back(cocos2d::Point(v.x, v.y));
    }
    m_drawNode->drawPoly(verts.data(), verts.size(), true, cocos2d::Color4F::GREEN);
}

/// Draw a circle.
void CollisionManager::DebugDraw::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) {
}

/// Draw a solid circle.
void CollisionManager::DebugDraw::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color) {
//    cocos2d::log("DrawSolidCircle");
}

/// Draw a line segment.
void CollisionManager::DebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) {
    m_drawNode->drawLine(cocos2d::Vec2(p1.x, p1.y),
        cocos2d::Vec2(p2.x, p2.y), cocos2d::Color4F::GREEN);
}

/// Draw a transform. Choose your own length scale.
void CollisionManager::DebugDraw::DrawTransform(const b2Transform& xf) {
    //cocos2d::log("DrawTransform");
}

void CollisionManager::DebugDraw::DrawPoint(const b2Vec2& p, float32 size, const b2Color& color) {
    m_drawNode->drawDot(cocos2d::Vec2(p.x, p.y), 1, cocos2d::Color4F::MAGENTA);
}
