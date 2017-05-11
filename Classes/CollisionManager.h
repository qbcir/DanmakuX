#ifndef __DX_COLLISIONMANAGER_H__
#define __DX_COLLISIONMANAGER_H__

#include "cocos2d.h"
#include "Objects/GameObject.h"

class CollisionManager {
public:
    class DebugDraw : public b2Draw {
    public:
        void setDrawNode(cocos2d::DrawNode* drawNode);
        void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
        /// Draw a solid closed polygon provided in CCW order.
        void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
        /// Draw a circle.
        void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color);
        /// Draw a solid circle.
        void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color);
        /// Draw a line segment.
        void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);
        /// Draw a transform. Choose your own length scale.
        void DrawTransform(const b2Transform& xf);
        void DrawPoint(const b2Vec2& p, float32 size, const b2Color& color);
    private:
        cocos2d::DrawNode* m_drawNode = nullptr;
    };
    class ContactListener : public b2ContactListener {
    public:
        void BeginContact(b2Contact* contact) override;
    };
    typedef std::vector<b2Vec2> TriangleT;
    typedef std::vector<TriangleT> TrianglesT;
    typedef std::unordered_map<std::string, TrianglesT> TrianglesMapT;
    static CollisionManager* getInstance();
    void loadPolygonData(const std::string& filename);
    const TrianglesT& getPolygonData(const std::string& sp);
    b2ContactListener *getContactListener();
    b2Draw* getDebugDraw();
    void setDrawNode(cocos2d::DrawNode* drawNode);
protected:
    TrianglesMapT m_polygons;
    ContactListener m_contactListener;
    DebugDraw m_debugDraw;
};

#endif /* __DX_COLLISIONMANAGER_H__ */
