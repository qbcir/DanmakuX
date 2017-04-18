#ifndef __cocos2dx_PHYTREE_H__
#define __cocos2dx_PHYTREE_H__

#include "cocos2d.h"
#include "json/document.h"
#include <array>

class PhyTree;
class PhyTreeNode : public cocos2d::Node {
public:
    //
    void setTree(PhyTree* tree);
    float getBubbleSize() const;
    float getNodeSpan() const;
    int getDepth() const;
    void setDepth(int depth);
    float getAngle() const;
    void setAngle(float angle);
    float getRadius() const;
    void setRadius(float radius);
    bool isLeafNode() const;
    PhyTreeNode* getParentNode();
    void addChildNode(PhyTreeNode* node);
    void setParentNode(PhyTreeNode* node);
    cocos2d::Vector<PhyTreeNode*>& getChildrenNodes();
    //
    static PhyTreeNode* createFromJson(PhyTree* tree, int depth, const rapidjson::Value& el);
private:
    bool initWithJson(PhyTree* tree, int depth, const rapidjson::Value& el);
    int m_depth = 1;
    float m_textAlign = 0;
    float m_textAngle = 0;
    float m_angle = 0;
    float m_radius = 0;
    cocos2d::Vec2 m_position;
    PhyTree* m_tree = nullptr;
    PhyTreeNode* m_parentNode = nullptr;
    cocos2d::Vector<PhyTreeNode*> m_children;
};

class PhyTree : public cocos2d::Node
{
public:
    enum class Spacing {
        FixedStep,
        INVALID = -1
    };
    /* */
    void setRadial(bool radial);
    bool isRadial() const;
    bool atLeastOneDimensionFixed() const;
    /* Spacing */
    float getSpacingX() const;
    float getSpacingY() const;
    const cocos2d::Vec2& getSpacing() const;
    void setSpacingX(float s, bool skipRender = false);
    void setSpacingY(float s, bool skipRender = false);
    void setSpacing(const cocos2d::Vec2& s, bool skipRender = false);
    /* */
    void addNode(PhyTreeNode* node);
    /* */
    static PhyTree* createFromJson(const std::string& fname);
private:
    bool initWithJson(const std::string& fname);
    void processInternalNode(PhyTreeNode* node);
    float treeLayout(PhyTreeNode* node);
    float separation(PhyTreeNode* node1, PhyTreeNode* node2);
    void placeNodes();
    void placeNodesRadial();
    void cartesianToPolar(PhyTreeNode* node, float radius, float annular_shift);
    void setupTopBottomSpacing();
    void setupLeftRightSpacing();
    int m_maxDepth = std::numeric_limits<int>::max();
    float m_maxRadius = 100;
    float m_rescaleNodeSpan = 0;
    float m_annularLimit = 0;
    float m_labelWidth = 0;
    float m_fontSize = 12;
    float m_shownFontSize = 0;
    bool m_radial = true;
    bool m_doScaling = false;
    bool m_drawSizeBubbles = false;
    float m_leftOffset = 0;
    float m_compression = 0;
    Spacing m_topBottomSpacing;
    Spacing m_leftRightSpacing;
    std::array<std::array<float, 2>, 2> m_extents = {{{{0.0f, 0.0f}}, {{0.0f, 0.0f}}}};
    cocos2d::Vec2 m_size = { 1, 1 };
    cocos2d::Vec2 m_offsets = { 0.0f, 12.0f };
    cocos2d::Vec2 m_scales = {1.0f, 1.0f};
    cocos2d::Vec2 m_fixedWidth = {15.0f, 20.0f};
    cocos2d::Vector<PhyTreeNode*> m_nodes;
    /* Temp */
    bool m_underCollapsedParent = false;
    float m_x = 0.0f;
    float m_saveX = 0.0f;
    float m_lastSpan = 0.0f;
    float m_saveSpan = 0.0f;
    PhyTreeNode* m_lastNode = nullptr;
};

#endif // __cocos2dx_PHYTREE_H__
